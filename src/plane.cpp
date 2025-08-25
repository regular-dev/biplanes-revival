/*
  Biplanes Revival
  Copyright (C) 2019-2025 Regular-dev community
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
#include <include/time.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/sdl.hpp>
#include <include/render.hpp>
#include <include/cloud.hpp>
#include <include/bullet.hpp>
#include <include/math.hpp>
#include <include/menu.hpp>
#include <include/network.hpp>
#include <include/network_data.hpp>
#include <include/effects.hpp>
#include <include/stats.hpp>
#include <include/sounds.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>

#include <lib/SDL_Vector.h>

#include <cmath>


Plane::Plane(
  const PLANE_TYPE construct_type )
  : mType{construct_type}
{
}

void
Plane::Update()
{
  if (  mIsDead == true &&
        mIsLocal == true &&
        mHasJumped == false &&
        mDeadCooldown.isReady() == true )
  {
    Respawn();
    eventPush(EVENTS::PLANE_RESPAWN);
  }

  SpeedUpdate();
  CoordinatesUpdate();
  CollisionsUpdate();
  AbandonedUpdate();

  if ( mHasJumped == true )
    pilot.Update();

  mPitchCooldown.Update();
  mShootCooldown.Update();

  mDeadCooldown.Update();
  mProtection.Update();

  AnimationsUpdate();
}

void
Plane::Draw() const
{
  namespace plane = constants::plane;


  if ( mIsDead == true )
    return;


  const SDL_FRect planeRect
  {
    toWindowSpaceX(mX - 0.5f * plane::sizeX),
    toWindowSpaceY(mY - 0.5f * plane::sizeY),
    scaleToScreenX(plane::sizeX),
    scaleToScreenY(plane::sizeY),
  };

  auto* planeTexture {textures.plane_blue};
  double textureAngle {mDir - 90.0};

  if ( mType == PLANE_TYPE::RED )
  {
    planeTexture = textures.plane_red;
    textureAngle = mDir + 90.0;
  }

  if ( mProtection.isReady() == false )
    SDL_SetTextureAlphaMod( planeTexture, 127 );

  SDL_RenderCopyExF(
    gRenderer,
    planeTexture,
    nullptr,
    &planeRect,
    textureAngle,
    nullptr,
    SDL_FLIP_NONE );

  SDL_SetTextureAlphaMod( planeTexture, 255 );


  DrawFire();
}

void
Plane::DrawCollisionLayer() const
{
  namespace plane = constants::plane;
  namespace colors = constants::colors::debug::collisions;

  if ( mIsDead == true )
    return;


  const auto planeHitbox = Hitbox();

  const SDL_FRect hitbox
  {
    toWindowSpaceX(planeHitbox.x),
    toWindowSpaceY(planeHitbox.y),
    scaleToScreenX(planeHitbox.w),
    scaleToScreenY(planeHitbox.h),
  };

  setRenderColor(colors::planeToBullet);
  SDL_RenderDrawRectF( gRenderer, &hitbox );


  const SDL_FRect planeCenter
  {
    toWindowSpaceX(mX - 0.05f * plane::sizeX),
    toWindowSpaceY(mY - 0.05f * plane::sizeY),
    scaleToScreenX(0.1f * plane::sizeX),
    scaleToScreenY(0.1f * plane::sizeY),
  };

  setRenderColor(colors::planeToObstacles);
  SDL_RenderDrawRectF( gRenderer, &planeCenter );

  const auto hitboxOffset = constants::plane::hitboxOffset;
  const float dir = mDir * M_PI / 180.0f;

  const SDL_Vector hitboxCenter
  {
    mX + hitboxOffset * std::sin(dir),
    mY - hitboxOffset * std::cos(dir),
  };

  draw_circle(hitboxCenter.x, hitboxCenter.y, plane::hitboxRadius);
}

void
Plane::DrawFire() const
{
  namespace fire = constants::fire;


  if ( mHp > 0 )
    return;


  const SDL_FRect textureRect
  {
    toWindowSpaceX(mX - 0.5f * fire::sizeX),
    toWindowSpaceY(mY - 0.5f * fire::sizeY),
    scaleToScreenX(fire::sizeX),
    scaleToScreenY(fire::sizeY),
  };

  const double textureAngle =
    mType == PLANE_TYPE::RED
    ? mDir + 90.0
    : mDir - 90.0;

  const auto textureFlip =
    mType == PLANE_TYPE::BLUE
    ? SDL_FLIP_NONE
    : SDL_FLIP_HORIZONTAL;

  SDL_RenderCopyExF(
    gRenderer,
    textures.anim_fire,
    &textures.anim_fire_rect[mFireFrame],
    &textureRect,
    textureAngle,
    nullptr,
    textureFlip );
}

void
Plane::Accelerate()
{
  namespace plane = constants::plane;


  if ( mIsDead == true || mHasJumped == true )
    return;


  if ( mIsOnGround == true )
  {
    if ( mIsTakingOff == false )
      TakeOffStart();

    mSpeed += plane::takeoffAcceleration * deltaTime;

    return;
  }


  if ( mDir != 0.0f )
  {
    if ( mDir == 22.5f || mDir == 337.5f )
      mSpeed += 0.25f * plane::acceleration * deltaTime;

    else if ( mDir == 45.0f || mDir == 315.0f )
      mSpeed += 0.5f * plane::acceleration * deltaTime;

    else
      mSpeed += 0.75f * plane::acceleration * deltaTime;

    mSpeed = std::min(mSpeed, mMaxSpeedVar);
  }
}

void
Plane::Decelerate()
{
  namespace plane = constants::plane;


  if ( mIsDead == true || mHasJumped == true )
    return;


  if ( mIsTakingOff == true )
  {
    mSpeed -= plane::takeoffDeceleration * deltaTime;
    return;
  }


  mSpeed -= plane::deceleration * deltaTime;

  mSpeed = std::max(mSpeed, 0.0f);

  mMaxSpeedVar = std::max(mSpeed, plane::maxSpeedBase);
}

void
Plane::Turn( const PLANE_PITCH inputDir )
{
  if ( mIsDead == true || mIsOnGround == true )
    return;

  if ( mPitchCooldown.isReady() == false )
    return;


  mPitchCooldown.Start();

  const float dir =
    inputDir == PLANE_PITCH::PITCH_LEFT
    ? -1.0f : 1.0f;

  mDir += dir * constants::plane::pitchStep;

  mDir = clamp_angle(mDir, 360.f);
}

void
Plane::Shoot()
{
  if (  mHasJumped == true ||
        mIsDead == true ||
        mIsOnGround == true ||
        mProtection.isReady() == false )
    return;

  if (  mIsLocal == true &&
        mShootCooldown.isReady() == false )
    return;


  mShootCooldown.Start();

  panSound( playSound(sounds.shoot), mX );

  const auto bulletOffset = bulletSpawnOffset();

  bullets.SpawnBullet(
    mX + bulletOffset.x,
    mY + bulletOffset.y,
    mDir,
    mType );


  if ( mIsLocal == true )
    eventPush(EVENTS::SHOOT);

  if ( gameState().isRoundFinished == false )
    mStats.shots++;
}

void
Plane::Jump()
{
  if (  mIsDead == true ||
        mIsOnGround == true ||
        mProtection.isReady() == false )
    return;


  mHasJumped = true;
  pilot.Bail(mX, mY, jumpDir());


  if ( mIsLocal == false )
    pilot.ChuteUnlock();
}


void
Plane::SpeedUpdate()
{
  namespace plane = constants::plane;


  if ( mIsOnGround == true )
    return TakeOffUpdate();


//  Decrease vertical speed
  if ( mDir <= 70 || mDir >= 290 )
  {
    if ( mDir == 0 )                          // 90 climb
    {
      mSpeed -= 0.225f * plane::acceleration * deltaTime;
      mMaxSpeedVar = mSpeed;
    }
    else if ( mDir <= 25 || mDir >= 330 )     // 75 climb
    {
      mSpeed -= 0.100f * plane::acceleration * deltaTime;
      mMaxSpeedVar = mSpeed;
    }
    else if ( mDir <= 50 || mDir >= 310 )     // 45 climb
    {
      mSpeed -= 0.065f * plane::acceleration * deltaTime;
      mMaxSpeedVar = mSpeed;
    }
    else                                      // 30 climb
    {
      mSpeed -= 0.020f * plane::acceleration * deltaTime;
      mMaxSpeedVar = mSpeed;
    }

    mMaxSpeedVar = std::max(
      mMaxSpeedVar, plane::maxSpeedBase );

    mSpeed = std::max(mSpeed, 0.0f);

    return;
  }

//  Increase vertical speed
  if ( mDir > 113 && mDir < 246 )
  {
    if ( mSpeed < plane::maxSpeedBoosted )
    {
      mSpeed += plane::diveAcceleration * deltaTime;

      if ( mSpeed > mMaxSpeedVar )
      {
        mMaxSpeedVar = std::min(
          mSpeed, plane::maxSpeedBoosted );

        mSpeed = mMaxSpeedVar;
      }

      return;
    }

    mSpeed = plane::maxSpeedBoosted;
    mMaxSpeedVar = mSpeed;
  }
}

void
Plane::CoordinatesUpdate()
{
  if ( mIsDead == true )
    return;

  if ( mIsOnGround == true && mIsTakingOff == false )
    return;


  const SDL_FPoint currentPos {mX, mY};

//  Change coordinates
  mX += mSpeed * std::sin( mDir * M_PI / 180.0f ) * deltaTime;


  if ( mIsOnGround == false )
  {
    mY -= mSpeed * std::cos( mDir * M_PI / 180.0f ) * deltaTime;

//    Gravity
    if ( mSpeed < mMaxSpeedVar )
      mY += ( mMaxSpeedVar - mSpeed ) * deltaTime;
  }

  mSpeedVec =
  {
    mX - currentPos.x,
    mY - currentPos.y,
  };

  CoordinatesWrap();
}

void
Plane::CoordinatesWrap()
{
  mX = std::fmod( std::fmod(mX, 1.0f) + 1.0f, 1.0f );

  mY = std::max(mY, 0.0f);
}

void
Plane::CollisionsUpdate()
{
  namespace barn = constants::barn;


  if (  mIsLocal == false ||
        mIsDead == true ||
        (mIsOnGround == true && mIsTakingOff == false) )
    return;


  const bool collidesWithBarn
  {
    mY > barn::planeCollisionY &&
    mX > barn::planeCollisionX &&
    mX < barn::planeCollisionX + barn::sizeX
  };

  const bool collidesWithGround
  {
    mIsOnGround == false &&
    mY > constants::plane::groundCollision
  };

  if  ( collidesWithBarn == true ||
        collidesWithGround == true )
  {
    Crash();
    eventPush(EVENTS::PLANE_DEATH);
  }
}

// UPDATE ABANDONED PLANE
void
Plane::AbandonedUpdate()
{
  namespace plane = constants::plane;


  if (  mIsDead == true ||
        mHasJumped == false ||
        mDir == 180.0f )
    return;


  if ( mSpeed > plane::maxSpeedAbandoned )
    mSpeed -= plane::abandonedDeceleration * deltaTime;


  if ( mPitchCooldown.isReady() == false )
    return;


  mPitchCooldown.Start();

  mDir += plane::pitchStep * (mDir < 180.f ? 1.f : -1.f);
}

void
Plane::AnimationsReset()
{
  namespace ai = constants::ai;
  namespace plane = constants::plane;
  namespace fire = constants::fire;
  namespace smoke = constants::smoke;


  mSmokeFrame = 0;
  mSmokeAnim.Stop();
  mSmokeCooldown.Stop();

  mFireAnim.Stop();
  mFireFrame = 0;

  if (  mIsBot == true &&
        gameState().botDifficulty == DIFFICULTY::EASY )
    mShootCooldown.SetNewTimeout( ai::shootCooldownEasy );
  else
    mShootCooldown.SetNewTimeout( plane::shootCooldown );

  mPitchCooldown.SetNewTimeout( plane::pitchCooldown );

  mDeadCooldown.SetNewTimeout( plane::deadCooldown );
  mProtection.SetNewTimeout( plane::spawnProtectionCooldown );

  mSmokeAnim.SetNewTimeout( smoke::frameTime );
  mSmokeCooldown.SetNewTimeout( smoke::cooldown );
  mFireAnim.SetNewTimeout( fire::frameTime );

  pilot.AnimationsReset();
}

void
Plane::AnimationsUpdate()
{
  if ( mHasJumped == true )
    pilot.AnimationsUpdate();


  if ( mIsDead == true )
    return;


  SmokeUpdate();
  FireUpdate();
}

void
Plane::SmokeUpdate()
{
  if ( mHp > 1 )
    return;


  mSmokeAnim.Update();
  mSmokeCooldown.Update();

  if ( mSmokeCooldown.isReady() == true )
  {
    mSmokeCooldown.Start();
    mSmokeAnim.Stop();
    mSmokeFrame = 0;
  }


  if ( mSmokeAnim.isReady() == false )
    return;


  if ( mSmokeFrame < constants::smoke::frameCount )
  {
    effects.Spawn(new SmokePuff{mX, mY});
    mSmokeAnim.Start();
    ++mSmokeFrame;
  }
}

void
Plane::FireUpdate()
{
  if ( mHp > 0 )
    return;


  mFireAnim.Update();

  if ( mFireAnim.isReady() == true )
  {
    mFireAnim.Start();
    ++mFireFrame;

    if ( mFireFrame > 2 )
      mFireFrame = 0;
  }
}

SDL_FRect
Plane::Hitbox() const
{
  namespace plane = constants::plane;

  return
  {
    mX - 0.5f * plane::hitboxSizeX,
    mY - 0.5f * plane::hitboxSizeY,
    plane::hitboxSizeX,
    plane::hitboxSizeY,
  };
}

void
Plane::TakeOffUpdate()
{
  namespace plane = constants::plane;


  if ( mIsTakingOff == false )
    return;

  mSpeed += 0.75f * plane::acceleration * deltaTime;

  if ( mSpeed >= plane::maxSpeedBase )
    TakeOffFinish();
}

void
Plane::TakeOffStart()
{
  if ( mIsTakingOff == true )
    return;

  mIsTakingOff = true;

  mDir = mType == PLANE_TYPE::BLUE
    ? constants::plane::takeoffDirectionBlue
    : constants::plane::takeoffDirectionRed;
}

void
Plane::TakeOffFinish()
{
  mIsTakingOff = false;
  mIsOnGround = false;
}

void
Plane::Hit(
  Plane& attacker )
{
  if ( mIsDead == true )
    return;


  if ( mIsLocal == true )
  {
    if ( mProtection.isReady() == false )
      return;

    eventPush(EVENTS::HIT_PLANE);
  }


  const auto& game = gameState();

  if ( game.isRoundFinished == false )
    attacker.mStats.plane_hits++;

  if ( game.features.oneShotKills == true )
    mHp = 0;

  if ( mHp > 0 )
  {
    panSound( playSound(sounds.hitPlane), mX );

    --mHp;

    if ( mHp == 1 )
    {
      mSmokeAnim.Start();
      mSmokeCooldown.Start();
    }
    else if ( mHp == 0 )
      mFireAnim.Start();

    return;
  }

  Explode();


  if ( mHasJumped == true )
    return;


  if ( gameState().isRoundFinished == false )
  {
    mStats.plane_deaths++;
    attacker.mStats.plane_kills++;
  }

  attacker.ScoreChange(1);
}

// DIE
void
Plane::Explode()
{
  namespace spark = constants::explosion::spark;


  panSound( playSound(sounds.explosion), mX );

  const auto sparkDirFactor =
    std::sin(mDir * M_PI / 180.f);

  const auto sparkSpeedFactor =
    mSpeed / constants::plane::maxSpeedBoosted;

  const auto sparkDirOffset =
    spark::dirOffset * sparkDirFactor * sparkSpeedFactor;


  mSpeed = 0.0f;
  mDir = 0.0f;
  mHp = 0;

  mIsDead = true;
  mIsOnGround = false;
  mIsTakingOff = false;

  mProtection.Stop();
  mPitchCooldown.Stop();
  mShootCooldown.Stop();
  mDeadCooldown.Start();

  effects.Spawn(new Explosion{mX, mY});

  for ( size_t i = 0; i < spark::count; ++i )
  {
    const auto instanceFactor =
      static_cast <float> (i) / spark::count;

    const auto dir =
      sparkDirOffset + spark::dirRange *
      ( -0.5f + instanceFactor +
        0.45f * sparkDirFactor / spark::count );

    const auto speedVariation = std::fmod(
      spark::speedMask, instanceFactor );

    const auto sparkSpeed =
      spark::speedMin + speedVariation * spark::speedRange;

    const float sparkDir = dir * M_PI / 180.f;

    effects.Spawn(new ExplosionSpark{
      mX, mY, sparkSpeed, sparkDir });
  }


  mX = 0.0f;
  mY = 0.0f;
  mSpeedVec = {};
}

void
Plane::Crash()
{
  if ( mIsDead == true )
    return;


  Explode();


  if ( mHasJumped == true )
    return;

  ScoreChange(-1);


  if ( gameState().isRoundFinished == false )
    mStats.crashes++;
}

void
Plane::Respawn()
{
  namespace plane = constants::plane;


  mIsDead = false;
  mHp = plane::maxHp;
  mIsOnGround = true;
  mIsTakingOff = false;
  mSpeed = 0.0f;
  mMaxSpeedVar = plane::maxSpeedBase;
  mSpeedVec = {};

  mDeadCooldown.Stop();
  mPitchCooldown.Stop();
  mShootCooldown.Stop();
  mProtection.Stop();

  mY = plane::spawnY;

  mX = mType == PLANE_TYPE::BLUE
    ? plane::spawnBlueX
    : plane::spawnRedX;

  mDir = mType == PLANE_TYPE::BLUE
    ? plane::spawnRotationBlue
    : plane::spawnRotationRed;

  mHasJumped = false;

  pilot.Respawn();

  AnimationsReset();

  ResetSpawnProtection();
}

void
Plane::ResetSpawnProtection()
{
  const auto& opponentPlane =
    planes.at(static_cast <PLANE_TYPE> (!mType));

  if (  opponentPlane.mIsOnGround == false &&
        opponentPlane.mIsDead == false )
    mProtection.Start();
  else
    mProtection.Stop();
}

void
Plane::ResetScore()
{
  mScore = 0;
}

void
Plane::ScoreChange(
  const int8_t deltaScore )
{
  if ( mScore + deltaScore < 0 )
    ResetScore();
  else
    mScore += deltaScore;

  auto& game = gameState();

  if (  game.winScore == 0 ||
        mScore < game.winScore ||
        game.isRoundFinished == true )
    return;


  auto& opponentPlane =
    planes.at(static_cast <PLANE_TYPE> (!mType));

  if ( mIsLocal == true )
  {
    if ( mIsBot == true )
    {
      if ( opponentPlane.isBot() == false )
      {
        playSound(sounds.defeat);
        menu.setMessage(MESSAGE_TYPE::GAME_LOST);
      }
      else
      {
        playSound(sounds.victory);

        if ( mType == PLANE_TYPE::BLUE )
          menu.setMessage(MESSAGE_TYPE::BLUE_SIDE_WON);
        else
          menu.setMessage(MESSAGE_TYPE::RED_SIDE_WON);

//      TODO: finish bot match
      }
    }
    else
    {
      playSound(sounds.victory);

      if ( game.gameMode != GAME_MODE::HUMAN_VS_HUMAN_HOTSEAT )
        menu.setMessage(MESSAGE_TYPE::GAME_WON);
      else
      {
        if ( mType == PLANE_TYPE::BLUE )
          menu.setMessage(MESSAGE_TYPE::BLUE_SIDE_WON);
        else
          menu.setMessage(MESSAGE_TYPE::RED_SIDE_WON);
      }
    }
  }
  else
  {
    playSound(sounds.defeat);
    menu.setMessage(MESSAGE_TYPE::GAME_LOST);
  }


  mStats.wins++;
  opponentPlane.mStats.losses++;

  // Track victory against DEVELOPER bot for INSANE unlock
  if ( game.gameMode == GAME_MODE::HUMAN_VS_BOT &&
       mIsBot == false &&
       opponentPlane.isBot() == true &&
       game.botDifficulty == DIFFICULTY::DEVELOPER )
    mStats.wins_vs_developer++;

  // Track victories against INSANE bot
  if ( game.gameMode == GAME_MODE::HUMAN_VS_BOT &&
       mIsBot == false &&
       opponentPlane.isBot() == true &&
       game.botDifficulty == DIFFICULTY::INSANE )
    mStats.wins_vs_insane++;

  game.isRoundFinished = true;

  updateRecentStats();

  if ( game.gameMode == GAME_MODE::HUMAN_VS_BOT ||
       game.gameMode == GAME_MODE::HUMAN_VS_HUMAN )
    updateTotalStats();
}

void
Plane::ResetStats()
{
  mStats = {};
}

const Statistics&
Plane::stats() const
{
  return mStats;
}

bool
Plane::isHit(
  const float x,
  const float y ) const
{
  if ( mIsDead == true )
    return false;

  if ( mProtection.isReady() == false )
    return false;


  if ( gameState().features.alternativeHitboxes == false )
  {
    const SDL_FPoint hitPoint {x, y};
    const auto hitbox = Hitbox();

    return SDL_PointInFRect(&hitPoint, &hitbox);
  }

  const auto hitboxOffset = constants::plane::hitboxOffset;
  const float dir = mDir * M_PI / 180.0f;

  const SDL_Vector hitboxCenter
  {
    mX + hitboxOffset * std::sin(dir),
    mY - hitboxOffset * std::cos(dir),
  };

  const auto distance =
    (SDL_Vector {x, y} - hitboxCenter).length();

  return distance <= constants::plane::hitboxRadius;
}

bool
Plane::isInCloud(
  const Cloud& cloud ) const
{
  if ( isDead() == true || mIsLocal == false )
    return false;


  if ( mHasJumped == true )
    return pilot.isInCloud(cloud);


  if ( mIsBot == true &&
       gameState().gameMode != GAME_MODE::BOT_VS_BOT )
    return false;


  return cloud.isHit(mX, mY);
}

PLANE_TYPE
Plane::type() const
{
  return mType;
}

uint8_t
Plane::score() const
{
  return mScore;
}

uint8_t
Plane::hp() const
{
  return mHp;
}

float
Plane::protectionRemainder() const
{
  if (  mIsDead == true ||
        mHasJumped == true )
    return 0.0f;

  return mProtection.remainderTime();
}

void
Plane::setLocal(
  const bool local )
{
  mIsLocal = local;
}

bool
Plane::isLocal() const
{
  return mIsLocal;
}

void
Plane::setBot(
  const bool bot )
{
  mIsBot = bot;
}

bool
Plane::isBot() const
{
  return mIsBot;
}

float
Plane::x() const
{
  return mX;
}

float
Plane::y() const
{
  return mY;
}

float
Plane::dir() const
{
  return mDir;
}

float
Plane::jumpDir() const
{
  namespace plane = constants::plane;

  return mType == PLANE_TYPE::RED
          ? clamp_angle(mDir + plane::jumpDirOffsetRed, 360.f)
          : clamp_angle(mDir + plane::jumpDirOffsetBlue, 360.f);
}

float
Plane::speed() const
{
  return mSpeed;
}

float
Plane::maxSpeed() const
{
  return mMaxSpeedVar;
}

SDL_FPoint
Plane::speedVector() const
{
  return mSpeedVec;
}

SDL_FPoint
Plane::bulletSpawnOffset() const
{
  if ( gameState().features.alternativeHitboxes == false )
    return {};


  const auto offset = constants::plane::bulletSpawnOffset;
  const float dir = mDir * M_PI / 180.0f;

  return
  {
    offset * std::sin(dir),
    -offset * std::cos(dir),
  };
}

// Get coordinates for sending
PlaneNetworkData
Plane::getNetworkData() const
{
  return
  {
    mX,
    mY,
    mDir,
    pilot.x(),
    pilot.y(),
  };
}

bool
Plane::isDead() const
{
  return
    pilot.isDead() == true ||
    ( mHasJumped == false && mIsDead == true );
}

bool
Plane::hasJumped() const
{
  return mHasJumped;
}

bool
Plane::isAirborne() const
{
  return mIsOnGround == false;
}

bool
Plane::isTakingOff() const
{
  return mIsTakingOff == true;
}

bool
Plane::canAccelerate() const
{
  return
    isDead() == false &&
    mHasJumped == false &&
    mDir != 0.0f &&
    mSpeed < mMaxSpeedVar;
}

bool
Plane::canDecelerate() const
{
  return
    isDead() == false &&
    mHasJumped == false &&
    (mIsOnGround == false || mIsTakingOff == true);
}

bool
Plane::canTurn() const
{
  return
    isDead() == false &&
    ( ( mIsOnGround == false &&
        mHasJumped == false &&
        mPitchCooldown.isReady() == true ) ||
      pilot.mIsRunning == true ||
      pilot.mIsChuteOpen == true );
}

bool
Plane::canShoot() const
{
  return
    isDead() == false &&
    mHasJumped == false &&
    mIsOnGround == false &&
    mProtection.isReady() == true &&
    mShootCooldown.isReady() == true;
}

bool
Plane::canJump() const
{
  return
    isDead() == false &&
    mIsOnGround == false &&
    mProtection.isReady() == true &&
    pilot.mIsRunning == false &&
    pilot.mIsChuteOpen == false &&
    ( mHasJumped == false ||
      pilot.mChuteState == CHUTE_IDLE );
}

void
Plane::setCoords(
  const PlaneNetworkData& data )
{
  mX = data.x;
  mY = data.y;
}

void
Plane::setDir(
  const float new_dir )
{
  mDir = new_dir;
}
