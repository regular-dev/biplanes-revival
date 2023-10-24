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
#include <include/plane.hpp>
#include <include/sizes.hpp>
#include <include/textures.hpp>


Zeppelin::Zeppelin()
{
  Respawn();
}

void
Zeppelin::Update()
{
  mX += sizes.zeppelin_speed * deltaTime;

  if ( mX <= sizes.screen_width + sizes.zeppelin_sizex / 2.0f )
    return;


  mX = -sizes.zeppelin_sizex / 2.0f;

  if ( mIsAscending == true )
  {
    mY -= sizes.zeppelin_sizey / 2.0f;

    if ( mY < sizes.zeppelin_highest_y )
      mIsAscending = false;

    return;
  }

  mY += sizes.zeppelin_sizey / 2.0f;

  if ( mY > sizes.zeppelin_lowest_y )
    mIsAscending = true;
}

void
Zeppelin::Draw()
{
  const SDL_Rect zeppelinRect
  {
    mX - sizes.zeppelin_sizex / 2.0f,
    mY - sizes.zeppelin_sizey / 2.0f,
    sizes.zeppelin_sizex,
    sizes.zeppelin_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.texture_zeppelin,
    nullptr,
    &zeppelinRect );


  const auto& planeRed = planes.at(PLANE_TYPE::RED);
  const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);

  SDL_Rect scoreRect
  {
    mX - sizes.zeppelin_score_sizex * 2.1f,
    mY - sizes.zeppelin_score_sizey * 0.95f,
    sizes.zeppelin_score_sizex,
    sizes.zeppelin_score_sizey,
  };


//  Blue score
  SDL_RenderCopy(
    gRenderer,
    textures.font_zeppelin_score,
    &textures.zeppelin_score_rect[planeBlue.score() / 10],
    &scoreRect );

  scoreRect.x = mX - sizes.zeppelin_score_sizex * 1.1f;

  SDL_RenderCopy(
    gRenderer,
    textures.font_zeppelin_score,
    &textures.zeppelin_score_rect[planeBlue.score() % 10],
    &scoreRect );


//  Red score
  scoreRect.x = mX + sizes.zeppelin_score_sizex * 1.75f;

  SDL_RenderCopy(
    gRenderer,
    textures.font_zeppelin_score,
    &textures.zeppelin_score_rect[10 + planeRed.score() % 10],
    &scoreRect );

  scoreRect.x = mX + sizes.zeppelin_score_sizex * 0.75f;

  SDL_RenderCopy(
    gRenderer,
    textures.font_zeppelin_score,
    &textures.zeppelin_score_rect[10 + planeRed.score() / 10],
    &scoreRect );
}

void
Zeppelin::Respawn()
{
  mX = sizes.zeppelin_spawn_x;
  mY = sizes.zeppelin_lowest_y - sizes.zeppelin_highest_y + sizes.zeppelin_sizey;
}
