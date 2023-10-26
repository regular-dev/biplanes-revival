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
#include <include/game_state.hpp>
#include <include/sdl.hpp>
#include <include/cloud.hpp>
#include <include/bullet.hpp>
#include <include/menu.hpp>
#include <include/network.hpp>
#include <include/network_data.hpp>
#include <include/effects.hpp>
#include <include/sizes.hpp>
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

  SpeedUpdate();
  CoordinatesUpdate();
  CollisionsUpdate();
  AbandonedUpdate();

  if ( mHasJumped == true )
    pilot.Update();

  HitboxUpdate();

  mPitchCooldown.Update();
  mFireCooldown.Update();

  mDeadCooldown.Update();
  mProtection.Update();

  AnimationsUpdate();
}

void
Plane::Draw() const
{
  if ( mIsDead == true )
    return;


  const SDL_Rect planeRect
  {
    mX - sizes.plane_sizex / 2.0f,
    mY - sizes.plane_sizey / 2.0f,
    sizes.plane_sizex,
    sizes.plane_sizey,
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

  SDL_RenderCopyEx(
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
    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &mHitbox );

    const SDL_Rect planeCenter
    {
      mX - sizes.plane_sizex * 0.05f,
      mY - sizes.plane_sizey * 0.05f,
      sizes.plane_sizex * 0.1f,
      sizes.plane_sizey * 0.1f,
    };

    SDL_RenderDrawRect( gRenderer, &planeCenter );
  }


  if ( mHasJumped == false && gameState().debug.aiInputs == true )
  {
    SDL_Point dot = getClosestCollision();
    SDL_SetRenderDrawColor( gRenderer, 255, 255, 0, 1 );

    if ( mHasJumped == false )
      SDL_RenderDrawLine(
        gRenderer,
        mX, mY,
        dot.x, dot.y );
  }
}

