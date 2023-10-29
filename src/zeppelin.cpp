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

#include <include/zeppelin.hpp>
#include <include/sdl.hpp>
#include <include/time.hpp>
#include <include/constants.hpp>
#include <include/plane.hpp>
#include <include/textures.hpp>


Zeppelin::Zeppelin()
{
  Respawn();
}

void
Zeppelin::Update()
{
  namespace zeppelin = constants::zeppelin;


  mX += zeppelin::speed * deltaTime;

  if ( mX <= 1.0f + 0.5f * zeppelin::sizeX )
    return;


  mX = -0.5f * zeppelin::sizeX;

  if ( mIsAscending == true )
  {
    mY -= 0.5f * zeppelin::sizeY;

    if ( mY < zeppelin::maxHeight )
      mIsAscending = false;

    return;
  }

  mY += 0.5f * zeppelin::sizeY;

  if ( mY > zeppelin::minHeight )
    mIsAscending = true;
}

void
Zeppelin::Draw()
{
  namespace zeppelin = constants::zeppelin;
  namespace score = zeppelin::score;


  const SDL_FRect zeppelinRect
  {
    toWindowSpaceX(mX - 0.5f * zeppelin::sizeX),
    toWindowSpaceY(mY - 0.5f * zeppelin::sizeY),
    scaleToScreenX(zeppelin::sizeX),
    scaleToScreenY(zeppelin::sizeY),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.texture_zeppelin,
    nullptr,
    &zeppelinRect );


  const auto& planeRed = planes.at(PLANE_TYPE::RED);
  const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);

  SDL_FRect scoreRect
  {
    toWindowSpaceX(mX - score::numOffsetBlue1X),
    toWindowSpaceY(mY - score::numOffsetY),
    scaleToScreenX(score::sizeX),
    scaleToScreenY(score::sizeY),
  };


//  Blue score
  SDL_RenderCopyF(
    gRenderer,
    textures.font_zeppelin_score,
    &textures.zeppelin_score_rect[planeBlue.score() / 10],
    &scoreRect );

  scoreRect.x = toWindowSpaceX(mX - score::numOffsetBlue2X);

  SDL_RenderCopyF(
    gRenderer,
    textures.font_zeppelin_score,
    &textures.zeppelin_score_rect[planeBlue.score() % 10],
    &scoreRect );


//  Red score
  scoreRect.x = toWindowSpaceX(mX + score::numOffsetRed1X);

  SDL_RenderCopyF(
    gRenderer,
    textures.font_zeppelin_score,
    &textures.zeppelin_score_rect[10 + planeRed.score() % 10],
    &scoreRect );

  scoreRect.x = toWindowSpaceX(mX + score::numOffsetRed2X);

  SDL_RenderCopyF(
    gRenderer,
    textures.font_zeppelin_score,
    &textures.zeppelin_score_rect[10 + planeRed.score() / 10],
    &scoreRect );
}

void
Zeppelin::Respawn()
{
  namespace zeppelin = constants::zeppelin;

  mX = zeppelin::spawnX;
  mY = zeppelin::minHeight - zeppelin::maxHeight + zeppelin::sizeX;
}
