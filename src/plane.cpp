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
#include <include/time.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/sdl.hpp>
#include <include/cloud.hpp>
#include <include/bullet.hpp>
#include <include/menu.hpp>
#include <include/network.hpp>
#include <include/network_data.hpp>
#include <include/effects.hpp>
#include <include/stats.hpp>
#include <include/sounds.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>
#include <include/utility.hpp>

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

  mPrevX = mX;
  mPrevY = mY;

  SpeedUpdate();
  CoordinatesUpdate();
  CollisionsUpdate();
  AbandonedUpdate();

  if ( mHasJumped == true )
    pilot.Update();

  HitboxUpdate();

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
  namespace colors = constants::colors;


  if ( mIsDead == true )
    return;


  const SDL_FRect planeRect
  {
    toWindowSpaceX(mX - 0.5f * plane::sizeX),
    toWindowSpaceY(mY - 0.5f * plane::sizeY),
    scaleToScreenX(plane::sizeX),
    scaleToScreenY(plane::sizeY),
  };

  auto* planeTexture {textures.texture_biplane_b};
  double textureAngle {mDir - 90.0};

  if ( mType == PLANE_TYPE::RED )
  {
    planeTexture = textures.texture_biplane_r;
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


  if ( gameState().debug.collisions == true )
  {
    const SDL_FRect hitbox
    {
      toWindowSpaceX(mHitbox.x),
      toWindowSpaceY(mHitbox.y),
      scaleToScreenX(mHitbox.w),
      scaleToScreenY(mHitbox.h),
    };

    setRenderColor(colors::bulletHitbox);
    SDL_RenderDrawRectF( gRenderer, &hitbox );

    const SDL_FRect planeCenter
    {
      toWindowSpaceX(mX - 0.05f * plane::sizeX),
      toWindowSpaceY(mY - 0.05f * plane::sizeY),
      scaleToScreenX(0.1f * plane::sizeX),
      scaleToScreenY(0.1f * plane::sizeY),
    };

    setRenderColor(colors::planeHitbox);
    SDL_RenderDrawRectF( gRenderer, &planeCenter );
  }


  if ( mHasJumped == false && gameState().debug.aiInputs == true )
  {
    setRenderColor(colors::bulletHitbox);

    SDL_Point dot = getClosestCollision();

    SDL_RenderDrawLine(
      gRenderer,
      toWindowSpaceX(mX),
      toWindowSpaceY(mY),
      toWindowSpaceX(dot.x),
      toWindowSpaceY(dot.y) );
  }
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

    mSpeed += 0.85f * plane::acceleration * deltaTime;

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

    if ( mSpeed > mMaxSpeedVar )
      mSpeed = mMaxSpeedVar;
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
    mSpeed -= 0.75f * plane::acceleration * deltaTime;
    return;
  }


  mSpeed -= 0.5f * plane::acceleration * deltaTime;


  if ( mSpeed < 0.0f )
    mSpeed = 0.0f;

  if ( mMaxSpeedVar <= plane::maxSpeedBase )
    return;


  mMaxSpeedVar = mSpeed;

  if ( mMaxSpeedVar < plane::maxSpeedBase )
    mMaxSpeedVar = plane::maxSpeedBase;
}

void
Plane::Turn( const PLANE_PITCH inputDir )
{
  if ( mIsDead == true || mIsOnGround == true )
    return;

  if ( mPitchCooldown.isReady() == false )
    return;


  mPitchCooldown.Start();

  float dir =
    inputDir == PLANE_PITCH::PITCH_LEFT
    ? -1.0f : 1.0f;

  mDir += dir * constants::plane::pitchStep;

  if ( mDir < 0.0f )
    mDir += 360.0f;

  else if ( mDir >= 360.0f )
    mDir -= 360.0f;
}

void
Plane::Shoot()
{
//  THINK: is spawn protection condition needed ?
//  THINK: is shoot cooldown condition needed ?
  if (  mHasJumped == true ||
        mIsDead == true ||
        mIsOnGround == true ||
        mProtection.isReady() == false )
    return;

  if (  mIsLocal == true &&
        mShootCooldown.isReady() == false )
    return;


  mShootCooldown.Start();
  playSound(sounds.shoot, -1, false);

  bullets.SpawnBullet(mX, mY, mDir, mType);


  if ( mIsLocal == true )
    eventPush(EVENTS::SHOOT);

  if ( gameState().isRoundFinished == false )
  {
    mStats.shots++;
    log_message("Plane::Shoot() mStats.shots++\n");
  }
}

