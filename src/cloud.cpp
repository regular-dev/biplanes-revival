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
#include <include/constants.hpp>
#include <include/game_state.hpp>
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
  namespace cloud = constants::cloud;

  const float moveDir =
    mDir == true
    ? 1.0f
    : -1.0f;

  mX += ( cloud::minSpeed + mId * cloud::speedRange / clouds.size() )
        * moveDir * deltaTime;

  if ( mDir == true )
  {
    if ( mX - 0.5f * cloud::sizeX > 1.0f )
    {
      mX = -0.5f * cloud::sizeX;
      UpdateHeight();
    }

    return;
  }

  if ( mX + 0.5f * cloud::sizeX < 0.0f )
  {
    mX = 1.0f + 0.5f * cloud::sizeX;
    UpdateHeight();
  }
}

void
Cloud::UpdateHeight()
{
  namespace cloud = constants::cloud;


  const float dir =
    mDir == true
    ? 1.0f : -1.0f;


  mY += dir * 0.5f * cloud::sizeX;


  if ( mY > cloud::minHeight )
    mY = cloud::maxHeight + dir * mId * cloud::heightRange / clouds.size();

  if ( mY < cloud::maxHeight )
    mY = cloud::minHeight + dir * mId * cloud::heightRange / clouds.size();
}

void
Cloud::UpdateCollisionBox()
{
  namespace cloud = constants::cloud;

  mCollisionBox =
  {
    mX - 0.5f * cloud::sizeX,
    mY - 0.5f * cloud::sizeY,
    cloud::sizeX,
    cloud::sizeY,
  };
}

bool
Cloud::isHit(
  const float x,
  const float y ) const
{
  const SDL_FPoint point {x, y};

  return SDL_PointInFRect(&point, &mCollisionBox);
}

void
Cloud::Draw()
{
  namespace cloud = constants::cloud;


  const auto& features = gameState().features;

  if ( features.extraClouds == false && mId >= 2 )
    return;


  auto* const cloudTexture =
    mIsOpaque == true
    ? textures.texture_cloud_opaque
    : textures.texture_cloud;

  const SDL_FRect cloudRect
  {
    toWindowSpaceX(mX - 0.5f * cloud::sizeX),
    toWindowSpaceY(mY - 0.5f * cloud::sizeY),
    scaleToScreenX(cloud::sizeX),
    scaleToScreenY(cloud::sizeY),
  };

  SDL_RenderCopyF(
    gRenderer,
    cloudTexture,
    nullptr,
    &cloudRect );
}

void
Cloud::DrawCollisionLayer()
{
  const auto& features = gameState().features;

  if ( features.extraClouds == false && mId >= 2 )
    return;


  const SDL_FRect cloudHitbox
  {
    toWindowSpaceX(mCollisionBox.x),
    toWindowSpaceY(mCollisionBox.y),
    scaleToScreenX(mCollisionBox.w),
    scaleToScreenY(mCollisionBox.h),
  };

  namespace colors = constants::colors::debug::collisions;
  setRenderColor(colors::cloudToPlane);
  SDL_RenderDrawRectF( gRenderer, &cloudHitbox );
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
  namespace cloud = constants::cloud;


  if ( mDir == true )
    mX = cloud::spawnRightX + (clouds.size() - mId) * cloud::sizeY;
  else
    mX = cloud::spawnLeftX + (clouds.size() - mId) * cloud::sizeY;


  if ( mId % 2 )
    mY = cloud::maxHeight + (clouds.size() + mId) / (float) clouds.size() * cloud::sizeY;
  else
    mY = cloud::minHeight - (clouds.size() + mId) / (float) clouds.size() * cloud::sizeY;


  mIsOpaque = true;
}
