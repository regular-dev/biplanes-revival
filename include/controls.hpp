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

struct KeyBindings
{
  SDL_Scancode throttleUp {};
  SDL_Scancode throttleDown {};
  SDL_Scancode turnLeft {};
  SDL_Scancode turnRight {};

  SDL_Scancode fire {};
  SDL_Scancode jump {};


  KeyBindings() = default;

  void verifyAndFix( const KeyBindings& fallback );
};


namespace bindings
{

extern KeyBindings player1;
extern KeyBindings player2;

namespace defaults
{

extern const KeyBindings player1;
extern const KeyBindings player2;

} // namespace defaults

} // namespace bindings


void readKeyboardInput();

bool isKeyDown( const SDL_Scancode );
bool isKeyPressed( const SDL_Scancode );
bool isKeyReleased( const SDL_Scancode );

Controls getLocalControls( const KeyBindings& = bindings::player1 );
void processPlaneControls( Plane&, const Controls& );

void assignKeyBinding(
  SDL_Scancode& targetBinding,
  const SDL_Scancode newBinding );
