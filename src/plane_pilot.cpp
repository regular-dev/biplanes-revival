/*
  Biplanes Revival
  Copyright (C) 2019-2023 Regular-dev community
  https://regular-dev.org
  regular.dev.org@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <include/plane.hpp>
#include <include/sdl.hpp>
#include <include/enums.hpp>
#include <include/time.hpp>
#include <include/timer.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/network.hpp>
#include <include/cloud.hpp>
#include <include/sounds.hpp>
#include <include/textures.hpp>

#include <cmath>


Plane::Pilot::Pilot()
{
  HitboxUpdate();
}

void
Plane::Pilot::setPlane(
  Plane* parentPlane )
{
  plane = parentPlane;
}

void
Plane::Pilot::Update()
{
  FallUpdate();
  RunUpdate();
  HitboxUpdate();
  ChuteHitboxUpdate();
  DeathUpdate();
}

void
Plane::Pilot::Draw() const
{
  namespace pilot = constants::pilot;
  namespace chute = pilot::chute;
  namespace angel = pilot::angel;


  if ( plane->hasJumped() == false )
    return;


  if ( mIsDead == true )
  {
    const SDL_FRect angelRect
    {
      toWindowSpaceX(mX - 0.5f * angel::sizeX),
      toWindowSpaceY(mY - 0.5f * angel::sizeY),
      scaleToScreenX(angel::sizeX),
      scaleToScreenY(angel::sizeY),
    };

    SDL_RenderCopyF(
      gRenderer,
      textures.anim_pilot_angel,
      &textures.anim_pilot_angel_rect[mAngelFrame],
      &angelRect );

    return;
  }


  if ( mIsChuteOpen == true )
  {
    const SDL_FRect chuteRect
    {
      toWindowSpaceX(mX - 0.5f * chute::sizeX),
      toWindowSpaceY(mY - chute::offsetY),
      scaleToScreenX(chute::sizeX),
      scaleToScreenY(chute::sizeY),
    };

    SDL_RenderCopyF(
      gRenderer,
      textures.anim_chute,
      &textures.anim_chute_rect[mChuteState],
      &chuteRect );
  }


  const SDL_FRect pilotRect
  {
    toWindowSpaceX(mX - 0.5f * pilot::sizeX),
    toWindowSpaceY(mY - 0.5f * pilot::sizeY),
    scaleToScreenX(pilot::sizeX),
    scaleToScreenY(pilot::sizeY),
  };

  if ( mIsRunning == true )
  {
    auto* const pilotTexture =
      plane->mType == PLANE_TYPE::RED
      ? textures.anim_pilot_run_r
      : textures.anim_pilot_run_b;

    if ( mDir == 270 )
    {
      SDL_RenderCopyF(
        gRenderer,
        pilotTexture,
        &textures.anim_pilot_run_rect[mRunFrame],
        &pilotRect );
    }
    else
    {
      SDL_RenderCopyExF(
        gRenderer,
        pilotTexture,
        &textures.anim_pilot_run_rect[mRunFrame],
        &pilotRect,
        0.0,
        nullptr,
        SDL_FLIP_HORIZONTAL );
    }
  }
  else
  {
    auto* const pilotTexture =
      plane->mType == PLANE_TYPE::RED
      ? textures.anim_pilot_fall_r
      : textures.anim_pilot_fall_b;

    SDL_RenderCopyF(
      gRenderer,
      pilotTexture,
      &textures.anim_pilot_fall_rect[mFallFrame],
      &pilotRect );
  }
}

void
Plane::Pilot::DrawCollisionLayer() const
{
  namespace colors = constants::colors::debug::collisions;

  if ( mIsDead == true || plane->mHasJumped == false )
    return;


  if ( mIsChuteOpen == true )
  {
    const SDL_FRect chuteHitbox
    {
      toWindowSpaceX(mChuteHitbox.x),
      toWindowSpaceY(mChuteHitbox.y),
      scaleToScreenX(mChuteHitbox.w),
      scaleToScreenY(mChuteHitbox.h),
    };

    setRenderColor(colors::bulletToChute);
    SDL_RenderDrawRectF( gRenderer, &chuteHitbox );
  }

  const SDL_FRect hitbox
  {
    toWindowSpaceX(mHitbox.x),
    toWindowSpaceY(mHitbox.y),
    scaleToScreenX(mHitbox.w),
    scaleToScreenY(mHitbox.h),
  };

  setRenderColor(colors::pilotToBullet);
  SDL_RenderDrawRectF( gRenderer, &hitbox );
}

void
Plane::Pilot::Move(
  const PLANE_PITCH inputDir )
{
  namespace pilot = constants::pilot;
  namespace chute = pilot::chute;


  const auto moveDir =
    inputDir == PLANE_PITCH::PITCH_LEFT
    ? -1.0f : 1.0f;

  if ( mIsChuteOpen == true )
  {
    mChuteState = static_cast <CHUTE_STATE> (inputDir);
    mMoveSpeed = moveDir * chute::baseSpeedX;

    return;
  }

  if ( mIsRunning == false )
    return;

  mDir = moveDir == PLANE_PITCH::PITCH_LEFT ? 90 : 270;
  mMoveSpeed = moveDir * pilot::runSpeed;

  if ( mRunAnim.isReady() == false )
    return;

  mRunAnim.Start();
  ++mRunFrame;

  if ( mRunFrame > 2 )
    mRunFrame = 0;
}

void
Plane::Pilot::MoveIdle()
{
  mMoveSpeed = 0.0f;

  if ( mIsRunning == true )
  {
    mRunAnim.Stop();
    mRunFrame = 0;

    return;
  }

  if ( mChuteState < CHUTE_STATE::CHUTE_DESTROYED )
    mChuteState = CHUTE_STATE::CHUTE_IDLE;
}

// PILOT EJECT
void
Plane::Pilot::Bail(
  const float planeX,
  const float planeY,
  const float bailDir )
{
  namespace pilot = constants::pilot;


  mX = planeX;
  mY = planeY;
  mDir = bailDir;

  if ( mDir < 0 )
    mDir += 360;
  else if ( mDir >= 360 )
    mDir -= 360;

  mGravity = pilot::gravity;
  mSpeed.x =  pilot::ejectSpeed * std::sin(mDir * M_PI / 180.0);
  mSpeed.y = -pilot::ejectSpeed * std::cos(mDir * M_PI / 180.0);
  mMoveSpeed = 0.0f;


  if ( gameState().isRoundFinished == false )
    plane->mStats.jumps++;

  if ( plane->mIsLocal == true )
    eventPush(EVENTS::EJECT);
}

void
Plane::Pilot::OpenChute()
{
  if (  mIsDead == true ||
        mIsChuteOpen == true ||
        mIsRunning == true ||
        mChuteState != CHUTE_STATE::CHUTE_IDLE )
    return;


  mIsChuteOpen = true;
  mGravity = constants::pilot::chute::gravity;


  if ( plane->mIsLocal == true )
    eventPush(EVENTS::EJECT);
}

void
Plane::Pilot::ChuteUnlock()
{
  if (  plane->mHasJumped == true &&
        mIsDead == false &&
        mChuteState == CHUTE_STATE::CHUTE_NONE )
    mChuteState = CHUTE_STATE::CHUTE_IDLE;
}

void
Plane::Pilot::CoordinatesWrap()
{
  mX = std::fmod( std::fmod(mX, 1.0f) + 1.0f, 1.0f );
}

void
Plane::Pilot::FallUpdate()
{
  namespace pilot = constants::pilot;


  if ( mIsDead == true || mIsRunning == true )
    return;


  const SDL_FPoint currentPos {mX, mY};

  mX += (mSpeed.x + mMoveSpeed) * deltaTime;


//  TODO: do something with this magic mess

  if ( mIsChuteOpen == true )
  {
    const auto chuteChannel = playSound(
      sounds.chute, mAudioLoopChannel, true );

    if ( chuteChannel != -1 )
      mAudioLoopChannel = chuteChannel;

    panSound(mAudioLoopChannel, mX);

    if ( mSpeed.y < 0.0f )
      mSpeed.y = 0.4f * mGravity;

    else if ( mSpeed.y > pilot::chute::baseSpeedY )
    {
      mSpeed.y -= 0.5f * deltaTime; // 0.375 ?
      mSpeed.y = std::max(mSpeed.y, 0.08f);
    }

    else if ( mSpeed.y < pilot::chute::baseSpeedY )
      mSpeed.y += 0.5f * deltaTime;
  }
  else
  {
    const auto fallChannel = playSound(
      sounds.fall, mAudioLoopChannel, true );

    if ( fallChannel != -1 )
      mAudioLoopChannel = fallChannel;

    panSound(mAudioLoopChannel, mX);

    if ( mSpeed.y <= 0.0f )
    {
      mGravity += mGravity * 2.0f * deltaTime;
      mSpeed.y += mGravity * deltaTime;

      if ( mSpeed.y >= 0.0f )
        mGravity = 0.24f; //!!
    }
    else
    {
      mGravity += mGravity * deltaTime;
      mSpeed.y += mGravity * deltaTime;
    }
  }

  mY += mSpeed.y * deltaTime;


  if ( std::abs(mSpeed.x) > pilot::maxSpeedXThreshold )
  {
    const float slowdownFactor = mIsChuteOpen == true
      ? pilot::chuteSpeedSlowdownFactor   // = 0.5 seconds to reach zero
      : pilot::airSpeedSlowdownFactor;    // = 1.0 seconds to reach zero

    mSpeed.x += -slowdownFactor * mSpeed.x * deltaTime;
  }
  else
    mSpeed.x = 0.0f;


  mSpeedVec =
  {
    mX - currentPos.x,
    mY - currentPos.y,
  };

  CoordinatesWrap();


//  THINK: autoland remote pilot ?
  if ( plane->mIsLocal == false )
    return;


  HitGroundCheck();

  if ( plane->mIsBot == true )
    return;


  for ( auto& cloud : clouds )
  {
    if ( cloud.isHit( mX, mY ) == true )
      cloud.setTransparent();
    else
      cloud.setOpaque();
  }
}

void
Plane::Pilot::RunUpdate()
{
  namespace barn = constants::barn;


  if ( mIsRunning == false || mIsDead == true )
    return;


  const SDL_FPoint currentPos {mX, mY};

  mX += mMoveSpeed * deltaTime;

  mSpeedVec =
  {
    mX - currentPos.x,
    mY - currentPos.y,
  };

  CoordinatesWrap();

  if ( plane->mIsLocal == false )
    return;


  if (  mX <= barn::pilotCollisionLeftX ||
        mX >= barn::pilotCollisionRightX )
    return;

  Rescue();
}

void
Plane::Pilot::DeathUpdate()
{
  namespace angel = constants::pilot::angel;


  if ( mIsDead == false )
    return;


  mY -= angel::ascentRate * deltaTime;

  if ( mAngelAnim.isReady() == false )
    return;


  mAngelAnim.Start();

  if ( mAngelFrame == angel::framePastLoopId )
  {
    if ( plane->mIsLocal == true )
    {
      plane->Respawn();
      eventPush(EVENTS::PLANE_RESPAWN);
    }

    return;
  }


  ++mAngelFrame;

  if ( mAngelFrame != angel::framePastLoopId )
    return;


  if ( mAngelLoop < angel::loopCount )
  {
    ++mAngelLoop;
    mAngelFrame = 0;
  }
}

void
Plane::Pilot::AnimationsUpdate()
{
  if ( mIsDead == true )
    return mAngelAnim.Update();


  if ( mIsRunning == true )
    return mRunAnim.Update();


  if ( mIsChuteOpen == true )
    ChuteAnimUpdate();
  else
    FallAnimUpdate();
}

void
Plane::Pilot::AnimationsReset()
{
  namespace pilot = constants::pilot;
  namespace chute = pilot::chute;
  namespace angel = pilot::angel;


  mFallAnim.Stop();
  mFallFrame = 0;

  mChuteAnim.Stop();
  mChuteState = CHUTE_STATE::CHUTE_NONE;

  mRunAnim.Stop();
  mRunFrame = 0;

  mAngelAnim.Stop();
  mAngelFrame = 0;
  mAngelLoop = 0;

  mFallAnim.SetNewTimeout( pilot::fallFrameTime );
  mChuteAnim.SetNewTimeout( chute::frameTime );
  mRunAnim.SetNewTimeout( pilot::runFrameTime );
  mAngelAnim.SetNewTimeout( angel::frameTime );
}

void
Plane::Pilot::FallAnimUpdate()
{
  mFallAnim.Update();

  if ( mFallAnim.isReady() == true )
  {
    mFallAnim.Start();
    mFallFrame = !mFallFrame;
  }
}

void
Plane::Pilot::ChuteAnimUpdate()
{
  if ( mChuteState >= CHUTE_STATE::CHUTE_DESTROYED )
    return;


  mChuteAnim.Update();

  if ( mChuteAnim.isReady() == true )
  {
    mChuteAnim.Start();
    mFallFrame = !mFallFrame;
  }
}

void
Plane::Pilot::HitboxUpdate()
{
  namespace pilot = constants::pilot;


  if ( mIsDead == true )
    return;


  mHitbox =
  {
    mX - 0.5f * pilot::sizeX,
    mY - 0.5f * pilot::sizeY,
    pilot::sizeX,
    pilot::sizeY,
  };
}

void
Plane::Pilot::ChuteHitboxUpdate()
{
  namespace chute = constants::pilot::chute;


  if ( mIsDead == true || mIsChuteOpen == false )
    return;


  mChuteHitbox =
  {
    mX - 0.5f * chute::sizeX,
    mY - chute::offsetY,
    chute::sizeX,
    chute::sizeY,
  };
}

void
Plane::Pilot::FadeLoopingSounds()
{
  if ( mAudioLoopChannel != -1 )
  {
    Mix_FadeOutChannel(mAudioLoopChannel, constants::audioFadeDuration);
    mAudioLoopChannel = -1;
  }
}

float
Plane::Pilot::x() const
{
  if ( plane->hasJumped() == false )
    return plane->x();

  return mX;
}

float
Plane::Pilot::y() const
{
  if ( plane->hasJumped() == false )
    return plane->y();

  return mY;
}

void
Plane::Pilot::setX(
  const float x )
{
  if ( plane->mHasJumped == true )
    mX = x;
}

void
Plane::Pilot::setY(
  const float y )
{
  if ( plane->mHasJumped == true )
    mY = y;
}

SDL_FPoint
Plane::Pilot::speedVec() const
{
  return mSpeedVec;
}

void
Plane::Pilot::ChuteHit(
  Plane& attacker )
{
  FadeLoopingSounds();

  panSound(
    playSound(sounds.hitChute, -1, false),
    mX );

  mChuteState = CHUTE_STATE::CHUTE_DESTROYED;
  mIsChuteOpen = false;
  mFallAnim.Stop();


  if ( gameState().isRoundFinished == false )
    attacker.mStats.chute_hits++;
}

void
Plane::Pilot::Death()
{
  FadeLoopingSounds();

  panSound(
    playSound(sounds.dead, -1, false),
    mX );

  mIsRunning = false;
  mIsChuteOpen = false;
  mIsDead = true;

  mDir = 0;
  mSpeed = {};
  mGravity = 0.0f;
  mSpeedVec = {};
  mChuteState = CHUTE_STATE::CHUTE_NONE;
}

void
Plane::Pilot::Kill(
  Plane& killedBy )
{
  Death();
  killedBy.ScoreChange(2);


  if ( gameState().isRoundFinished == false )
    killedBy.mStats.pilot_hits++;
}

void
Plane::Pilot::HitGroundCheck()
{
  namespace pilot = constants::pilot;
  namespace chute = pilot::chute;


  if (  mY <= pilot::groundCollision ||
        plane->mIsLocal == false )
    return;


//  SURVIVE LANDING
  if ( mSpeed.y <= chute::gravity )
  {
    FallSurvive();
    eventPush(EVENTS::PILOT_LAND);

    return;
  }


//  THINK: autoland remote pilot ?
//  $REF: if self is remote
//    return;


  Death();
  plane->ScoreChange(-1);


  if ( gameState().isRoundFinished == false )
    plane->mStats.falls++;

  eventPush(EVENTS::PILOT_DEATH);
}

void
Plane::Pilot::FallSurvive()
{
  FadeLoopingSounds();

  mY = constants::pilot::groundCollision;

  mIsRunning = true;
  mIsChuteOpen = false;

  mSpeed = {};
  mGravity = 0.0f;
  mSpeedVec = {};
  mChuteState = CHUTE_STATE::CHUTE_IDLE;
}

void
Plane::Pilot::Rescue()
{
  plane->Respawn();

  panSound(
    playSound(sounds.rescue, -1, false),
    plane->mX );

  if ( gameState().isRoundFinished  == false )
    plane->mStats.rescues++;

  if ( plane->mIsLocal == true )
    eventPush(EVENTS::PILOT_RESPAWN);
}

void
Plane::Pilot::Respawn()
{
  FadeLoopingSounds();

  mIsRunning = false;
  mIsChuteOpen = false;
  mIsDead = false;

  mDir = 0;
  mGravity = 0.0f;
  mSpeed = {};
  mSpeedVec = {};

  AnimationsReset();
}

bool
Plane::Pilot::isDead() const
{
  return mIsDead;
}

bool
Plane::Pilot::isChuteOpen() const
{
  return mIsChuteOpen;
}

bool
Plane::Pilot::isChuteBroken() const
{
  return mChuteState == CHUTE_STATE::CHUTE_DESTROYED;
}

bool
Plane::Pilot::isRunning() const
{
  return mIsRunning;
}

bool
Plane::Pilot::isHit(
  const float x,
  const float y ) const
{
  if ( mIsDead == true || plane->mHasJumped == false )
    return false;


  const SDL_FPoint hitPoint {x, y};

  return SDL_PointInFRect(&hitPoint, &mHitbox);
}

bool
Plane::Pilot::ChuteIsHit(
  const float x,
  const float y ) const
{
  if ( mIsChuteOpen == false )
    return false;


  const SDL_FPoint hitPoint {x, y};

  return SDL_PointInFRect(&hitPoint, &mChuteHitbox);
}
