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


namespace bindings
{

KeyBindings player1 = defaults::player1;
KeyBindings player2 = defaults::player2;

namespace defaults
{

const KeyBindings player1
{
//  THROTTLE_UP
  SDL_SCANCODE_UP,

//  THROTTLE_DOWN
  SDL_SCANCODE_DOWN,

//  TURN_LEFT
  SDL_SCANCODE_LEFT,

//  TURN_RIGHT
  SDL_SCANCODE_RIGHT,

//  FIRE
  SDL_SCANCODE_SPACE,

//  JUMP
  SDL_SCANCODE_LCTRL,
};

const KeyBindings player2
{
//  THROTTLE_UP
  SDL_SCANCODE_I,

//  THROTTLE_DOWN
  SDL_SCANCODE_K,

//  TURN_LEFT
  SDL_SCANCODE_J,

//  TURN_RIGHT
  SDL_SCANCODE_L,

//  FIRE
  SDL_SCANCODE_E,

//  JUMP
  SDL_SCANCODE_Q,
};

} // namespace defaults

} // namespace bindings


struct
{
  Uint8 current[SDL_NUM_SCANCODES] {};
  decltype(current) previous {};

} static keyboardState {};


void
KeyBindings::verifyAndFix(
  const KeyBindings& fallback )
{
  if ( fire >= SDL_NUM_SCANCODES )
    fire = fallback.fire;

  if ( jump >= SDL_NUM_SCANCODES )
    jump = fallback.jump;

  if ( throttleDown >= SDL_NUM_SCANCODES )
    throttleDown = fallback.throttleDown;

  if ( throttleUp >= SDL_NUM_SCANCODES )
    throttleUp = fallback.throttleUp;

  if ( turnLeft >= SDL_NUM_SCANCODES )
    turnLeft = fallback.turnLeft;

  if ( turnRight >= SDL_NUM_SCANCODES )
    turnRight = fallback.turnRight;
}

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
  if ( false )
    ;

  else if ( newBinding == bindings::player1.throttleUp )
    bindings::player1.throttleUp = targetBinding;

  else if ( newBinding == bindings::player2.throttleUp )
    bindings::player2.throttleUp = targetBinding;

  else if ( newBinding == bindings::player1.throttleDown )
    bindings::player1.throttleDown = targetBinding;

  else if ( newBinding == bindings::player2.throttleDown )
    bindings::player2.throttleDown = targetBinding;

  else if ( newBinding == bindings::player1.turnLeft )
    bindings::player1.turnLeft = targetBinding;

  else if ( newBinding == bindings::player2.turnLeft )
    bindings::player2.turnLeft = targetBinding;

  else if ( newBinding == bindings::player1.turnRight )
    bindings::player1.turnRight = targetBinding;

  else if ( newBinding == bindings::player2.turnRight )
    bindings::player2.turnRight = targetBinding;

  else if ( newBinding == bindings::player1.fire )
    bindings::player1.fire = targetBinding;

  else if ( newBinding == bindings::player2.fire )
    bindings::player2.fire = targetBinding;

  else if ( newBinding == bindings::player1.jump )
    bindings::player1.jump = targetBinding;

  else if ( newBinding == bindings::player2.jump )
    bindings::player2.jump = targetBinding;


  targetBinding = newBinding;
}

Controls
getLocalControls(
  const KeyBindings& bindings )
{
  Controls controls {};

  if (  isKeyDown(bindings.throttleUp) == true &&
        isKeyDown(bindings.throttleDown) == false )
    controls.throttle = PLANE_THROTTLE::THROTTLE_INCREASE;

  else if ( isKeyDown(bindings.throttleDown) == true &&
            isKeyDown(bindings.throttleUp) == false )
    controls.throttle = PLANE_THROTTLE::THROTTLE_DECREASE;

  else
    controls.throttle = PLANE_THROTTLE::THROTTLE_IDLE;


  if (  isKeyDown(bindings.turnLeft) == true &&
        isKeyDown(bindings.turnRight) == false )
    controls.pitch = PLANE_PITCH::PITCH_LEFT;

  else if ( isKeyDown(bindings.turnRight) == true &&
            isKeyDown(bindings.turnLeft) == false )
    controls.pitch = PLANE_PITCH::PITCH_RIGHT;

  else
    controls.pitch = PLANE_PITCH::PITCH_IDLE;


//  SHOOT
  if ( isKeyDown(bindings.fire) == true )
    controls.shoot = true;
  else
    controls.shoot = false;


//  EJECT
  if ( isKeyDown(bindings.jump) == true )
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
