#include "../include/controls.h"
#include "../include/variables.h"


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
  controls_opponent.fire     = opponent_data.fire;
  controls_opponent.jump     = opponent_data.jump;
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
