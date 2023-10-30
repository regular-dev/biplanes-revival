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
#include <include/utility.hpp>

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
  mPrevX = mX;
  mPrevY = mY;

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
  namespace colors = constants::colors;


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


  if ( gameState().debug.collisions == true )
  {
    const SDL_FRect chuteHitbox
    {
      toWindowSpaceX(mChuteHitbox.x),
      toWindowSpaceY(mChuteHitbox.y),
      scaleToScreenX(mChuteHitbox.w),
      scaleToScreenY(mChuteHitbox.h),
    };

    setRenderColor(colors::bulletHitbox);
    SDL_RenderDrawRectF( gRenderer, &chuteHitbox );

    const SDL_FRect hitbox
    {
      toWindowSpaceX(mHitbox.x),
      toWindowSpaceY(mHitbox.y),
      scaleToScreenX(mHitbox.w),
      scaleToScreenY(mHitbox.h),
    };

    setRenderColor(colors::bulletHitbox);
    SDL_RenderDrawRectF( gRenderer, &hitbox );
  }

  if ( gameState().debug.aiInputs == true )
  {
    SDL_Point dot = getClosestCollision();

//    TODO: choose more suitable color
    setRenderColor(colors::bulletHitbox);
    SDL_RenderDrawLine(
      gRenderer,
      toWindowSpaceX(mX),
      toWindowSpaceY(mY),
      toWindowSpaceX(dot.x),
      toWindowSpaceY(dot.y) );
  }
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
    mX += moveDir * chute::speed * deltaTime;

    return;
  }

  if ( mIsRunning == false )
    return;

  mDir = moveDir == PLANE_PITCH::PITCH_LEFT ? 90 : 270;
  mX += moveDir * pilot::runSpeed * deltaTime;

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
  mPrevX = mX;
  mPrevY = mY;
  mDir = bailDir;

  if ( mDir < 0 )
    mDir += 360;
  else if ( mDir >= 360 )
    mDir -= 360;

  mGravity = pilot::gravity;
  mSpeed = pilot::ejectSpeed;
  mVSpeed = mSpeed * cos( mDir * M_PI / 180.0 );


  if ( gameState().isRoundFinished == false )
  {
    plane->mStats.jumps++;
    log_message("Pilot::Bail() plane->mStats.jumps++\n");
  }

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
Plane::Pilot::FallUpdate()
{
  namespace pilot = constants::pilot;


  if ( mIsDead == true || mIsRunning == true )
    return;


  if ( mSpeed > 0.0f )
    mX += mSpeed * sin( mDir * M_PI / 180.0f ) * deltaTime;


//  TODO: do something with this magic mess

  if ( mIsChuteOpen == true )
  {
    playSound(sounds.chute, plane->type(), true);

    if ( mVSpeed > 0.0f )
      mVSpeed = -mGravity * 0.4f;

    else if ( mVSpeed < -0.08f )
    {
      mVSpeed += 0.5f * deltaTime; // 0.375 ?

      if ( mVSpeed > -0.08f )
        mVSpeed = -0.08f;
    }

    else if ( mVSpeed > -0.08f )
      mVSpeed -= 0.5f * deltaTime;

    if ( mSpeed > 0.0f )
      mSpeed -= 2.0f * mSpeed * deltaTime;
  }
  else
  {
    playSound(sounds.fall, plane->type(), true);

    if ( mVSpeed > 0.0f )
    {
      mGravity += mGravity * 2.0f * deltaTime;
      mVSpeed -= mGravity * deltaTime;

      if ( mVSpeed < 0.0f )
        mGravity = 0.24f; //!!
    }
    else
    {
      mGravity += mGravity * deltaTime;
      mVSpeed -= mGravity * deltaTime;
    }
  }

  mY -= mVSpeed * deltaTime;


  if ( mSpeed > pilot::maxFallSpeedX )
    mSpeed -= mSpeed * deltaTime;
  else
    mSpeed = 0.0f;


  if ( mSpeed < 0.0f )
    mSpeed = 0.0f;


  if ( mX < 0.0f )
    mX = 1.0f;
  else if ( mX > 1.0f )
    mX = 0.0f;


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


  if ( mX < 0.0f )
    mX = 1.0f;

  else if ( mX > 1.0f )
    mX = 0.0f;


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
  else
  {
//    TODO: if frame == 2 -> set frame to frame count
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
    toWindowSpaceX(mX - 0.5f * pilot::sizeX),
    toWindowSpaceY(mY - 0.5f * pilot::sizeY),
    scaleToScreenX(pilot::sizeX),
    scaleToScreenY(pilot::sizeY),
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
    toWindowSpaceX(mX - 0.5f * chute::sizeX),
    toWindowSpaceY(mY - chute::offsetY),
    scaleToScreenX(chute::sizeX),
    scaleToScreenY(chute::sizeY),
  };
}

