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

#include <include/cloud.hpp>
#include <include/sdl.hpp>
#include <include/time.hpp>
#include <include/game_state.hpp>
#include <include/sizes.hpp>
#include <include/textures.hpp>


Cloud::Cloud(
  const bool dir,
  const uint8_t id )
  : mDir{dir}
  , mId{id}
{
  Respawn();
}


void
Cloud::Update()
{
  UpdateCoordinates();
  UpdateCollisionBox();
}

void
Cloud::UpdateCoordinates()
{
  const float moveDir =
    mDir == true
    ? 1.0f
    : -1.0f;

  const float speedMin = sizes.cloud_speed * 0.5f;
  const float speedMax = sizes.cloud_speed * 1.5f;
  const float speedDif = speedMax - speedMin;

  mX += ( speedMin + speedDif * mId / clouds.size() )
        * moveDir * deltaTime;

  if ( mDir == true )
  {
    if ( mX - sizes.cloud_sizex / 2.0f > sizes.screen_width )
    {
      mX = -sizes.cloud_sizex / 2.0f;
      UpdateHeight();
    }

    return;
  }

  if ( mX + sizes.cloud_sizex / 2.0f < 0.0f )
  {
    mX = sizes.screen_width + sizes.cloud_sizex / 2.0f;
    UpdateHeight();
  }
}

void
Cloud::UpdateHeight()
{
  float cloudHeight = sizes.cloud_lowest_y - sizes.cloud_highest_y;

  if ( mDir == true )
  {
    mY += sizes.cloud_sizex / 2.0f;

    if ( mY > sizes.cloud_lowest_y )
      mY = sizes.cloud_highest_y + mId * cloudHeight / clouds.size();

    return;
  }

  mY -= sizes.cloud_sizex / 2.0f;

  if ( mY < sizes.cloud_highest_y )
    mY = sizes.cloud_lowest_y - mId * cloudHeight / clouds.size();
}

void
Cloud::UpdateCollisionBox()
{
  mCollisionBox =
  {
    mX - sizes.cloud_sizex / 2.0f,
    mY - sizes.cloud_sizey / 2.0f,
    sizes.cloud_sizex,
    sizes.cloud_sizey,
  };
}

bool
Cloud::isHit(
  const float x,
  const float y ) const
{
  return
    x > mCollisionBox.x &&
    x < mCollisionBox.x + mCollisionBox.w &&
    y > mCollisionBox.y &&
    y < mCollisionBox.y + mCollisionBox.h;
}

void
Cloud::Draw()
{
  const auto& game = gameState();

  if ( game.isHardcoreEnabled == false && mId >= 2 )
    return;


  auto* const cloudTexture =
    mIsOpaque == true
    ? textures.texture_cloud_opaque
    : textures.texture_cloud;

  const SDL_Rect cloudRect
  {
    mX - sizes.cloud_sizex / 2.0f,
    mY - sizes.cloud_sizey / 2.0f,
    sizes.cloud_sizex,
    sizes.cloud_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    cloudTexture,
    nullptr,
    &cloudRect );


  if ( game.debug.collisions == true )
  {
    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &mCollisionBox );
  }
}

void
Cloud::setTransparent()
{
  mIsOpaque = false;
}

void
Cloud::setOpaque()
{
  mIsOpaque = true;
}

void
Cloud::Respawn()
{
  if ( mDir == true )
    mX = sizes.cloud_right_spawn_x + (clouds.size() - mId) * sizes.cloud_sizey;
  else
    mX = sizes.cloud_left_spawn_x + (clouds.size() - mId) * sizes.cloud_sizey;


  if ( mId % 2 )
    mY = sizes.cloud_highest_y + (clouds.size() + mId) / (float) clouds.size() * sizes.cloud_sizey;
  else
    mY = sizes.cloud_lowest_y - (clouds.size() + mId) / (float) clouds.size() * sizes.cloud_sizey;


  mIsOpaque = true;
}
