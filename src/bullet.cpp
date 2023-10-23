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

#include <include/bullet.hpp>
#include <include/sdl.hpp>
#include <include/time.hpp>
#include <include/game_state.hpp>
#include <include/network.hpp>
#include <include/plane.hpp>
#include <include/sizes.hpp>
#include <include/sounds.hpp>
#include <include/textures.hpp>


Bullet::Bullet(
  const float planeX,
  const float planeY,
  const float planeDir,
  const PLANE_TYPE planeType )
  : mX{planeX}
  , mY{planeY}
  , mDir{planeDir}
  , mFiredBy{planeType}
  , mHitAnim{sizes.bullet_hit_frame_time}
{
}

void
Bullet::Update()
{
  UpdateCoordinates();
  HitAnimUpdate();
  mHitAnim.Update();
}

void
Bullet::UpdateCoordinates()
{
  if ( mIsMoving == false )
    return;


  mX += sizes.bullet_speed * sin( mDir * M_PI / 180.0f ) * deltaTime;
  mY -= sizes.bullet_speed * cos( mDir * M_PI / 180.0f ) * deltaTime;


  const bool collidesWithScreenBorder
  {
    mX > sizes.screen_width ||
    mX < 0.0f ||
    mY < 0.0f - sizes.bullet_sizey / 2.0f
  };

  if ( collidesWithScreenBorder == true )
    return Destroy();


  const bool collidesWithSurface
  {
    ( mX > sizes.barn_x_bullet_collision &&
      mX < sizes.barn_x_bullet_collision + sizes.barn_sizex * 0.95f &&
      mY > sizes.barn_y_bullet_collision ) ||
      mY > sizes.bullet_ground_collision
  };

  if ( collidesWithSurface == true )
    return HitSurface();

  const auto& game = gameState();

  if (  game.gameMode == GAME_MODE::HUMAN_VS_HUMAN &&
        planes.at(mFiredBy).isLocal() == true )
    return;


  Plane* planeShooter = &planes.at(PLANE_TYPE::BLUE);
  Plane* planeTarget = &planes.at(PLANE_TYPE::RED);

  if ( mFiredBy == PLANE_TYPE::RED )
    std::swap(planeShooter, planeTarget);


//  HIT PLANE
  if ( planeTarget->isHit(mX, mY) == true )
  {
    Destroy();
    planeTarget->Hit(*planeShooter);

    return;
  }

//  HIT CHUTE
  if ( planeTarget->pilot.ChuteIsHit(mX, mY) == true )
  {
    Destroy();
    planeTarget->pilot.ChuteHit(*planeShooter);

    eventPush(EVENTS::HIT_CHUTE);
    return;
  }

//  HIT PILOT
  if ( planeTarget->pilot.isHit(mX, mY) == true )
  {
    Destroy();
    planeTarget->pilot.Kill(*planeShooter);

    eventPush(EVENTS::HIT_PILOT);
    return;
  }
}

void
Bullet::Draw()
{
  if ( mIsMoving == true )
  {
    const SDL_Rect bulletRect
    {
      mX - sizes.bullet_sizex / 2.0f,
      mY - sizes.bullet_sizey / 2.0f,
      sizes.bullet_sizex,
      sizes.bullet_sizey,
    };

    SDL_RenderCopy(
      gRenderer,
      textures.texture_bullet,
      nullptr,
      &bulletRect );

    return;
  }

  if ( mHitFrame <= 0 || mHitFrame >= 6 )
    return;

  const SDL_Rect textureRect
  {
    mX - sizes.hit_sizex / 2.0f,
    mY - sizes.hit_sizey / 2.0f,
    sizes.hit_sizex,
    sizes.hit_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.anim_hit,
    &textures.anim_hit_rect[mHitFrame - 1],
    &textureRect );
}

void
Bullet::HitAnimUpdate()
{
  if (  mIsMoving == true ||
        mHitFrame <= 0 ||
        mHitFrame >= 6 ||
        mHitAnim.isReady() == false )
    return;

  mHitAnim.Start();
  ++mHitFrame;
}

void
Bullet::HitSurface()
{
  playSound(sounds.hitMiss, -1, false);
  mIsMoving = false;
  mHitFrame = 1;
}

void
Bullet::Destroy()
{
  mIsMoving = false;
  mHitFrame = 6;
}


bool
Bullet::isMoving() const
{
  return mIsMoving;
}

bool
Bullet::isDead() const
{
  return mIsMoving == false && mHitFrame == 6;
}

float
Bullet::x() const
{
  return mX;
}

float
Bullet::y() const
{
  return mY;
}

float
Bullet::dir() const
{
  return mDir;
}

PLANE_TYPE
Bullet::firedBy() const
{
  return mFiredBy;
}


void
BulletSpawner::SpawnBullet(
  const float planeX,
  const float planeY,
  const float planeDir,
  const PLANE_TYPE planeType )
{
  mInstances.push_back(
  {
    planeX, planeY,
    planeDir,
    planeType,
  });
}

void
BulletSpawner::UpdateBullets()
{
  size_t i {};

  while ( i < mInstances.size() )
  {
    mInstances[i].Update();

    if ( mInstances[i].isDead() == true )
    {
      mInstances.erase(mInstances.begin() + i);
      continue;
    }

    ++i;
  }
}

void
BulletSpawner::Clear()
{
  mInstances.clear();
}

void
BulletSpawner::Draw()
{
  for ( auto& bullet : mInstances )
    bullet.Draw();


  if ( gameState().debug.aiInputs == true )
  {
    const auto& planeRed = planes.at(PLANE_TYPE::RED);
    const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);

    Bullet bullet = GetClosestBullet(
      planeRed.x(),
      planeRed.y(),
      planeRed.type() );

    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawLine(
      gRenderer,
      planeRed.x(),
      planeRed.y(),
      bullet.x(),
      bullet.y() );


    bullet = GetClosestBullet(
      planeBlue.x(),
      planeBlue.y(),
      planeBlue.type() );

    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawLine(
      gRenderer,
      planeBlue.x(),
      planeBlue.y(),
      bullet.x(),
      bullet.y() );
  }
}

Bullet
BulletSpawner::GetClosestBullet(
  const float planeX,
  const float planeY,
  const PLANE_TYPE planeType ) const
{
  std::pair <uint32_t, float> minDistance {};
  bool minDistanceInitialized = false;


  for ( size_t index = 0; index < mInstances.size(); ++index )
  {
    if (  mInstances[index].isMoving() == false ||
          mInstances[index].firedBy() == planeType )
      continue;

    const float distance = sqrt(
      pow( mInstances[index].x() - planeX, 2 ) +
      pow( mInstances[index].y() - planeY, 2 ) );

    if ( minDistanceInitialized == false )
    {
      minDistance = { index, distance };
      minDistanceInitialized = true;

      continue;
    }

    if ( distance < minDistance.second )
      minDistance = { index, distance };
  }

  if ( minDistanceInitialized == false )
    return {0.0f, 0.0f, 0.0f, planeType};

  return mInstances[minDistance.first];
}