void
Plane::Jump()
{
  if (  mIsDead == true ||
        mIsOnGround == true ||
        mProtection.isReady() == false )
    return;


  const float jumpDir =
    mType == PLANE_TYPE::RED
    ? mDir + 90.0f
    : mDir - 90.0f;

  mHasJumped = true;
  pilot.Bail(mX, mY, jumpDir);


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

    if ( mMaxSpeedVar < plane::maxSpeedBase )
      mMaxSpeedVar = plane::maxSpeedBase;

    if ( mSpeed < 0.0f )
      mSpeed = 0.0f;

    return;
  }

//  Increase vertical speed
  if ( mDir > 113 && mDir < 246 )
  {
    if ( mSpeed < plane::maxSpeedBoosted )
    {
      mSpeed += 0.2f * plane::acceleration * deltaTime;

      if ( mSpeed > mMaxSpeedVar )
      {
        mMaxSpeedVar = mSpeed;

        if ( mMaxSpeedVar > plane::maxSpeedBoosted )
        {
          mMaxSpeedVar = plane::maxSpeedBoosted;
          mSpeed = mMaxSpeedVar;
        }
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


//  Change coordinates
  mX += mSpeed * sin( mDir * M_PI / 180.0f ) * deltaTime;


  if ( mIsOnGround == false )
  {
    mY -= mSpeed * cos( mDir * M_PI / 180.0f ) * deltaTime;

//    Gravity
    if ( mSpeed < mMaxSpeedVar )
      mY += ( mMaxSpeedVar - mSpeed ) * deltaTime;
  }

  CoordinatesClamp();
}

void
Plane::CoordinatesClamp()
{
  if ( mX > 1.0f )
    mX -= 1.0f;
  else if ( mX < 0.0f )
    mX += 1.0f;


  if ( mY < 0.0f )
    mY = 0.0f;
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


  if ( mHasJumped == true || mIsBot == true )
    return;


  for ( auto& cloud : clouds )
  {
    if ( cloud.isHit(mX, mY) == true )
      cloud.setTransparent();
    else
      cloud.setOpaque();
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


  if ( mSpeed > 0.5f * plane::maxSpeedBase )
    mSpeed -= 0.2f * plane::acceleration * deltaTime;


  if ( mPitchCooldown.isReady() == false )
    return;


  mPitchCooldown.Start();

  mDir += mDir < 180.0f ? plane::pitchStep : -plane::pitchStep;
}

void
Plane::AnimationsReset()
{
  namespace plane = constants::plane;
  namespace fire = constants::fire;
  namespace smoke = constants::smoke;


  mSmokeFrame = 0;
  mSmokeAnim.Stop();
  mSmokeCooldown.Stop();

  mFireAnim.Stop();
  mFireFrame = 0;

  mPitchCooldown.SetNewTimeout( plane::pitchCooldown );
  mShootCooldown.SetNewTimeout( plane::shootCooldown );

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

void
Plane::HitboxUpdate()
{
  namespace plane = constants::plane;


  if ( mIsDead == true )
    return;

  mHitbox =
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
  mDir = mType == PLANE_TYPE::RED ? 270.0f : 90.0f;
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

  if ( gameState().isRoundFinished == false )
  {
    attacker.mStats.plane_hits++;
    log_message("Plane::Hit() attacker.mStats.plane_hits++\n");
  }


  if ( gameState().isHardcoreEnabled == true )
    mHp = 0;

  if ( mHp > 0 )
  {
    playSound(sounds.hit, -1, false);
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


  attacker.ScoreChange(1);

  if ( gameState().isRoundFinished == false )
  {
    mStats.deaths++;
    attacker.mStats.plane_kills++;
    log_message("Plane::Hit() mStats.deaths++\n");
    log_message("Plane::Hit() attacker.mStats.plane_kills++\n");
  }
}

// DIE
void
Plane::Explode()
{
  playSound(sounds.expl, -1, false);

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
  {
    mStats.crashes++;
    log_message("Plane::Crash() mStats.crashes++\n");
  }
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

  mPrevX = mX;
  mPrevY = mY;

  mHasJumped = false;

  pilot.Respawn();

  HitboxUpdate();

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
        playSound(sounds.loss, -1, false);
        menu.setMessage(MESSAGE_TYPE::GAME_LOST);
      }
      else
      {
        playSound(sounds.victory, -1, false);

        if ( mType == PLANE_TYPE::BLUE )
          menu.setMessage(MESSAGE_TYPE::BLUE_SIDE_WON);
        else
          menu.setMessage(MESSAGE_TYPE::RED_SIDE_WON);

//      TODO: finish bot match
      }
    }
    else
    {
      playSound(sounds.victory, -1, false);
      menu.setMessage(MESSAGE_TYPE::GAME_WON);
    }
  }
  else
  {
    playSound(sounds.loss, -1, false);
    menu.setMessage(MESSAGE_TYPE::GAME_LOST);
  }


  mStats.wins++;
  opponentPlane.mStats.losses++;
  log_message("Plane::ScoreChange() mStats.wins++\n");
  log_message("Plane::ScoreChange() opponentPlane.mStats.losses++\n");

  game.isRoundFinished = true;

  updateRecentStats();

  if ( game.gameMode != GAME_MODE::BOT_VS_BOT )
    updateTotalStats();
}

void
Plane::ResetStats()
{
  mStats = {};
  log_message("Plane::ResetStats()\n");
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


  const SDL_FPoint hitPoint {x, y};

  return SDL_PointInFRect(&hitPoint, &mHitbox);
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

// Get coordinates for sending
PlaneData
Plane::getData() const
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
  const PlaneData& data )
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

SDL_Point
Plane::getClosestCollision() const
{
  namespace barn = constants::barn;
  namespace plane = constants::plane;


  const SDL_Point dotBarn
  {
    std::max( barn::planeCollisionX,
              std::min( mX, barn::planeCollisionX + barn::sizeX ) ), // THINK: or barn::bulletCollisionSizeX ?
    std::max( barn::planeCollisionY,
              std::min( mY, barn::planeCollisionY + barn::sizeY ) ),
  };

  const SDL_Point dot =
  {
    mX,
    plane::groundCollision,
  };

  float distanceToBarn    = sqrt( pow( dotBarn.x - mX, 2 ) + pow( dotBarn.y - mY, 2 ) );
  float distanceToGround  = sqrt( pow( dot.x - mX, 2 )     + pow( dot.y - mY, 2 ) );

  if ( distanceToBarn < distanceToGround )
    return dotBarn;

  return dot;
}

float
Plane::getDistanceToPoint(
  const SDL_Point& dot ) const
{
  if ( mHasJumped == false )
    return sqrt( pow( dot.x - mX, 2 ) + pow( dot.y - mY, 2 ) );

  return sqrt( pow( dot.x - pilot.mX, 2 ) + pow( dot.y - pilot.mY, 2 ) );
}

float
Plane::getSpeedDir() const
{
  if ( mHasJumped == false )
  {
    float speedY = 0.0f;

    if ( mIsOnGround == false )
      speedY = mSpeed * cos( mDir * M_PI / 180.0 ) + mMaxSpeedVar - mSpeed;

    const float speedX = mSpeed * sin( mDir * M_PI / 180.0 );
    float speedDir = atan2( speedY, speedX ) + M_PI / 2.0;

    speedDir *= 180.0f / M_PI;

    if ( speedDir < 0.0f )
      speedDir += 360.0f;

    if ( mIsOnGround == true && mIsTakingOff == false )
      return 0.0f;

    return speedDir;
  }

  if ( pilot.mIsRunning == true )
    return pilot.mDir;

  const float pilotSpeedX = pilot.mSpeed * sin( pilot.mDir * M_PI / 180.0 );
  float pilotDir = atan2( -pilot.mVSpeed, pilotSpeedX ) + M_PI / 2.0;

  pilotDir *= 180.0f / M_PI;

  if ( pilotDir < 0.0f )
    pilotDir += 360.0f;

  return pilotDir;
}

float
Plane::getSpeed() const
{
  if ( mHasJumped == false )
    return mSpeed / constants::plane::maxSpeedBoosted;

  const float pilotSpeedX = pilot.mSpeed * sin( pilot.mDir * M_PI / 180.0f );

  return sqrt( pow( pilotSpeedX, 2) + pow( -pilot.mVSpeed, 2 ) );
}

float
getAngleRelative(
  const float angleSource,
  const float angleTarget )
{
  float relativeAngle = angleTarget - angleSource;

  if ( relativeAngle > 180.0f )
    relativeAngle -= 360.0f;
  else if ( relativeAngle < -180.0f )
    relativeAngle += 360.0f;

  return relativeAngle;
}

std::vector <float>
Plane::aiState() const
{
  namespace plane = constants::plane;


  std::vector <float> inputs {};

  const auto& opponentPlane =
    planes.at(static_cast <PLANE_TYPE> (!mType));


  const auto getPlaneState =
  [&inputs] ( const Plane& plane )
  {
    const auto damage =
      (plane.isDead() == false && plane.mHasJumped == false) *
      float(plane::maxHp - plane.mHp) / plane::maxHp;

    const float shootCooldown =
      (plane.mIsOnGround == false && plane.mHasJumped == false) *
      (plane::shootCooldown - plane.mShootCooldown.remainderTime()) /
      plane::shootCooldown;

    const float protectionCooldown =
      (plane.mHasJumped == false) *
      plane.mProtection.remainderTime() / plane::spawnProtectionCooldown;

    inputs.push_back(plane.canAccelerate());
    inputs.push_back(plane.canDecelerate());
    inputs.push_back(plane.canTurn());
    inputs.push_back(plane.canShoot());
    inputs.push_back(plane.canJump());

    inputs.push_back(plane.isDead());

    if ( plane.isDead() == true )
    {
      inputs.resize(inputs.size() + 15);
      return;
    }

//    OnGround
    inputs.push_back(
      plane.mIsOnGround == true &&
      plane.mIsTakingOff == false &&
      plane.mHasJumped == false );

//    TakingOff
    inputs.push_back(
      plane.mIsTakingOff == true &&
      plane.mHasJumped == false );

//    Airborne
    inputs.push_back(
      plane.mIsOnGround == false &&
      plane.mIsTakingOff == false &&
      plane.mHasJumped == false );

//    Ejected
    inputs.push_back(
      plane.mHasJumped == true &&
      plane.pilot.mIsChuteOpen == false &&
      plane.pilot.mChuteState != CHUTE_DESTROYED &&
      plane.pilot.mIsRunning == false );

//    ChuteOpen
    inputs.push_back(
      plane.mHasJumped == true &&
      plane.pilot.mIsChuteOpen == true &&
      plane.pilot.mChuteState != CHUTE_DESTROYED &&
      plane.pilot.mIsRunning == false );

//    ChuteDestroyed
    inputs.push_back(
      plane.mHasJumped == true &&
      plane.pilot.mIsRunning == false &&
      plane.pilot.mChuteState == CHUTE_DESTROYED );

//    Running
    inputs.push_back(
      plane.pilot.mIsRunning == true );


    inputs.push_back(damage);

    inputs.push_back(shootCooldown);
    inputs.push_back(protectionCooldown);

    if ( plane.mHasJumped == false )
    {
      inputs.push_back(plane.mX);
      inputs.push_back(plane.mY); // TODO: clamp to ground collision

      inputs.push_back(plane.mPrevX);
      inputs.push_back(plane.mPrevY);

      inputs.push_back(plane.mDir / 337.5f); // TODO: or 360 ?
    }
    else
    {
      inputs.push_back(plane.pilot.mX);
      inputs.push_back((0.5f + plane.pilot.mY) / 1.5f); // TODO: clamp to ground collision

      inputs.push_back(plane.pilot.mPrevX);
      inputs.push_back((1.0f + plane.pilot.mPrevY) / 2.0f);

      inputs.push_back(0.0f);
    }
  };

  getPlaneState(*this);
  getPlaneState(opponentPlane);

//  TODO: handle pilot death cooldown
  const float deadCooldown =
    (opponentPlane.isDead() == true) *
    opponentPlane.mDeadCooldown.remainderTime() /
    plane::deadCooldown;

  inputs.push_back(deadCooldown);

  const size_t maxBullets {3};
  const auto targetSize = inputs.size() + maxBullets * 3;

  size_t serializedBullets {};

  for ( const auto& bullet : bullets.GetClosestBullets(mX, mY, mType) )
  {
    if ( ++serializedBullets > maxBullets )
      break;

    inputs.push_back(bullet.x());
    inputs.push_back(bullet.y());
    inputs.push_back(bullet.dir() / 337.5f); // TODO: or 360 ?
  }

  inputs.resize(targetSize);

  return inputs;
}
