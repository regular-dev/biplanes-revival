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

#include <include/render.hpp>
#include <include/sdl.hpp>
#include <include/canvas.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/plane.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>

#include <string>


void
draw_text(
  const char text[],
  const float x, const float y )
{
  namespace Text = constants::text;


  for ( size_t i = 0; i < strlen(text); i++ )
  {
    const SDL_Rect textRect
    {
      toWindowSpaceX(x + Text::sizeX * i),
      toWindowSpaceY(y),
      scaleToScreenX(Text::sizeX),
      scaleToScreenY(Text::sizeY),
    };

    SDL_RenderCopy(
      gRenderer,
      textures.main_font,
      &textures.font_rect[text[i] - 32],
      &textRect );
  }
}

void
draw_text(
  const std::string& text,
  const float x, const float y )
{
  return draw_text(text.c_str(), x, y);
}

void
draw_background()
{
  namespace plane = constants::plane;
  namespace bullet = constants::bullet;
  namespace colors = constants::colors;


  setRenderColor(colors::background);
  SDL_RenderClear(gRenderer);


  const SDL_FRect backgroundRect
  {
    toWindowSpaceX(0.0f),
    toWindowSpaceY(0.0f),
    scaleToScreenX(1.0f),
    scaleToScreenY(1.0f),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.texture_background,
    nullptr,
    &backgroundRect );


  if ( gameState().debug.collisions == true )
  {
    setRenderColor(colors::planeHitbox);
    SDL_RenderDrawLineF(
      gRenderer,
      toWindowSpaceX(0.0f),
      toWindowSpaceY(plane::groundCollision),
      toWindowSpaceX(1.0f),
      toWindowSpaceY(plane::groundCollision) );

    setRenderColor(colors::bulletHitbox);
    SDL_RenderDrawLineF(
      gRenderer,
      toWindowSpaceX(0.0f),
      toWindowSpaceY(bullet::groundCollision),
      toWindowSpaceX(1.0f),
      toWindowSpaceY(bullet::groundCollision) );
  }
}

void
draw_barn()
{
  namespace barn = constants::barn;
  namespace pilot = constants::pilot;
  namespace colors = constants::colors;


  const SDL_FRect barnRect
  {
    toWindowSpaceX(barn::posX),
    toWindowSpaceY(barn::posY),
    scaleToScreenX(barn::sizeX),
    scaleToScreenY(barn::sizeY),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.texture_barn,
    nullptr,
    &barnRect );


  if ( gameState().debug.collisions == true )
  {
    SDL_FRect barnHitbox
    {
      toWindowSpaceX(barn::bulletCollisionX),
      toWindowSpaceY(barn::bulletCollisionY),
      scaleToScreenX(barn::bulletCollisionSizeX),
      scaleToScreenY(barn::sizeY),
    };

    setRenderColor(colors::bulletHitbox);
    SDL_RenderDrawRectF( gRenderer, &barnHitbox );

    barnHitbox.x = toWindowSpaceX(barn::planeCollisionX);
    barnHitbox.y = toWindowSpaceY(barn::planeCollisionY);

    setRenderColor(colors::planeHitbox);
    SDL_RenderDrawRectF( gRenderer, &barnHitbox );

    barnHitbox.x = toWindowSpaceX(barn::pilotCollisionLeftX);
    barnHitbox.y = toWindowSpaceY(pilot::groundCollision);
    barnHitbox.w = scaleToScreenX(barn::pilotCollisionRightX - barn::pilotCollisionLeftX);

    setRenderColor(colors::pilotHitbox);
    SDL_RenderDrawRectF( gRenderer, &barnHitbox );
  }
}

void
draw_score()
{
  const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  const auto& planeRed = planes.at(PLANE_TYPE::RED);

  const auto text =
    std::to_string(planeBlue.score()) + "-" +
    std::to_string(planeRed.score());

  draw_text( text, 0.45f, 0.5f );
}

void
draw_menu_rect()
{
  namespace menu = constants::menu;


  const SDL_FRect menuRect
  {
    toWindowSpaceX(menu::originX),
    toWindowSpaceY(menu::originY),
    scaleToScreenX(menu::sizeX),
    scaleToScreenY(menu::sizeY),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.menu_box,
    nullptr,
    &menuRect );
}

void
draw_window_letterbox()
{
  const SDL_FRect rectLeft
  {
    0.0f,
    0.0f,
    toWindowSpaceX(0.0f),
    canvas.windowHeight,
  };

  const SDL_FRect rectRight
  {
    toWindowSpaceX(1.0f),
    0.0f,
    canvas.windowWidth - toWindowSpaceX(1.0f),
    canvas.windowHeight,
  };

  const SDL_FRect rectTop
  {
    0.0f,
    0.0f,
    canvas.windowWidth,
    toWindowSpaceY(0.0f),
  };

  const SDL_FRect rectBottom
  {
    0.0f,
    toWindowSpaceY(1.0f),
    canvas.windowWidth,
    canvas.windowHeight - toWindowSpaceY(1.0f),
  };

  setRenderColor(constants::colors::letterbox);

  SDL_RenderFillRectF(
    gRenderer,
    &rectLeft );

  SDL_RenderFillRectF(
    gRenderer,
    &rectRight );

  SDL_RenderFillRectF(
    gRenderer,
    &rectTop );

  SDL_RenderFillRectF(
    gRenderer,
    &rectBottom );
}

void
display_update()
{
  SDL_RenderPresent(gRenderer);
}
