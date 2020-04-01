//    Biplanes Revival
//    Copyright (C) 2019-2020 Regular-dev community
//    https://regular-dev.org/
//    regular.dev.org@gmail.com
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include "include/variables.h"


Zeppelin::Zeppelin()
{
  Respawn();
}

void Zeppelin::Update()
{
  UpdateCoordinates();
  Draw();
  DrawScore();
}

void Zeppelin::UpdateCoordinates()
{
  x += sizes.zeppelin_speed * deltaTime;
  if ( x > sizes.screen_width + sizes.zeppelin_sizex / 2 )
  {
    x = - sizes.zeppelin_sizex / 2;
    if ( ascent )
    {
      y -= sizes.zeppelin_sizey / 2;
      if ( y < sizes.zeppelin_highest_y )
        ascent = false;
    }
    else
    {
      y += sizes.zeppelin_sizey / 2;
      if ( y > sizes.zeppelin_lowest_y )
        ascent = true;
    }
  }
}

void Zeppelin::Draw()
{
  textures.destrect.x = x - sizes.zeppelin_sizex / 2;
  textures.destrect.y = y - sizes.zeppelin_sizey / 2;
  textures.destrect.w = sizes.zeppelin_sizex;
  textures.destrect.h = sizes.zeppelin_sizey;
  SDL_RenderCopy( gRenderer,
                  textures.texture_zeppelin,
                  NULL,
                  &textures.destrect );
}

void Zeppelin::DrawScore()
{
  textures.destrect.x = x - sizes.zeppelin_score_sizex * 2.1f;
  textures.destrect.y = y - sizes.zeppelin_score_sizey * 0.95f;
  textures.destrect.w = sizes.zeppelin_score_sizex;
  textures.destrect.h = sizes.zeppelin_score_sizey;

  // Blue score
  SDL_RenderCopy( gRenderer,
                  textures.font_zeppelin_score,
                  &textures.zeppelin_score_rect[ plane_blue.getScore() / 10 ],
                  &textures.destrect );

  textures.destrect.x = x - sizes.zeppelin_score_sizex * 1.1f;
  SDL_RenderCopy( gRenderer,
                  textures.font_zeppelin_score,
                  &textures.zeppelin_score_rect[ plane_blue.getScore() % 10 ],
                  &textures.destrect );
  // Red score
  textures.destrect.x = x + sizes.zeppelin_score_sizex * 1.75f;
  SDL_RenderCopy( gRenderer,
                  textures.font_zeppelin_score,
                  &textures.zeppelin_score_rect[ 10 + plane_red.getScore() % 10 ],
                  &textures.destrect );

  textures.destrect.x = x + sizes.zeppelin_score_sizex * 0.75f;
  SDL_RenderCopy( gRenderer,
                  textures.font_zeppelin_score,
                  &textures.zeppelin_score_rect[ 10 + plane_red.getScore() / 10 ],
                  &textures.destrect );
}

void Zeppelin::Respawn()
{
  x = sizes.zeppelin_spawn_x;
  y = sizes.zeppelin_lowest_y - sizes.zeppelin_highest_y + sizes.zeppelin_sizey;
}
