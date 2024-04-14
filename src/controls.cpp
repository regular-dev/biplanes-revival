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

#include <include/controls.hpp>
#include <include/plane.hpp>

#include <SDL_keyboard.h>

#include <cstring>


const SDL_Scancode DEFAULT_THROTTLE_UP   = SDL_SCANCODE_UP;
const SDL_Scancode DEFAULT_THROTTLE_DOWN = SDL_SCANCODE_DOWN;
const SDL_Scancode DEFAULT_TURN_LEFT     = SDL_SCANCODE_LEFT;
const SDL_Scancode DEFAULT_TURN_RIGHT    = SDL_SCANCODE_RIGHT;
const SDL_Scancode DEFAULT_FIRE          = SDL_SCANCODE_SPACE;
const SDL_Scancode DEFAULT_JUMP          = SDL_SCANCODE_LCTRL;

SDL_Scancode THROTTLE_UP   = DEFAULT_THROTTLE_UP;
SDL_Scancode THROTTLE_DOWN = DEFAULT_THROTTLE_DOWN;
SDL_Scancode TURN_LEFT     = DEFAULT_TURN_LEFT;
SDL_Scancode TURN_RIGHT    = DEFAULT_TURN_RIGHT;
SDL_Scancode FIRE          = DEFAULT_FIRE;
SDL_Scancode JUMP          = DEFAULT_JUMP;


struct
{
  Uint8 current[SDL_NUM_SCANCODES] {};
  decltype(current) previous {};

} static keyboardState {};


void
readKeyboardInput()
{
  std::memcpy(
    keyboardState.previous,
    keyboardState.current,
    sizeof(keyboardState.previous) );

  std::memcpy(
    keyboardState.current,
    SDL_GetKeyboardState({}),
    sizeof(keyboardState.current) );
}

bool
isKeyDown(
  const SDL_Scancode key )
{
  return keyboardState.current[key] == true;
}

bool
isKeyPressed(
  const SDL_Scancode key )
{
  return
    isKeyDown(key) == true &&
    keyboardState.previous[key] == false;
}

bool
isKeyReleased(
  const SDL_Scancode key )
{
  return
    isKeyDown(key) == false &&
    keyboardState.previous[key] == true;
}


void
assignKeyBinding(
  SDL_Scancode& targetBinding,
  const SDL_Scancode newBinding )
{
  if ( newBinding == THROTTLE_UP )
    THROTTLE_UP = targetBinding;

  else if ( newBinding == THROTTLE_DOWN )
    THROTTLE_DOWN = targetBinding;

  else if ( newBinding == TURN_LEFT )
    TURN_LEFT = targetBinding;

  else if ( newBinding == TURN_RIGHT )
    TURN_RIGHT = targetBinding;

  else if ( newBinding == FIRE )
    FIRE = targetBinding;

  else if ( newBinding == JUMP )
    JUMP = targetBinding;


  targetBinding = newBinding;
}

Controls
getLocalControls()
{
  Controls controls {};

  if (  isKeyDown(THROTTLE_UP) == true &&
        isKeyDown(THROTTLE_DOWN) == false )
    controls.throttle = PLANE_THROTTLE::THROTTLE_INCREASE;

  else if ( isKeyDown(THROTTLE_DOWN) == true &&
            isKeyDown(THROTTLE_UP) == false )
    controls.throttle = PLANE_THROTTLE::THROTTLE_DECREASE;

  else
    controls.throttle = PLANE_THROTTLE::THROTTLE_IDLE;


  if (  isKeyDown(TURN_LEFT) == true &&
        isKeyDown(TURN_RIGHT) == false )
    controls.pitch = PLANE_PITCH::PITCH_LEFT;

  else if ( isKeyDown(TURN_RIGHT) == true &&
            isKeyDown(TURN_LEFT) == false )
    controls.pitch = PLANE_PITCH::PITCH_RIGHT;

  else
    controls.pitch = PLANE_PITCH::PITCH_IDLE;


//  SHOOT
  if ( isKeyDown(FIRE) == true )
    controls.shoot = true;
  else
    controls.shoot = false;


//  EJECT
  if ( isKeyDown(JUMP) == true )
    controls.jump = true;
  else
    controls.jump = false;

  return controls;
}


void
processPlaneControls(
  Plane& plane,
  const Controls& controls )
{
  if ( controls.throttle == PLANE_THROTTLE::THROTTLE_INCREASE )
    plane.input.Accelerate();

  else if ( controls.throttle == PLANE_THROTTLE::THROTTLE_DECREASE )
    plane.input.Decelerate();


  if ( controls.pitch == PLANE_PITCH::PITCH_LEFT )
    plane.input.TurnLeft();

  else if ( controls.pitch == PLANE_PITCH::PITCH_RIGHT )
    plane.input.TurnRight();

  else
    plane.input.TurnIdle();


  if ( controls.shoot == true )
    plane.input.Shoot();

  if ( controls.jump == true )
    plane.input.Jump();
  else
    plane.pilot.ChuteUnlock();
}
