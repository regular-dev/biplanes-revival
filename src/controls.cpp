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

#include <include/controls.hpp>
#include <include/plane.hpp>
#include <include/network_data.hpp>

#include <SDL_keyboard.h>


const SDL_Keycode DEFAULT_THROTTLE_UP   = SDLK_UP;
const SDL_Keycode DEFAULT_THROTTLE_DOWN = SDLK_DOWN;
const SDL_Keycode DEFAULT_TURN_LEFT     = SDLK_LEFT;
const SDL_Keycode DEFAULT_TURN_RIGHT    = SDLK_RIGHT;
const SDL_Keycode DEFAULT_FIRE          = SDLK_SPACE;
const SDL_Keycode DEFAULT_JUMP          = SDLK_LCTRL;

SDL_Keycode THROTTLE_UP   = SDLK_UP;
SDL_Keycode THROTTLE_DOWN = SDLK_DOWN;
SDL_Keycode TURN_LEFT     = SDLK_LEFT;
SDL_Keycode TURN_RIGHT    = SDLK_RIGHT;
SDL_Keycode FIRE          = SDLK_SPACE;
SDL_Keycode JUMP          = SDLK_LCTRL;


void
readLocalInput()
{
  const Uint8* keyboard_state = SDL_GetKeyboardState({});

  if (  keyboard_state[SDL_GetScancodeFromKey(THROTTLE_UP)] == 1 &&
        keyboard_state[SDL_GetScancodeFromKey(THROTTLE_DOWN)] == 0 )
    controls_local.throttle = PLANE_THROTTLE::THROTTLE_INCREASE;

  else if ( keyboard_state[SDL_GetScancodeFromKey(THROTTLE_DOWN)] == 1 &&
            keyboard_state[SDL_GetScancodeFromKey(THROTTLE_UP)] == 0 )
    controls_local.throttle = PLANE_THROTTLE::THROTTLE_DECREASE;

  else
    controls_local.throttle = PLANE_THROTTLE::THROTTLE_IDLE;


  if (  keyboard_state[SDL_GetScancodeFromKey(TURN_LEFT)] == 1 &&
        keyboard_state[SDL_GetScancodeFromKey(TURN_RIGHT)] == 0 )
    controls_local.pitch = PLANE_PITCH::PITCH_LEFT;

  else if ( keyboard_state[SDL_GetScancodeFromKey(TURN_RIGHT)] == 1 &&
            keyboard_state[SDL_GetScancodeFromKey(TURN_LEFT)] == 0 )
    controls_local.pitch = PLANE_PITCH::PITCH_RIGHT;

  else
    controls_local.pitch = PLANE_PITCH::PITCH_IDLE;


//  SHOOT
  if ( keyboard_state[SDL_GetScancodeFromKey(FIRE)] == 1 )
    controls_local.fire = true;
  else
    controls_local.fire = false;


//  EJECT
  if ( keyboard_state[SDL_GetScancodeFromKey(JUMP)] == 1 )
    controls_local.jump = true;
  else
    controls_local.jump = false;
}


void
readOpponentInput()
{
  controls_opponent.throttle = opponentData.throttle;
  controls_opponent.pitch    = opponentData.pitch;
}


void
processLocalControls(
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


  if ( controls.fire == true )
    plane.input.Shoot();

  if ( controls.jump == true )
    plane.input.Jump();
  else
    plane.pilot.ChuteUnlock();
}
