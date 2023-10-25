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
#include <include/game_state.hpp>
#include <include/network.hpp>
#include <include/cloud.hpp>
#include <include/sizes.hpp>
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
  FallUpdate();
  RunUpdate();
  HitboxUpdate();
  ChuteHitboxUpdate();
  DeathUpdate();

  mFallAnim.Update();
  mChuteAnim.Update();
  mRunAnim.Update();
  mAngelAnim.Update();
}

void
Plane::Pilot::Move(
  const PLANE_PITCH inputDir )
{
  const auto moveDir =
    inputDir == PLANE_PITCH::PITCH_LEFT
    ? -1.0f : 1.0f;

  if ( mIsChuteOpen == true )
  {
    mChuteState = static_cast <CHUTE_STATE> (inputDir);
    mX += moveDir * sizes.pilot_chute_speed * deltaTime;

    return;
  }

  if ( mIsRunning == false )
    return;

  mDir = moveDir == PLANE_PITCH::PITCH_LEFT ? 90 : 270;
  mX += moveDir * sizes.pilot_run_speed * deltaTime;

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
  mX = planeX;
  mY = planeY;
  mDir = bailDir;

  if ( mDir < 0 )
    mDir += 360;
  else if ( mDir >= 360 )
    mDir -= 360;

  mGravity = sizes.pilot_gravity;
  mSpeed = sizes.pilot_eject_speed;
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
  mGravity = sizes.pilot_chute_gravity;


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
  if ( mIsDead == true || mIsRunning == true )
    return;


  if ( mSpeed > 0.0f )
    mX += mSpeed * sin( mDir * M_PI / 180.0f ) * deltaTime;


  if ( mIsChuteOpen == true )
  {
    playSound(sounds.chute, plane->type(), true);

    if ( mVSpeed > 0.0f )
      mVSpeed = -mGravity * 0.4f;

    else if ( mVSpeed < -sizes.screen_height * 0.08f )
    {
      mVSpeed += sizes.screen_height * 0.5f * deltaTime; // 0.375 ?

      if ( mVSpeed > -sizes.screen_height * 0.08f )
        mVSpeed = -sizes.screen_height * 0.08f;
    }

    else if ( mVSpeed > -sizes.screen_height * 0.08f )
      mVSpeed -= sizes.screen_height * 0.5f * deltaTime;

    if ( mSpeed > 0.0f )
      mSpeed -= mSpeed * 2.0f * deltaTime;
  }
  else
  {
    playSound(sounds.fall, plane->type(), true);

    if ( mVSpeed > 0.0f )
    {
      mGravity += mGravity * 2.0f * deltaTime;
      mVSpeed -= mGravity * deltaTime;

      if ( mVSpeed < 0.0f )
        mGravity = sizes.screen_height * 0.24f; //!!
    }
    else
    {
      mGravity += mGravity * deltaTime;
      mVSpeed -= mGravity * deltaTime;
    }
  }

  mY -= mVSpeed * deltaTime;


  if ( mSpeed > sizes.screen_width * 0.008f )
    mSpeed -= mSpeed * deltaTime;
  else
    mSpeed = 0.0f;


  if ( mSpeed < 0.0f )
    mSpeed = 0.0f;


  if ( mX < 0.0f )
    mX = sizes.screen_width;
  else if ( mX > sizes.screen_width )
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
  if ( mIsRunning == false || mIsDead == true )
    return;


  if ( mX < 0.0f )
    mX = sizes.screen_width;

  else if ( mX > sizes.screen_width )
    mX = 0.0f;


  if ( plane->mIsLocal == false )
    return;


  if (  mX <= sizes.barn_x_pilot_left_collision ||
        mX >= sizes.barn_x_pilot_right_collision )
    return;

  Rescue();
}

void
Plane::Pilot::DeathUpdate()
{
  if ( mIsDead == false )
    return;


  mY -= sizes.angel_ascent_speed * deltaTime;

  if ( mAngelAnim.isReady() == false )
    return;

  mAngelAnim.Start();

  if ( mAngelFrame == 3 )
  {
    if ( plane->mIsLocal == false )
      return;

    plane->Respawn();

    eventPush(EVENTS::PLANE_RESPAWN);
  }
  else
    ++mAngelFrame;

  if ( mAngelLoop < 6 && mAngelFrame > 2 )
  {
    mAngelFrame = 0;
    ++mAngelLoop;
  }
  else if ( mAngelLoop == 6 && mAngelFrame == 2 )
    mAngelFrame = 3;
}

void
Plane::Pilot::AnimationsUpdate()
{
  if ( mIsDead == true )
    return DeathAnimUpdate();


  if ( mIsRunning == true )
    RunAnimUpdate();

  else
  {
    if ( mIsChuteOpen == true )
      ChuteAnimUpdate();
    else
      FallAnimUpdate();
  }


  if ( gameState().debug.collisions == true )
  {
    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &mHitbox );
  }
}