void
Plane::DrawFire() const
{
  if ( mHp > 0 )
    return;


  const SDL_Rect textureRect
  {
    mX - sizes.smk_sizex / 2.0f,
    mY - sizes.smk_sizey / 2.0f,
    sizes.smk_sizex,
    sizes.smk_sizey,
  };

  const double textureAngle =
    mType == PLANE_TYPE::RED
    ? mDir + 90.0
    : mDir - 90.0;

  const auto textureFlip =
    mType == PLANE_TYPE::BLUE
    ? SDL_FLIP_NONE
    : SDL_FLIP_HORIZONTAL;

  SDL_RenderCopyEx(
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
  if ( mIsDead == true || mHasJumped == true )
    return;


  if ( mIsOnGround == true )
  {
    if ( mIsTakingOff == false )
      TakeOffStart();

    mSpeed += sizes.plane_incr_speed * 0.85f * deltaTime;

    return;
  }


  if ( mDir != 0.0f )
  {
    if ( mDir == 22.5f || mDir == 337.5f )
      mSpeed += sizes.plane_incr_speed * 0.25f * deltaTime;

    else if ( mDir == 45.0f || mDir == 315.0f )
      mSpeed += sizes.plane_incr_speed * 0.5f * deltaTime;

    else
      mSpeed += sizes.plane_incr_speed * 0.75f * deltaTime;

    if ( mSpeed > mMaxSpeedVar )
      mSpeed = mMaxSpeedVar;
  }
}

void
Plane::Decelerate()
{
  if ( mIsDead == true || mHasJumped == true )
    return;


  if ( mIsTakingOff == true )
  {
    mSpeed -= sizes.plane_incr_speed * 0.75f * deltaTime;
    return;
  }


  mSpeed -= sizes.plane_incr_speed * 0.5f * deltaTime;


  if ( mSpeed < 0.0f )
    mSpeed = 0.0f;

  if ( mMaxSpeedVar <= sizes.plane_max_speed_def )
    return;


  mMaxSpeedVar = mSpeed;

  if ( mMaxSpeedVar < sizes.plane_max_speed_def )
    mMaxSpeedVar = sizes.plane_max_speed_def;
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

  mDir += dir * sizes.plane_incr_rot;

  if ( mDir < 0.0f )
    mDir += 360.0f;

  else if ( mDir >= 360.0f )
    mDir -= 360.0f;
}

void
Plane::Shoot()
{
//  THINK: is spawn protection condition needed ?
//  THINK: is fire cooldown condition needed ?
  if (  mHasJumped == true ||
        mIsDead == true ||
        mIsOnGround == true ||
        mProtection.isReady() == false )
    return;

  if (  mIsLocal == true &&
        mFireCooldown.isReady() == false )
    return;


  mFireCooldown.Start();
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
  if ( mIsOnGround == true )
    return TakeOffUpdate();


//  Decrease vertical speed
  if ( mDir <= 70 || mDir >= 290 )
  {
    if ( mDir == 0 )                          // 90 climb
    {
      mSpeed -= sizes.plane_incr_speed * 0.225f * deltaTime;
      mMaxSpeedVar = mSpeed;
    }
    else if ( mDir <= 25 || mDir >= 330 )     // 75 climb
    {
      mSpeed -= sizes.plane_incr_speed * 0.100f * deltaTime;
      mMaxSpeedVar = mSpeed;
    }
    else if ( mDir <= 50 || mDir >= 310 )     // 45 climb
    {
      mSpeed -= sizes.plane_incr_speed * 0.065f * deltaTime;
      mMaxSpeedVar = mSpeed;
    }
    else                                      // 30 climb
    {
      mSpeed -= sizes.plane_incr_speed * 0.020f * deltaTime;
      mMaxSpeedVar = mSpeed;
    }

    if ( mMaxSpeedVar < sizes.plane_max_speed_def )
      mMaxSpeedVar = sizes.plane_max_speed_def;

    if ( mSpeed < 0.0f )
      mSpeed = 0.0f;

    return;
  }

//  Increase vertical speed
  if ( mDir > 113 && mDir < 246 )
  {
    if ( mSpeed < sizes.plane_max_speed_acc )
    {
      mSpeed += sizes.plane_incr_speed * 0.2 * deltaTime;

      if ( mSpeed > mMaxSpeedVar )
      {
        mMaxSpeedVar = mSpeed;

        if ( mMaxSpeedVar > sizes.plane_max_speed_acc )
        {
          mMaxSpeedVar = sizes.plane_max_speed_acc;
          mSpeed = mMaxSpeedVar;
        }
      }

      return;
    }

    mSpeed = sizes.plane_max_speed_acc;
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

//  Screen borders teleport
  if ( mX > sizes.screen_width )
    mX = 0.0f;
  else if ( mX < 0.0f )
    mX = sizes.screen_width;


  if ( mY < 0.0f )
    mY = 0.0f;
}

void
Plane::CollisionsUpdate()
{
  if (  mIsLocal == false ||
        mIsDead == true ||
        (mIsOnGround == true && mIsTakingOff == false) )
    return;


  const bool collidesWithBarn
  {
    mY > sizes.barn_y_collision &&
    mX > sizes.barn_x_collision &&
    mX < sizes.barn_x_collision + sizes.barn_sizex
  };

  const bool collidesWithGround
  {
    mIsOnGround == false &&
    mY > sizes.ground_y_collision
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
  if (  mIsDead == true ||
        mHasJumped == false ||
        mDir == 180.0f )
    return;


  if ( mSpeed > sizes.plane_max_speed_def / 2.0f )
    mSpeed -= sizes.plane_incr_speed * 0.2 * deltaTime;


  if ( mPitchCooldown.isReady() == false )
    return;


  mPitchCooldown.Start();

  if ( mType == PLANE_TYPE::RED )
    mDir += mDir < 180.0f ? sizes.plane_incr_rot : -sizes.plane_incr_rot;
  else
    mDir += mDir < 180.0f ? sizes.plane_incr_rot : -sizes.plane_incr_rot;
}

void
Plane::AnimationsReset()
{
  mSmokeFrame = 0;
  mSmokeAnim.Stop();
  mSmokePeriod.Stop();

  mFireAnim.Stop();
  mFireFrame = 0;

  mFireCooldown.SetNewTimeout( sizes.plane_fire_cooldown_time );
  mPitchCooldown.SetNewTimeout( sizes.plane_pitch_cooldown_time );

  mDeadCooldown.SetNewTimeout( sizes.plane_dead_cooldown_time );
  mProtection.SetNewTimeout( sizes.plane_spawn_protection_time );

  mSmokeAnim.SetNewTimeout( sizes.smk_frame_time );
  mSmokePeriod.SetNewTimeout( sizes.smk_anim_period );
  mFireAnim.SetNewTimeout( sizes.fire_frame_time );

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
  mSmokePeriod.Update();

  if ( mSmokePeriod.isReady() == true )
  {
    mSmokePeriod.Start();
    mSmokeAnim.Stop();
    mSmokeFrame = 0;
  }


  if ( mSmokeAnim.isReady() == false )
    return;


  if ( mSmokeFrame < sizes.smk_frame_count )
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
  if ( mIsDead == true )
    return;

  mHitbox =
  {
    mX - sizes.plane_sizex / 3.0f,
    mY - sizes.plane_sizey / 3.0f,
    sizes.plane_sizex / 3.0f * 2.0f,
    sizes.plane_sizey / 3.0f * 2.0f,
  };
}

void
Plane::TakeOffUpdate()
{
  if ( mIsTakingOff == false )
    return;

  mSpeed += sizes.plane_incr_speed * 0.75f * deltaTime;

  if ( mSpeed >= sizes.plane_max_speed_def )
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
//      TODO: start puffing
      mSmokeAnim.Start();
      mSmokePeriod.Start();
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
  mFireCooldown.Stop();
  mPitchCooldown.Stop();
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
  mIsDead = false;
  mHp = sizes.plane_hp_max;
  mIsOnGround = true;
  mIsTakingOff = false;
  mSpeed = 0.0f;
  mMaxSpeedVar = sizes.plane_max_speed_def;

  mDeadCooldown.Stop();
  mFireCooldown.Stop();
  mPitchCooldown.Stop();
  mProtection.Stop();

  mX = mType == PLANE_TYPE::RED
    ? sizes.plane_red_landx
    : sizes.plane_blue_landx;

  mY = sizes.plane_landy;

  mDir = mType == PLANE_TYPE::RED
    ? 292.5f : 67.5f;

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


  const SDL_Point hitPoint {x, y};

  return SDL_PointInRect(&hitPoint, &mHitbox);
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
    mFireCooldown.isReady() == true;
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
  const SDL_Point dotBarn
  {
    std::max( (float) sizes.barn_x_collision,
              std::min( mX, sizes.barn_x_collision + sizes.barn_sizex * 0.95f ) ),
    std::max( (float) sizes.barn_y_collision,
              std::min( mY, sizes.barn_y_collision + (float) sizes.barn_sizey ) ),
  };

  const SDL_Point dot =
  {
    mX,
    sizes.ground_y_collision,
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
Plane::getAngleToPoint(
  const SDL_Point& source,
  const SDL_Point& target ) const
{
  float angle = atan2( target.y - source.y, target.x - source.x ) + M_PI / 2;

  angle *= 180.0 / M_PI;

  if ( angle < 0.0f )
    angle += 360.0f;

  return angle;
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
    return mSpeed / sizes.plane_max_speed_def;

  const float pilotSpeedX = pilot.mSpeed * sin( pilot.mDir * M_PI / 180.0f );

  return sqrt( pow( pilotSpeedX, 2) + pow( -pilot.mVSpeed, 2 ) );
}

float
Plane::getAngleRelative(
  const float angleSource,
  const float angleTarget ) const
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
  std::vector <float> inputs {};

  const auto& opponentPlane =
    planes.at(static_cast <PLANE_TYPE> (!mType));


  const bool canAccelerate =
    isDead() == false &&
    mHasJumped == false &&
    mDir != 0.0f;

  const bool canDecelerate =
    isDead() == false &&
    mHasJumped == false &&
    (mIsOnGround == false || mIsTakingOff == true);


  const auto damage =
    float(sizes.plane_hp_max - mHp) / sizes.plane_hp_max;

  const float fireCooldown =
    (mIsOnGround == false && mHasJumped == false) *
    (sizes.plane_fire_cooldown_time - mFireCooldown.remainderTime()) /
    sizes.plane_fire_cooldown_time;

  const float protectionCooldown =
    (mHasJumped == false) *
    mProtection.remainderTime() / sizes.plane_spawn_protection_time;

  const float deadCooldown =
    (sizes.plane_dead_cooldown_time - opponentPlane.mDeadCooldown.remainderTime()) /
    sizes.plane_dead_cooldown_time;

  inputs.push_back(canAccelerate);
  inputs.push_back(canDecelerate);
  inputs.push_back(canTurn());
  inputs.push_back(canShoot());
  inputs.push_back(canJump());

  inputs.push_back(isDead());

  if ( isDead() == true )
    inputs.resize(inputs.size() + 7);
  else
  {
//    OnGround
    inputs.push_back(
      mIsOnGround == true &&
      mIsTakingOff == false &&
      mHasJumped == false );

//    TakingOff
    inputs.push_back(
      mIsTakingOff == true &&
      mHasJumped == false );

//    Airborne
    inputs.push_back(
      mIsOnGround == false &&
      mIsTakingOff == false &&
      mHasJumped == false );

//    Ejected
    inputs.push_back(
      mHasJumped == true &&
      pilot.mIsChuteOpen == false &&
      pilot.mChuteState != CHUTE_DESTROYED &&
      pilot.mIsRunning == false );

//    ChuteOpen
    inputs.push_back(
      mHasJumped == true &&
      pilot.mIsChuteOpen == true &&
      pilot.mChuteState != CHUTE_DESTROYED &&
      pilot.mIsRunning == false );

//    ChuteDestroyed
    inputs.push_back(
      mHasJumped == true &&
      pilot.mIsRunning == false &&
      pilot.mChuteState == CHUTE_DESTROYED );

//    Running
    inputs.push_back(
      pilot.mIsRunning == true );
  }

  inputs.push_back(damage);

  inputs.push_back(fireCooldown);
  inputs.push_back(protectionCooldown);
  inputs.push_back(deadCooldown); // put only opponent's

  inputs.push_back( getSpeed() );
  inputs.push_back( getSpeedDir() );


//  TODO: dirToOpponentDirect
//  TODO: dirToOpponentIndirect

  const auto opponent =
    opponentPlane.mHasJumped == false
    ? SDL_Point { opponentPlane.x(), opponentPlane.y() }
    : SDL_Point { opponentPlane.pilot.x(), opponentPlane.pilot.y() };


  if ( mHasJumped == false )
  {
    const float opponentDirToAI = getAngleToPoint( { mX, mY }, opponent );
    inputs.push_back( getAngleRelative( mDir, opponentDirToAI ) );
  }
  else
  {
//    TODO: PILOT DIR
    inputs.push_back( getSpeedDir() );
  }

  SDL_Point closestCollision = getClosestCollision();


//  Data about AI plane

  inputs.push_back( getDistanceToPoint( closestCollision ) );
  inputs.push_back( getAngleToPoint( { mX, mY }, closestCollision ) );  // todo: make relative to speedDir ???

  inputs.push_back( getDistanceToPoint( opponent ) );

  if ( mHasJumped == false )
  {
    const float dirToOpponent = getAngleToPoint( { mX, mY }, opponent );
    inputs.push_back( getAngleRelative( mDir, dirToOpponent ) );
  }
  else
  {
    const float dirToOpponent = getAngleToPoint( { pilot.mX, pilot.mY }, opponent );
    inputs.push_back( getAngleRelative( getSpeedDir(), dirToOpponent ) );
  }


//  Data about closest bullet

  Bullet bullet = bullets.GetClosestBullet(mX, mY, mType);

  const bool bulletPresent =
    bullet.firedBy() == mType
    ? false : true;

  inputs.push_back( bulletPresent );

  if ( bulletPresent == true )
  {
    inputs.push_back( getDistanceToPoint( { bullet.x(), bullet.y() } ) );

    const float aiDirToBullet = getAngleToPoint( { mX, mY }, { bullet.x(), bullet.y() } );
    inputs.push_back( getAngleRelative( mDir, aiDirToBullet ) );

    const float bulletDirToAI = getAngleToPoint( { bullet.x(), bullet.y() }, { mX, mY } );
    inputs.push_back( getAngleRelative( bullet.dir(), bulletDirToAI ) );
  }
  else
    inputs.resize(inputs.size() + 3);


  return inputs;
}
