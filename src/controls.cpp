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


#include "include/controls.h"
#include "include/variables.h"


// Process input
void collect_local_input()
{
  const Uint8 *keyboard_state = SDL_GetKeyboardState( NULL );

  if ( keyboard_state[ SDL_GetScancodeFromKey( THROTTLE_UP ) ] && !keyboard_state[ SDL_GetScancodeFromKey( THROTTLE_DOWN ) ] )
    controls_local.throttle = PLANE_THROTTLE::THROTTLE_INCREASE;
  else if ( keyboard_state[ SDL_GetScancodeFromKey( THROTTLE_DOWN ) ] && !keyboard_state[ SDL_GetScancodeFromKey( THROTTLE_UP ) ] )
    controls_local.throttle = PLANE_THROTTLE::THROTTLE_DECREASE;
  else
    controls_local.throttle = PLANE_THROTTLE::THROTTLE_IDLE;


  if ( keyboard_state[ SDL_GetScancodeFromKey( TURN_LEFT ) ] && !keyboard_state[ SDL_GetScancodeFromKey( TURN_RIGHT ) ] )
    controls_local.pitch = PLANE_PITCH::PITCH_LEFT;
  else if ( keyboard_state[ SDL_GetScancodeFromKey( TURN_RIGHT ) ] && !keyboard_state[ SDL_GetScancodeFromKey( TURN_LEFT ) ] )
    controls_local.pitch = PLANE_PITCH::PITCH_RIGHT;
  else
    controls_local.pitch = PLANE_PITCH::PITCH_IDLE;


  // SHOOT
  if ( keyboard_state[ SDL_GetScancodeFromKey( FIRE ) ] )
    controls_local.fire = true;
  else
    controls_local.fire = false;


  // EJECT
  if ( keyboard_state[ SDL_GetScancodeFromKey( JUMP ) ] )
    controls_local.jump = true;
  else
    controls_local.jump = false;
}


void collect_opponent_input()
{
  controls_opponent.throttle = opponent_data.throttle;
  controls_opponent.pitch    = opponent_data.pitch;
}


// Process host input
void process_local_controls( Plane &planebuf, Controls &controls )
{
  if ( controls.throttle == PLANE_THROTTLE::THROTTLE_INCREASE )
    planebuf.input->Accelerate();
  else if ( controls.throttle == PLANE_THROTTLE::THROTTLE_DECREASE )
    planebuf.input->Decelerate();


  if ( controls.pitch == PLANE_PITCH::PITCH_LEFT )
    planebuf.input->TurnLeft();
  else if ( controls.pitch == PLANE_PITCH::PITCH_RIGHT )
    planebuf.input->TurnRight();
  else
    planebuf.input->TurnIdle();

  if ( controls.fire )
    planebuf.input->Shoot();

  if ( controls.jump )
    planebuf.input->Jump();
  else
    planebuf.pilot->ChuteUnlock();
}
