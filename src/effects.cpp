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
#include <include/sizes.hpp>
#include <include/textures.hpp>

#include <SDL_rect.h>


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
  : Effect {x, y, sizes.smk_frame_time, sizes.smk_frame_count}
{
}

void
SmokePuff::DrawImpl() const
{
  const SDL_Rect smokeRect
  {
    mX - sizes.smk_sizex / 2.0f,
    mY - sizes.smk_sizey / 2.0f,
    sizes.smk_sizex,
    sizes.smk_sizey,
  };

  SDL_RenderCopy(
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
  const SDL_Rect explosionRect
  {
    mX - sizes.expl_sizex / 2.0f,
    mY - sizes.expl_sizey / 2.0f,
    sizes.expl_sizex,
    sizes.expl_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.anim_expl,
    &textures.anim_expl_rect[mFrame],
    &explosionRect );
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
  const SDL_Rect impactRect
  {
    mX - sizes.bullet_hit_sizex / 2.0f,
    mY - sizes.bullet_hit_sizey / 2.0f,
    sizes.bullet_hit_sizex,
    sizes.bullet_hit_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.anim_hit,
    &textures.anim_hit_rect[mFrame],
    &impactRect );
}
