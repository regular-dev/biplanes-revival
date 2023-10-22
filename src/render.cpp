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

#include <include/render.h>
#include <include/sdl.h>
#include <include/game_state.hpp>
#include <include/plane.h>
#include <include/sizes.hpp>
#include <include/textures.hpp>
#include <include/variables.h>

#include <string>


void
draw_text(
  const char text[],
  const int x, const int y )
{
  for ( size_t i = 0; i < strlen(text); i++ )
  {
    const SDL_Rect textRect
    {
      x + sizes.text_sizex * i,
      y,
      sizes.text_sizex,
      sizes.text_sizey,
    };

    SDL_RenderCopy(
      gRenderer,
      textures.main_font,
      &textures.font_rect[text[i] - 32],
      &textRect  );
  }
}

void
draw_background()
{
  SDL_RenderClear(gRenderer);


  const SDL_Rect backgroundRect
  {
    0, 0,
    sizes.screen_width,
    sizes.screen_height,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.texture_background,
    nullptr,
    &backgroundRect );


  if ( gameState().debug.collisions == true )
  {
    SDL_SetRenderDrawColor( gRenderer, 255, 255, 0, 1 );
    SDL_RenderDrawLine(
      gRenderer,
      0,
      sizes.ground_y_collision,
      sizes.screen_width,
      sizes.ground_y_collision );

    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawLine(
      gRenderer,
      0,
      sizes.bullet_ground_collision,
      sizes.screen_width,
      sizes.bullet_ground_collision );
  }
}

void
draw_barn()
{
  const SDL_Rect barnRect
  {
    sizes.screen_width * 0.5f - sizes.barn_sizex * 0.5,
    sizes.screen_height * 0.808,
    sizes.barn_sizex,
    sizes.barn_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.texture_barn,
    nullptr,
    &barnRect );


  if ( gameState().debug.collisions == true )
  {
    SDL_Rect barnHitbox
    {
      sizes.barn_x_bullet_collision,
      sizes.barn_y_bullet_collision,
      sizes.barn_sizex * 0.95f,
      sizes.barn_sizey,
    };

    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &barnHitbox );

    barnHitbox.x = sizes.barn_x_collision;
    barnHitbox.y = sizes.barn_y_collision;

    SDL_SetRenderDrawColor( gRenderer, 255, 255, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &barnHitbox );

    barnHitbox.y = sizes.ground_y_pilot_collision;
    barnHitbox.x = sizes.barn_x_pilot_left_collision;
    barnHitbox.w = sizes.barn_x_pilot_right_collision - sizes.barn_x_pilot_left_collision;

    SDL_SetRenderDrawColor( gRenderer, 0, 255, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &barnHitbox );
  }
}

void
draw_score()
{
  const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  const auto& planeRed = planes.at(PLANE_TYPE::RED);

  char text [5];

  sprintf( text, "%u-%u", planeBlue.score(), planeRed.score() );
  draw_text( text, sizes.screen_width * 0.45, sizes.screen_height * 0.5 );
}

void
display_update()
{
  SDL_RenderPresent(gRenderer);
//  SDL_UpdateWindowSurface(gWindow);
}