void
Plane::Pilot::AnimationsReset()
{
  mFallAnim.Stop();
  mFallFrame = 0;

  mChuteAnim.Stop();
  mChuteState = CHUTE_STATE::CHUTE_NONE;

  mRunAnim.Stop();
  mRunFrame = 0;

  mAngelAnim.Stop();
  mAngelFrame = 0;
  mAngelLoop = 0;

  mFallAnim.SetNewTimeout( sizes.pilot_fall_frame_time );
  mChuteAnim.SetNewTimeout( sizes.chute_frame_time );
  mRunAnim.SetNewTimeout( sizes.pilot_run_frame_time );
  mAngelAnim.SetNewTimeout( sizes.angel_frame_time );
}

void
Plane::Pilot::FallAnimUpdate()
{
  if ( mFallAnim.isReady() == true )
  {
    mFallAnim.Start();
    mFallFrame = !mFallFrame;
  }


  auto* const pilotTexture =
    plane->mType == PLANE_TYPE::RED
    ? textures.anim_pilot_fall_r
    : textures.anim_pilot_fall_b;

  const SDL_Rect pilotRect
  {
    mX - sizes.pilot_sizex / 2.0f,
    mY - sizes.pilot_sizey / 2.0f,
    sizes.pilot_sizex,
    sizes.pilot_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    pilotTexture,
    &textures.anim_pilot_fall_rect[mFallFrame],
    &pilotRect );
}

void
Plane::Pilot::ChuteAnimUpdate()
{
  if ( mChuteState >= CHUTE_STATE::CHUTE_DESTROYED )
    return;


  const SDL_Rect chuteRect
  {
    mX - sizes.chute_sizex / 2.0f,
    mY - sizes.chute_sizey * 1.375f,
    sizes.chute_sizex,
    sizes.chute_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.anim_chute,
    &textures.anim_chute_rect[mChuteState],
    &chuteRect );


  if ( mChuteAnim.isReady() == true )
  {
    mChuteAnim.Start();
    mFallFrame = !mFallFrame;
  }


  auto* const pilotTexture =
    plane->mType == PLANE_TYPE::RED
    ? textures.anim_pilot_fall_r
    : textures.anim_pilot_fall_b;

  const SDL_Rect pilotRect
  {
    mX - sizes.pilot_sizex / 2.0f,
    mY - sizes.pilot_sizey / 2.0f,
    sizes.pilot_sizex,
    sizes.pilot_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    pilotTexture,
    &textures.anim_pilot_fall_rect[mFallFrame],
    &pilotRect );


  if ( gameState().debug.collisions == true )
  {
    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &mChuteHitbox );
  }
}

void
Plane::Pilot::RunAnimUpdate()
{
  const SDL_Rect pilotRect
  {
    mX - sizes.pilot_sizex / 2.0f,
    mY - sizes.pilot_sizey / 2.0f,
    sizes.pilot_sizex,
    sizes.pilot_sizey,
  };

  auto* const pilotTexture =
    plane->mType == PLANE_TYPE::RED
    ? textures.anim_pilot_run_r
    : textures.anim_pilot_run_b;

  if ( mDir == 270 )
  {
    SDL_RenderCopy(
      gRenderer,
      pilotTexture,
      &textures.anim_pilot_run_rect[mRunFrame],
      &pilotRect );

    return;
  }

  SDL_RenderCopyEx(
    gRenderer,
    pilotTexture,
    &textures.anim_pilot_run_rect[mRunFrame],
    &pilotRect,
    0.0,
    nullptr,
    SDL_FLIP_HORIZONTAL );
}

void
Plane::Pilot::DeathAnimUpdate()
{
  const SDL_Rect angelRect
  {
    mX - sizes.angel_sizex / 2.0f,
    mY - sizes.angel_sizey / 2.0f,
    sizes.angel_sizex,
    sizes.angel_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.anim_pilot_angel,
    &textures.anim_pilot_angel_rect[mAngelFrame],
    &angelRect );
}

void
Plane::Pilot::HitboxUpdate()
{
  if ( mIsDead == true )
    return;


  mHitbox =
  {
    mX - sizes.pilot_sizex / 2.0f,
    mY - sizes.pilot_sizey / 2.0f,
    sizes.pilot_sizex,
    sizes.pilot_sizey,
  };
}

void
Plane::Pilot::ChuteHitboxUpdate()
{
  if ( mIsDead == true || mIsChuteOpen == false )
    return;


  mChuteHitbox =
  {
    mX - sizes.chute_sizex / 2.0f,
    mY - sizes.chute_sizey * 1.375f,
    sizes.chute_sizex,
    sizes.chute_sizey,
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
  if (  mY <= sizes.ground_y_pilot_collision ||
        plane->mIsLocal == false )
    return;


//  SURVIVE LANDING
  if ( mVSpeed >= -sizes.pilot_chute_gravity )
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
  mY = sizes.ground_y_pilot_collision;

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
