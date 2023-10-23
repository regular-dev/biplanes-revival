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

#include <SDL_keycode.h>


struct Controls
{
  PLANE_PITCH pitch {};
  PLANE_THROTTLE throttle {};
  bool fire {};
  bool jump {};


  Controls() = default;
};


extern Controls controls_local;
extern Controls controls_opponent;


void readLocalInput();
void readOpponentInput();
void processLocalControls( Plane&, const Controls& );


extern const SDL_Keycode DEFAULT_THROTTLE_UP;
extern const SDL_Keycode DEFAULT_THROTTLE_DOWN;
extern const SDL_Keycode DEFAULT_TURN_LEFT;
extern const SDL_Keycode DEFAULT_TURN_RIGHT;
extern const SDL_Keycode DEFAULT_FIRE;
extern const SDL_Keycode DEFAULT_JUMP;

extern SDL_Keycode THROTTLE_UP;
extern SDL_Keycode THROTTLE_DOWN;
extern SDL_Keycode TURN_LEFT;
extern SDL_Keycode TURN_RIGHT;
extern SDL_Keycode FIRE;
extern SDL_Keycode JUMP;