bool
Plane::Pilot::ChuteIsHit(
  const float x,
  const float y ) const
{
  if ( mIsChuteOpen == false )
    return false;


  const SDL_Point hitPoint {x, y};

  return SDL_PointInRect(&hitPoint, &mChuteHitbox);
}

float
Plane::Pilot::x() const
{
  return mX;
}

float
Plane::Pilot::y() const
{
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


void
Plane::Pilot::ChuteHit(
  Plane& attacker )
{
  playSound(sounds.hitChute, -1, false);
  mChuteState = CHUTE_STATE::CHUTE_DESTROYED;
  mIsChuteOpen = false;
  mFallAnim.Stop();


  if ( gameState().isRoundFinished == false )
  {
    attacker.mStats.chute_hits++;
    log_message("Pilot::ChuteHit() attacker.mStats.chute_hits++\n");
  }
}

void
Plane::Pilot::Death()
{
  playSound(sounds.dead, -1, false);

  mIsRunning = false;
  mIsChuteOpen = false;
  mIsDead = true;

  mDir = 0;
  mSpeed = 0.0f;
  mVSpeed = 0.0f;
  mGravity = 0.0f;
  mChuteState = CHUTE_STATE::CHUTE_NONE;
}

void
Plane::Pilot::Kill(
  Plane& killedBy )
{
  Death();
  killedBy.ScoreChange(2);


  if ( gameState().isRoundFinished == false )
  {
    killedBy.mStats.pilot_hits++;
    log_message("Pilot::Kill() killedBy.mStats.pilot_hits++\n");
  }
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
  if ( mVSpeed >= -chute::gravity )
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
  {
    plane->mStats.falls++;
    log_message("Pilot::HitGroundCheck() plane->mStats.falls++\n");
  }

  eventPush(EVENTS::PILOT_DEATH);
}

void
Plane::Pilot::FallSurvive()
{
  mY = constants::pilot::groundCollision;

  mIsRunning = true;
  mIsChuteOpen = false;

  mSpeed = 0.0f;
  mVSpeed = 0.0f;
  mGravity = 0.0f;
  mChuteState = CHUTE_STATE::CHUTE_IDLE;
}

void
Plane::Pilot::Rescue()
{
  playSound(sounds.rescue, plane->mType, false);

  plane->Respawn();

  if ( gameState().isRoundFinished  == false )
  {
    plane->mStats.rescues++;
    log_message("Pilot::Rescue() plane->mStats.rescues++\n");
  }

  if ( plane->mIsLocal == true )
    eventPush(EVENTS::PILOT_RESPAWN);
}

void
Plane::Pilot::Respawn()
{
  mIsRunning = false;
  mIsChuteOpen = false;
  mIsDead = false;

  mDir = 0;
  mGravity = 0.0f;
  mSpeed = 0.0f;
  mVSpeed = 0.0f;

  AnimationsReset();
}


bool
Plane::Pilot::isDead() const
{
  return mIsDead;
}

bool
Plane::Pilot::isHit(
  const float x,
  const float y ) const
{
  if ( mIsDead == true || plane->mHasJumped == false )
    return false;


  const SDL_Point hitPoint {x, y};

  return SDL_PointInRect(&hitPoint, &mHitbox);
}

SDL_Point
Plane::Pilot::getClosestCollision() const
{
  namespace barn = constants::barn;
  namespace pilot = constants::pilot;

//  Distance to barn | ground
  return
  {
    std::max( barn::pilotCollisionLeftX,
              std::min( mX, barn::pilotCollisionRightX ) ),
    std::max( pilot::groundCollision,
              std::min( mY, pilot::groundCollision ) ),
  };
}
