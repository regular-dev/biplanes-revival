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
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/network.hpp>
#include <include/plane.hpp>
#include <include/effects.hpp>
#include <include/sounds.hpp>
#include <include/textures.hpp>

#include <algorithm>


Bullet::Bullet(
  const float planeX,
  const float planeY,
  const float planeDir,
  const PLANE_TYPE firedBy )
  : mX{planeX}
  , mY{planeY}
  , mDir{planeDir}
  , mFiredBy{firedBy}
{
}

void
Bullet::Update()
{
  namespace barn = constants::barn;
  namespace bullet = constants::bullet;


  if ( mIsDead == true )
    return;


  mX += bullet::speed * sin( mDir * M_PI / 180.0f ) * deltaTime;
  mY -= bullet::speed * cos( mDir * M_PI / 180.0f ) * deltaTime;


  const bool collidesWithScreenBorder
  {
    mX > 1.0f ||
    mX < 0.0f ||
    mY < 0.0f
  };

  if ( collidesWithScreenBorder == true )
  {
    mX = std::clamp(mX, 0.0f, 1.0f);

    mY = std::max(mY, 0.0f);

    return Destroy();
  }


  const bool collidesWithSurface
  {
    ( mX > barn::bulletCollisionX &&
      mX < barn::bulletCollisionX + barn::bulletCollisionSizeX &&
      mY > barn::bulletCollisionY ) ||
      mY > bullet::groundCollision
  };

  if ( collidesWithSurface == true )
  {
    panSound(
      playSound(sounds.hitMiss, -1, false),
      mX );

    effects.Spawn(new BulletImpact{mX, mY});

    return Destroy();
  }

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
Bullet::Draw() const
{
  namespace bullet = constants::bullet;


  if ( mIsDead == true )
    return;


  const SDL_FRect bulletRect
  {
    toWindowSpaceX(mX - 0.5f * bullet::sizeX),
    toWindowSpaceY(mY - 0.5f * bullet::sizeY),
    scaleToScreenX(bullet::sizeX),
    scaleToScreenY(bullet::sizeY),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.texture_bullet,
    nullptr,
    &bulletRect );
}

void
Bullet::Destroy()
{
  mIsDead = true;
}

bool
Bullet::isDead() const
{
  return mIsDead;
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
  const float x,
  const float y,
  const float dir,
  const PLANE_TYPE firedBy )
{
  mInstances.push_back(
  {
    x, y,
    dir,
    firedBy,
  });
}

void
BulletSpawner::Update()
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
BulletSpawner::Draw() const
{
  for ( auto& bullet : mInstances )
    bullet.Draw();


  if ( gameState().debug.aiInputs == true )
  {
    for ( const auto& [planeType, plane] : planes )
    {
      setRenderColor(constants::colors::bulletHitbox);

      const auto closestBullets = GetClosestBullets(
        plane.x(),
        plane.y(),
        plane.type() );

      for ( const auto& bullet : closestBullets )
      {
        SDL_RenderDrawLine(
          gRenderer,
          toWindowSpaceX(plane.x()),
          toWindowSpaceY(plane.y()),
          toWindowSpaceX(bullet.x()),
          toWindowSpaceY(bullet.y()) );
      }
    }
  }
}

std::vector <Bullet>
BulletSpawner::GetClosestBullets(
  const float x,
  const float y,
  const PLANE_TYPE target ) const
{
  std::vector <Bullet> result {};

  for ( const auto& bullet : mInstances )
  {
    if ( bullet.firedBy() != target )
      result.push_back(bullet);
  }

  std::sort(result.begin(), result.end(),
  [x, y] ( const Bullet& lhs, const Bullet& rhs )
  {
    const float distanceToLhs = sqrt(
      pow( lhs.x() - x, 2 ) +
      pow( lhs.y() - y, 2 ) );

    const float distanceToRhs = sqrt(
      pow( rhs.x() - x, 2 ) +
      pow( rhs.y() - y, 2 ) );

    return distanceToLhs < distanceToRhs;
  });

  return result;
}
