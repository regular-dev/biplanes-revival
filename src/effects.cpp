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

#include <include/effects.hpp>
#include <include/sdl.hpp>
#include <include/time.hpp>
#include <include/constants.hpp>
#include <include/textures.hpp>


Effect::Effect(
  const float x,
  const float y,
  const double frameTime,
  const uint8_t frameCount )
  : mX {x}
  , mY {y}
  , mAnim {frameTime}
  , mFrameCount {frameCount}
{
  mAnim.Start();
}

void
Effect::Update()
{
  if ( hasFinished() == true )
    return;


  mAnim.Update();

  if ( mAnim.isReady() == true )
  {
    mAnim.Start();
    ++mFrame;
  }
}

void
Effect::Draw() const
{
  if ( hasFinished() == false )
    DrawImpl();
}

bool
Effect::hasFinished() const
{
  return mFrame >= mFrameCount;
}


Effects::~Effects()
{
  Clear();
}

void
Effects::Spawn(
  Effect* effect )
{
  mEffects.push_back(effect);
}

void
Effects::Clear()
{
  for ( const auto effect : mEffects )
    delete effect;

  mEffects.clear();
}

void
Effects::Update()
{
  size_t i {};

  while ( i < mEffects.size() )
  {
    const auto effect = mEffects[i];

    effect->Update();

    if ( effect->hasFinished() == true )
    {
      delete effect;
      mEffects.erase(mEffects.begin() + i);

      continue;
    }

    ++i;
  }
}

void
Effects::Draw() const
{
  for ( const auto effect : mEffects )
    effect->Draw();
}


SmokePuff::SmokePuff(
  const float x,
  const float y )
  : Effect {x, y, constants::smoke::frameTime, constants::smoke::frameCount}
{
}

void
SmokePuff::DrawImpl() const
{
  namespace smoke = constants::smoke;


  const SDL_FRect smokeRect
  {
    toWindowSpaceX(mX - 0.5f * smoke::sizeX),
    toWindowSpaceY(mY - 0.5f * smoke::sizeY),
    scaleToScreenX(smoke::sizeX),
    scaleToScreenY(smoke::sizeY),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.anim_smk,
    &textures.anim_smk_rect[mFrame],
    &smokeRect );
}


Explosion::Explosion(
  const float x,
  const float y )
  : Effect {x, y, 0.075, 7}
{
}

void
Explosion::DrawImpl() const
{
  namespace explosion = constants::explosion;


  const SDL_FRect explosionRect
  {
    toWindowSpaceX(mX - 0.5f * explosion::sizeX),
    toWindowSpaceY(mY - 0.5f * explosion::sizeY),
    scaleToScreenX(explosion::sizeX),
    scaleToScreenY(explosion::sizeY),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.anim_expl,
    &textures.anim_expl_rect[mFrame],
    &explosionRect );
}


ExplosionSpark::ExplosionSpark(
  const float x,
  const float y,
  const float speed,
  const float dir )
  : Effect {x, y, 0.05, 5}
{
  mSpeedX = std::sin(dir) * speed;
  mSpeedY = std::cos(dir) * -speed;
}

void
ExplosionSpark::Update()
{
  namespace barn = constants::barn;
  namespace spark = constants::explosion::spark;

  if ( hasFinished() == true )
    return;


  mAnim.Update();

  if ( mAnim.isReady() == true )
  {
    mAnim.Start();
    ++mFrame;

    if ( mFrame >= mFrameCount )
      mFrame = 0;
  }


  mSpeedX -= mSpeedX * deltaTime;
  mSpeedY += spark::gravity * deltaTime;

  mX += mSpeedX * deltaTime;
  mY += mSpeedY * deltaTime;

  if ( mX > 1.0f )
    mX -= 1.0f;

  if ( mX < 0.0f )
    mX += 1.0f;


  if ( mSpeedY < 0.0f )
    return;


  if ( mBounces >= spark::maxBounces )
    mFrame = mFrameCount;


  if ( mY < barn::bulletCollisionY )
    return;

  if (  mY < spark::groundCollision &&
        ( mX < barn::bulletCollisionX ||
          mX > barn::bulletCollisionX + barn::bulletCollisionSizeX ) )
    return;


  ++mBounces;
  mSpeedY = -spark::speedBounce;
}

void
ExplosionSpark::DrawImpl() const
{
  namespace spark = constants::explosion::spark;
  namespace colors = constants::colors;


  const SDL_FRect sparkRect
  {
    toWindowSpaceX(mX - 0.5f * spark::sizeX),
    toWindowSpaceY(mY - 0.5f * spark::sizeY),
    scaleToScreenX(spark::sizeX),
    scaleToScreenY(spark::sizeY),
  };

  setRenderColor(colors::explosionSpark[mFrame]);
  SDL_RenderFillRectF(
    gRenderer,
    &sparkRect );
}


BulletImpact::BulletImpact(
  const float x,
  const float y )
  : Effect {x, y, 0.08, 6}
{
}

void
BulletImpact::DrawImpl() const
{
  namespace hit = constants::bullet::hit;


  const SDL_FRect impactRect
  {
    toWindowSpaceX(mX - 0.5f * hit::sizeX),
    toWindowSpaceY(mY - 0.5f * hit::sizeY),
    scaleToScreenX(hit::sizeX),
    scaleToScreenY(hit::sizeY),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.anim_hit,
    &textures.anim_hit_rect[mFrame],
    &impactRect );
}
