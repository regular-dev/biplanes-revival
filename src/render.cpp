/*
  Biplanes Revival
  Copyright (C) 2019-2024 Regular-dev community
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

#include <cmath>
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
draw_circle(
  const float x, const float y,
  const float radius,
  const size_t segments )
{
  const size_t degreeStep = 360 / std::max(segments, size_t{1});

  for ( size_t i {}; i < 360; i += degreeStep )
  {
    const auto dir = i * M_PI / 180.f;
    const auto nextDir = (i + degreeStep) * M_PI / 180.f;

    const SDL_FPoint from
    {
      x + radius * std::sin(dir),
      y + radius * std::cos(dir) * constants::aspectRatio,
    };

    const SDL_FPoint to
    {
      x + radius * std::sin(nextDir),
      y + radius * std::cos(nextDir) * constants::aspectRatio,
    };

    SDL_RenderDrawLine(
      gRenderer,
      toWindowSpaceX(from.x),
      toWindowSpaceY(from.y),
      toWindowSpaceX(to.x),
      toWindowSpaceY(to.y) );
  }
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
}

void
draw_ground_collision_layer()
{
  namespace plane = constants::plane;
  namespace pilot = constants::pilot;
  namespace bullet = constants::bullet;
  namespace colors = constants::colors::debug::collisions;


  setRenderColor(colors::planeToObstacles);
  SDL_RenderDrawLineF(
    gRenderer,
    toWindowSpaceX(0.0f),
    toWindowSpaceY(plane::groundCollision),
    toWindowSpaceX(1.0f),
    toWindowSpaceY(plane::groundCollision) );

  setRenderColor(colors::pilotToObstacles);
  SDL_RenderDrawLineF(
    gRenderer,
    toWindowSpaceX(0.0f),
    toWindowSpaceY(pilot::groundCollision),
    toWindowSpaceX(1.0f),
    toWindowSpaceY(pilot::groundCollision) );

  setRenderColor(colors::bulletToObstacles);
  SDL_RenderDrawLineF(
    gRenderer,
    toWindowSpaceX(0.0f),
    toWindowSpaceY(bullet::groundCollision),
    toWindowSpaceX(1.0f),
    toWindowSpaceY(bullet::groundCollision) );
}

void
draw_barn()
{
  namespace barn = constants::barn;
  namespace pilot = constants::pilot;


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
}

void
draw_barn_collision_layer()
{
  namespace barn = constants::barn;
  namespace pilot = constants::pilot;
  namespace colors = constants::colors::debug::collisions;


  SDL_FRect barnHitbox
  {
    toWindowSpaceX(barn::bulletCollisionX),
    toWindowSpaceY(barn::bulletCollisionY),
    scaleToScreenX(barn::bulletCollisionSizeX),
    scaleToScreenY(barn::sizeY),
  };

  setRenderColor(colors::bulletToObstacles);
  SDL_RenderDrawRectF( gRenderer, &barnHitbox );

  barnHitbox.x = toWindowSpaceX(barn::planeCollisionX);
  barnHitbox.y = toWindowSpaceY(barn::planeCollisionY);
  barnHitbox.w = scaleToScreenX(barn::sizeX);

  setRenderColor(colors::planeToObstacles);
  SDL_RenderDrawRectF( gRenderer, &barnHitbox );

  barnHitbox.x = toWindowSpaceX(barn::pilotCollisionLeftX);
  barnHitbox.y = toWindowSpaceY(barn::posY);
  barnHitbox.w = scaleToScreenX(barn::pilotCollisionRightX - barn::pilotCollisionLeftX);
  barnHitbox.h = toWindowSpaceY(1.f) - barnHitbox.y;

  setRenderColor(colors::pilotRescueZone);
  SDL_RenderDrawRectF( gRenderer, &barnHitbox );
}

void
draw_score()
{
  const auto maxWinScoreTextLength =
    std::to_string(constants::maxWinScore).size();


  const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  const auto& planeRed = planes.at(PLANE_TYPE::RED);

  auto textBlueScore =
    std::to_string(planeBlue.score());

  auto textRedScore =
    std::to_string(planeRed.score());

  while ( textBlueScore.size() < maxWinScoreTextLength )
    textBlueScore.insert(textBlueScore.begin(), ' ');

  while ( textRedScore.size() < maxWinScoreTextLength )
    textRedScore.push_back(' ');

  const auto text =
    textBlueScore + "-" + textRedScore;

  const auto textOffset =
    0.5f - constants::text::sizeX * text.size() / 2.f;

  draw_text( text, textOffset, 0.5f );
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
