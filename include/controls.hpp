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

#pragma once

#include <include/fwd.hpp>
#include <include/enums.hpp>

#include <SDL_scancode.h>


struct Controls
{
  PLANE_PITCH pitch {};
  PLANE_THROTTLE throttle {};
  bool shoot {};
  bool jump {};


  Controls() = default;
};


void readKeyboardInput();

bool isKeyDown( const SDL_Scancode );
bool isKeyPressed( const SDL_Scancode );
bool isKeyReleased( const SDL_Scancode );

Controls getLocalControls();
void processPlaneControls( Plane&, const Controls& );

void assignKeyBinding(
  SDL_Scancode& targetBinding,
  const SDL_Scancode newBinding );


extern const SDL_Scancode DEFAULT_THROTTLE_UP;
extern const SDL_Scancode DEFAULT_THROTTLE_DOWN;
extern const SDL_Scancode DEFAULT_TURN_LEFT;
extern const SDL_Scancode DEFAULT_TURN_RIGHT;
extern const SDL_Scancode DEFAULT_FIRE;
extern const SDL_Scancode DEFAULT_JUMP;

extern SDL_Scancode THROTTLE_UP;
extern SDL_Scancode THROTTLE_DOWN;
extern SDL_Scancode TURN_LEFT;
extern SDL_Scancode TURN_RIGHT;
extern SDL_Scancode FIRE;
extern SDL_Scancode JUMP;
