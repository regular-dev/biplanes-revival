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

#include <include/structures.h>
#include <include/variables.h>

#include <cmath>


Plane::Plane( bool construct_type )
  : type{construct_type}
{
  fire_cooldown = Timer( sizes.plane_fire_cooldown_time );
  pitch_cooldown = Timer( sizes.plane_pitch_cooldown_time );
  dead_cooldown = Timer( sizes.plane_dead_cooldown_time );
  protection = Timer( sizes.plane_spawn_protection_time );

  smk_anim = Timer( sizes.smk_frame_time );
  smk_period = Timer( sizes.smk_anim_period );
  fire_anim = Timer( sizes.fire_frame_time );
  expl_anim = Timer( sizes.expl_frame_time );

  Respawn();
}

// INPUT
Plane::Input::Input( Plane* parent_plane )
  : plane{parent_plane}
{
}


// Input Accelerate
void Plane::Input::Accelerate()
{
  plane->Accelerate();
}

// Input Decelerate
void Plane::Input::Decelerate()
{
  plane->Decelerate();
}

// Input Turn Left
void Plane::Input::TurnLeft()
{
  if ( plane->jump )
  else if ( plane->type == (int) srv_or_cli )
    plane->pilot.Move( PLANE_PITCH::PITCH_LEFT );
    plane->Turn( PLANE_PITCH::PITCH_LEFT );
}

// Input Turn Right
void Plane::Input::TurnRight()
{
  if ( plane->jump )
  else if ( plane->type == (int) srv_or_cli )
    plane->pilot.Move( PLANE_PITCH::PITCH_RIGHT );
    plane->Turn( PLANE_PITCH::PITCH_RIGHT );
}

// Input Turn Idle
void Plane::Input::TurnIdle()
{
  if ( plane->jump )
    plane->pilot.MoveIdle();
}

// Input Shoot
void Plane::Input::Shoot()
{
  plane->Shoot();
}

// Input Jump
void Plane::Input::Jump()
{
  if ( plane->jump )
    plane->pilot.OpenChute();
  else
    plane->Jump();
}

void Plane::InitTimers()
{
  fire_cooldown.SetNewTimeout( sizes.plane_fire_cooldown_time );
  pitch_cooldown.SetNewTimeout( sizes.plane_pitch_cooldown_time );

  dead_cooldown.SetNewTimeout( sizes.plane_dead_cooldown_time );
  protection.SetNewTimeout( sizes.plane_spawn_protection_time );

  smk_anim.SetNewTimeout( sizes.smk_frame_time );
  smk_period.SetNewTimeout( sizes.smk_anim_period );
  fire_anim.SetNewTimeout( sizes.fire_frame_time );
  expl_anim.SetNewTimeout( sizes.expl_frame_time );

  pilot.InitTimers();
}


// PLANE UPDATE
void Plane::Update()
{
  SpeedUpdate();
  CoordinatesUpdate();
  CollisionsUpdate();
  AbandonedUpdate();

  if ( jump )
    pilot.Update();

  HitboxUpdate();

  pitch_cooldown.Update();
  fire_cooldown.Update();

  dead_cooldown.Update();
  protection.Update();

  smk_anim.Update();
  smk_period.Update();
  fire_anim.Update();
  expl_anim.Update();
}

// ACCELERATE
void Plane::Accelerate()
{
  if ( dead || jump )
    return;

  if ( onground )
  {
    if ( !takeoff )
      TakeOffStart();

    speed += sizes.plane_incr_spd * 0.85f * deltaTime;
  }
  else
  {
    if ( dir != 0.0f )
    {
      if ( dir == 22.5f || dir == 337.5f )
        speed += sizes.plane_incr_spd * 0.25f * deltaTime;
      else if ( dir == 45.0f || dir == 315.0f )
        speed += sizes.plane_incr_spd * 0.5f * deltaTime;
      else
        speed += sizes.plane_incr_spd * 0.75f * deltaTime;

      if ( speed > max_speed_var )
        speed = max_speed_var;
    }
  }
}

// DECELERATE
void Plane::Decelerate()
{
  if ( dead || jump )
    return;

  if ( takeoff )
    speed -= sizes.plane_incr_spd * 0.75f * deltaTime;
  else
  {
    speed -= sizes.plane_incr_spd * 0.5f * deltaTime;

    if ( speed < 0.0f )
      speed = 0.0f;

    if ( max_speed_var > sizes.plane_max_speed_def )
    {
      max_speed_var -= sizes.plane_incr_spd * 0.5f * deltaTime;

      if ( max_speed_var < sizes.plane_max_speed_def )
        max_speed_var = sizes.plane_max_speed_def;
    }
  }
}

// TURN
void Plane::Turn( unsigned char input_dir )
{
  if ( dead || onground )
    return;

  if ( pitch_cooldown.isReady() )
  {
    pitch_cooldown.Start();

    float turn_dir = input_dir == PLANE_PITCH::PITCH_LEFT ? -1.0f : 1.0f;
    dir += turn_dir * sizes.plane_incr_rot;

    if ( dir < 0.0f )
      dir += 360.0f;
    else if ( dir >= 360.0f )
      dir -= 360.0f;
  }
}

// SHOOT
void Plane::Shoot()
{
  if ( jump || dead || onground || !protection.isReady() )
    return;

  if ( fire_cooldown->isReady() || type != (int) srv_or_cli )
  {
    if ( type == (int) srv_or_cli )
    {
      if ( opponent_connected && !game_finished )
        stats_recent.shots++;

      event_push( (unsigned char) EVENTS::SHOOT );
    }

    fire_cooldown.Start();
    playSound( sounds.shoot, -1, false );

    bullets.SpawnBullet( x, y, dir, type );
  }
}

// JUMP
void Plane::Jump()
{
  if ( dead || onground || !protection.isReady() )
    return;

  jump = true;
  pilot.Bail( x, y, type == PLANE_TYPE::RED ? dir + 90 : dir - 90 );

  if ( type != (int) srv_or_cli )
    pilot.ChuteUnlock();
}


// UPDATE SPEED
void Plane::SpeedUpdate()
{
  if ( onground )
  {
    TakeOffUpdate();
    return;
  }

  // Decrease vertical speed
  if ( dir <= 70 || dir >= 290 )
  {
    if ( dir == 0 )                         // 90 climb
    {
      speed -= sizes.plane_incr_spd * 0.225f * deltaTime;
      max_speed_var = speed;
    }
    else if ( dir <= 25 || dir >= 330 )     // 75 climb
    {
      speed -= sizes.plane_incr_spd * 0.100f * deltaTime;
      max_speed_var = speed;
    }
    else if ( dir <= 50 || dir >= 310 )     // 45 climb
    {
      speed -= sizes.plane_incr_spd * 0.065f * deltaTime;
      max_speed_var = speed;
    }
    else                                    // 30 climb
    {
      speed -= sizes.plane_incr_spd * 0.020f * deltaTime;
      max_speed_var = speed;
    }

    if ( max_speed_var < sizes.plane_max_speed_def )
      max_speed_var = sizes.plane_max_speed_def;

    if ( speed < 0.0f )
      speed = 0.0f;
  }


  // Increase vertical speed
  else if ( dir > 113 && dir < 246 )
  {
    if ( speed < sizes.plane_max_speed_acc )
    {
      speed += sizes.plane_incr_spd * 0.2 * deltaTime;

      if ( speed > max_speed_var )
      {
        max_speed_var = speed;

        if ( max_speed_var > sizes.plane_max_speed_acc )
        {
          max_speed_var = sizes.plane_max_speed_acc;
          speed = max_speed_var;
        }
      }
    }
    else
    {
      speed = sizes.plane_max_speed_acc;
      max_speed_var = speed;
    }
  }
}

// UPDATE COORDINATES
void Plane::CoordinatesUpdate()
{
  if ( dead )
  {
    if ( !jump && dead_cooldown.isReady() && type == (int) srv_or_cli )
    {
      Respawn();
      ResetSpawnProtection();

      event_push( (unsigned char) EVENTS::PLANE_RESP );
    }

    return;
  }
  else if ( onground && !takeoff )
    return;

  // Change coordinates
  x += speed * sin( dir * PI / 180.0f ) * deltaTime;

  if ( !onground )
  {
    y -= speed * cos( dir * PI / 180.0f ) * deltaTime;

    // Gravity
    if ( speed < max_speed_var )
      y += ( max_speed_var - speed ) * deltaTime;
  }

  // Screen borders teleport
  if ( x > sizes.screen_width )
    x = 0.0f;
  else if ( x < 0.0f )
    x = sizes.screen_width;
  if ( y < 0.0f )
    y = 0.0f;
}

// UPDATE COLLISIONS
void Plane::CollisionsUpdate()
{
  if ( dead || ( onground && !takeoff ) )
    return;

  if ( type != (int) srv_or_cli )
    return;

  // Obstacle collisions
  if  ( y > sizes.barn_y_collision &&
        x > sizes.barn_x_collision &&
        x < sizes.barn_x_collision + sizes.barn_sizex )
  {
    // Crash on barn
    Crash();
    event_push( (unsigned char) EVENTS::PLANE_DEATH );
  }
  else if ( !onground && y > sizes.ground_y_collision )
  {
    // Kiss ground
    Crash();
    event_push( (unsigned char) EVENTS::PLANE_DEATH );
  }

  if ( jump )
    return;

  for ( Cloud &cloud : clouds )
  {
    if ( (int) srv_or_cli == type )
    {
      if ( cloud.isHit( x, y ) )
        cloud.setTransparent();
      else
        cloud.setOpaque();
    }
  }
}

// UPDATE ABANDONED FLIGHT
void Plane::AbandonedUpdate()
{
  if ( dead || !jump || dir == 180.0f )
    return;

  if ( speed > sizes.plane_max_speed_def / 2 )
    speed -= sizes.plane_incr_spd * 0.2 * deltaTime;

  if ( pitch_cooldown.isReady() )
  {
    pitch_cooldown.Start();
    if ( type == PLANE_TYPE::RED )
      dir += dir < 180.0f ? sizes.plane_incr_rot : -sizes.plane_incr_rot;
    else
      dir += dir < 180.0f ? sizes.plane_incr_rot : -sizes.plane_incr_rot;
  }
}

// UPDATE ANIMATIONS
void Plane::AnimationsUpdate()
{
  if ( !dead )
  {
    textures.destrect.x = x - sizes.plane_sizex / 2;
    textures.destrect.y = y - sizes.plane_sizey / 2;
    textures.destrect.w = sizes.plane_sizex;
    textures.destrect.h = sizes.plane_sizey;

    if ( type == PLANE_TYPE::BLUE )
    {
      if ( protection.isReady() )
        SDL_SetTextureAlphaMod( textures.texture_biplane_b, 255 );
      else
        SDL_SetTextureAlphaMod( textures.texture_biplane_b, 127 );

      SDL_RenderCopyEx( gRenderer,
                        textures.texture_biplane_b,
                        NULL,
                        &textures.destrect,
                        dir - 90.0,
                        NULL,
                        SDL_FLIP_NONE );
    }
    else
    {
      if ( protection.isReady() )
        SDL_SetTextureAlphaMod( textures.texture_biplane_r, 255 );
      else
        SDL_SetTextureAlphaMod( textures.texture_biplane_r, 127 );

      SDL_RenderCopyEx( gRenderer,
                        textures.texture_biplane_r,
                        NULL,
                        &textures.destrect,
                        dir + 90.0,
                        NULL,
                        SDL_FLIP_NONE );
    }

    if ( show_hitboxes )
    {
      SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
      SDL_RenderDrawRect( gRenderer, &hitbox );

      SDL_Rect plane_center;
      plane_center.x = x - sizes.plane_sizex * 0.05f;
      plane_center.y = y - sizes.plane_sizey * 0.05f;
      plane_center.w = sizes.plane_sizex * 0.1f;
      plane_center.h = sizes.plane_sizey * 0.1f;
      SDL_RenderDrawRect( gRenderer, &plane_center );
    }
  }

  if ( jump )
    pilot.AnimationsUpdate();

  if ( !dead )
  {
    SmokeUpdate();
    FireUpdate();
  }
  ExplosionUpdate();
}

// RESET ANIMATIONS
void Plane::AnimationsReset()
{
  // FRAME APPEARANCE ORDER: { 4, 3, 2, 1, 5 }
  smk_frame[0] = -3;
  smk_frame[1] = -2;
  smk_frame[2] = -1;
  smk_frame[3] = 0;
  smk_frame[4] = -4;
  smk_destrect[0] = {};
  smk_destrect[1] = {};
  smk_destrect[2] = {};
  smk_destrect[3] = {};
  smk_destrect[4] = {};

  smk_anim.Stop();
  smk_period.Stop();
  smk_rect = 4;

  fire_anim.Stop();
  fire_frame = 0;

  expl_anim.Stop();
  expl_frame = 0;

  pilot.AnimationsReset();
}

// UPDATE SMOKE
void Plane::SmokeUpdate()
{
  if ( hp > 1 )
    return;

  if ( smk_anim.isReady() )
  {
    smk_anim.Start();
    for ( unsigned int i = 0; i < 5; i++ )
    {
      smk_frame[i]++;
      if ( smk_frame[i] > 5 )
        smk_frame[i] = 5;
    }

    if ( smk_period.isReady() )
    {
      smk_period.Start();

      smk_frame[0] = -3;
      smk_frame[1] = -2;
      smk_frame[2] = -1;
      smk_frame[3] = 0;
      smk_frame[4] = -4;
    }

    if ( smk_rect == 0 )
      smk_rect = 4;
    else
      smk_rect--;

    smk_destrect[smk_rect].x = x - sizes.smk_sizex / 2;
    smk_destrect[smk_rect].y = y - sizes.smk_sizey / 2;
    smk_destrect[smk_rect].w = sizes.smk_sizex;
    smk_destrect[smk_rect].h = sizes.smk_sizey;
  }

  for ( unsigned int i = 0; i < 5; i++ )
  {
    if ( smk_frame[i] >= 0 )
      SDL_RenderCopy( gRenderer,
                      textures.anim_smk,
                      &textures.anim_smk_rect[ smk_frame[i] ],
                      &smk_destrect[i] );
  }
}

// UPDATE FIRE
void Plane::FireUpdate()
{
  if ( hp > 0 )
    return;

  if ( fire_anim.isReady() )
  {
    fire_anim.Start();
    fire_frame++;
    if ( fire_frame > 2 )
      fire_frame = 0;
  }

  textures.destrect.x = x - sizes.smk_sizex / 2;
  textures.destrect.y = y - sizes.smk_sizey / 2;
  textures.destrect.w = sizes.smk_sizex;
  textures.destrect.h = sizes.smk_sizey;

  if ( type == PLANE_TYPE::BLUE )
    SDL_RenderCopyEx( gRenderer,
                      textures.anim_fire,
                      &textures.anim_fire_rect[fire_frame],
                      &textures.destrect,
                      dir + 180.0f,
                      NULL,
                      SDL_FLIP_NONE );
  else
    SDL_RenderCopyEx( gRenderer,
                      textures.anim_fire,
                      &textures.anim_fire_rect[fire_frame],
                      &textures.destrect,
                      dir -90.0f,
                      NULL,
                      SDL_FLIP_NONE );
}

// UPDATE EXPLOSION
void Plane::ExplosionUpdate()
{
  if ( !dead )
    return;

  if ( expl_frame < 7 )
  {
    textures.destrect.x = x - sizes.expl_sizex / 2;
    textures.destrect.y = y - sizes.expl_sizey / 2;
    textures.destrect.w = sizes.expl_sizex;
    textures.destrect.h = sizes.expl_sizey;
    SDL_RenderCopy( gRenderer,
                    textures.anim_expl,
                    &textures.anim_expl_rect[expl_frame],
                    &textures.destrect );

    if ( expl_anim.isReady() )
    {
      expl_anim.Start();
      expl_frame++;
    }
  }
}

// UPDATE HITBOX
void Plane::HitboxUpdate()
{
  if ( dead )
    return;

  hitbox.x = x - sizes.plane_sizex / 3;
  hitbox.y = y - sizes.plane_sizey / 3;
  hitbox.w = sizes.plane_sizex / 3 * 2;
  hitbox.h = sizes.plane_sizey / 3 * 2;
}

// TAKEOFF UPDATE
void Plane::TakeOffUpdate()
{
  if ( takeoff )
  {
    speed += sizes.plane_incr_spd * 0.75f * deltaTime;

    if ( speed >= sizes.plane_max_speed_def )
      TakeOffFinish();
  }
}

// TAKEOFF START
void Plane::TakeOffStart()
{
  if ( !takeoff )
  {
    takeoff = true;
    dir = type == PLANE_TYPE::RED ? 270.0f : 90.0f;
  }
}

// TAKEOFF FINISH
void Plane::TakeOffFinish()
{
  takeoff = false;
  onground = false;
}

// HIT
void Plane::Hit( bool hit_by )
{
  if ( dead )
    return;

  if ( type == (int) srv_or_cli )
  {
    if ( protection->isReady() )
      event_push( (unsigned char) EVENTS::HIT_PLANE );
    else
      return;
  }
  else if ( opponent_connected && !game_finished )
    stats_recent.plane_hits++;

  if ( HARDCORE_MODE )
    hp = 0;

  if ( hp > 0 )
  {
    playSound( sounds.hit, -1, false );
    hp--;
  }
  else
  {
    if ( opponent_connected && !game_finished )
    {
      if ( type != (int) srv_or_cli )
        stats_recent.plane_kills++;
      else
        stats_recent.deaths++;
    }

    Explode();

    if ( hit_by == PLANE_TYPE::BLUE )
      plane_blue.ScoreChange( 1 );
    else
      plane_red.ScoreChange( 1 );
  }
}

// DIE
void Plane::Explode()
{
  playSound( sounds.expl, -1, false );
  speed = 0.0f;
  dead = true;
  dead_cooldown.Start();
}

// CRASH
void Plane::Crash()
{
  if ( dead )
    return;

  Explode();

  if ( !jump )
  {
    ScoreChange( -1 );

    if ( type == (int) srv_or_cli )
      if ( opponent_connected && !game_finished )
        stats_recent.crashes++;
  }
}

// RESPAWN
void Plane::Respawn()
{
  dead = false;
  hp = 2;
  onground = true;
  takeoff = false;
  speed = 0.0f;
  max_speed_var = sizes.plane_max_speed_def;

  dead_cooldown.Stop();
  fire_cooldown.Stop();
  pitch_cooldown.Stop();
  protection.Stop();

  x = type == PLANE_TYPE::RED ? sizes.plane_red_landx : sizes.plane_blue_landx;
  y = sizes.plane_landy;
  dir = type == PLANE_TYPE::RED ? 292.5f : 67.5f;

  jump = false;

  pilot.Respawn();

  hitbox.x = x - sizes.plane_sizex / 3;
  hitbox.y = y - sizes.plane_sizey / 3;
  hitbox.w = sizes.plane_sizex;
  hitbox.h = sizes.plane_sizey;

  AnimationsReset();
}

void Plane::ResetSpawnProtection()
{
  if ( type == PLANE_TYPE::RED )
  {
    if ( !plane_blue.onground && !plane_blue.dead )
      protection.Start();
    else
      protection.Stop();
  }
  else
  {
    if ( !plane_red.onground && !plane_red.dead )
      protection.Start();
    else
      protection.Stop();
  }
}

void Plane::ResetScore()
{
  score = 0;
}

void Plane::ScoreChange( char deltaScore )
{
  if ( deltaScore < 0 && score < -deltaScore )
    ResetScore();
  else
    score += deltaScore;

  if ( !opponent_connected )
    return;

  if ( score >= sizes.winScore && !game_finished )
  {
    if ( type == (int) srv_or_cli )
    {
      playSound( sounds.victory, -1, false );
      menu.setMessage( MESSAGE_TYPE::GAME_WON );
      stats_recent.wins++;
    }
    else
    {
      playSound( sounds.loss, -1, false );
      menu.setMessage( MESSAGE_TYPE::GAME_LOST );
      stats_recent.losses++;
    }

    game_finished = true;
    stats_update();
  }
}

// CHECK FOR COLLISION
bool Plane::isHit( float check_x, float check_y )
{
  if ( dead )
    return false;

  if (  check_x > hitbox.x &&
        check_x < hitbox.x + hitbox.w &&
        check_y > hitbox.y &&
        check_y < hitbox.y + hitbox.h )
          return true;
  else
    return false;
}

bool Plane::getType()
{
  return type;
}

unsigned char Plane::getScore()
{
  return score;
}

float Plane::getX()
{
  return x;
}

float Plane::getY()
{
  return y;
}


Plane::Pilot::Pilot(  Plane* parentPlane )
  : plane{parentPlane}
{
  fall_anim = Timer( sizes.pilot_fall_frame_time );

  chute_state = CHUTE_STATE::CHUTE_NONE;
  chute_anim = Timer( sizes.chute_frame_time );

  run_anim = Timer( sizes.pilot_run_frame_time );

  angel_anim = Timer( sizes.angel_frame_time );

  pilot_hitbox.x = x - sizes.pilot_sizex / 2;
  pilot_hitbox.y = y - sizes.pilot_sizey / 2;
  pilot_hitbox.w = sizes.pilot_sizex;
  pilot_hitbox.h = sizes.pilot_sizey;
}

// PILOT UPDATE
void Plane::Pilot::Update()
{
  FallUpdate();
  RunUpdate();
  HitboxUpdate();
  ChuteHitboxUpdate();
  DeathUpdate();

  fall_anim.Update();
  chute_anim.Update();
  run_anim.Update();
  angel_anim.Update();
}


void Plane::Pilot::InitTimers()
{
  fall_anim.SetNewTimeout( sizes.pilot_fall_frame_time );
  chute_anim.SetNewTimeout( sizes.chute_frame_time );
  run_anim.SetNewTimeout( sizes.pilot_run_frame_time );
  angel_anim.SetNewTimeout( sizes.angel_frame_time );
}

// PILOT MOVE
void Plane::Pilot::Move( unsigned char input_dir )
{
  float move_dir = input_dir == PLANE_PITCH::PITCH_LEFT ? -1.0f : 1.0f;
  if ( chute )
  {
    chute_state = input_dir;
    x += move_dir * sizes.pilot_chute_speed * deltaTime;
  }
  else if ( run )
  {
    dir = move_dir == PLANE_PITCH::PITCH_LEFT ? 90 : 270;
    x += move_dir * sizes.pilot_run_speed * deltaTime;

    if ( run_anim.isReady() )
    {
      run_anim.Start();

      run_frame++;
      if ( run_frame > 2 )
        run_frame = 0;
    }
  }
}

// PILOT MOVE IDLE
void Plane::Pilot::MoveIdle()
{
  if ( run )
  {
    run_anim.Stop();
    run_frame = 0;

    return;
  }

  if ( chute_state < CHUTE_STATE::CHUTE_DESTROYED )
    chute_state = CHUTE_STATE::CHUTE_IDLE;
}

// PILOT EJECT
void Plane::Pilot::Bail( float plane_x, float plane_y, float bail_dir )
{
  if ( plane->type == (int) srv_or_cli )
  {
    if ( opponent_connected && !game_finished )
      stats_recent.jumps++;

    event_push( (unsigned char) EVENTS::EJECT );
  }

  x = plane_x;
  y = plane_y;
  dir = bail_dir;

  if ( dir < 0 )
    dir += 360;
  else if ( dir >= 360 )
    dir -= 360;

  gravity = sizes.pilot_gravity;
  speed = sizes.pilot_eject_speed;
  vspeed = speed * cos( dir * PI / 180.0f );
}

// PILOT OPEN CHUTE
void Plane::Pilot::OpenChute()
{
  if ( !chute && !run && chute_state == CHUTE_STATE::CHUTE_IDLE )
  {
    if ( plane->type == (int) srv_or_cli )
      event_push( (unsigned char) EVENTS::EJECT );

    chute = true;
    gravity = sizes.pilot_chute_gravity;
  }
}

// UNLOCK CHUTE
void Plane::Pilot::ChuteUnlock()
{
  if ( plane->jump && chute_state == CHUTE_STATE::CHUTE_NONE )
    chute_state = CHUTE_STATE::CHUTE_IDLE;
}


// UPDATE PILOT SPEED
void Plane::Pilot::FallUpdate()
{
  if ( dead || run )
    return;

  if ( speed > 0.0f )
    x += speed * sin( dir * PI / 180.0f ) * deltaTime;

  if ( chute )
  {
    playSound( sounds.chute, plane->getType(), true );

    if ( vspeed > 0.0f )
      vspeed = -gravity * 0.4f;
    else if ( vspeed < -sizes.screen_height * 0.08f )
    {
      vspeed += sizes.screen_height * 0.5f * deltaTime; // 0.375 ?

      if ( vspeed > -sizes.screen_height * 0.08f )
        vspeed = -sizes.screen_height * 0.08f;
    }
    else if ( vspeed > -sizes.screen_height * 0.08f )
      vspeed -= sizes.screen_height * 0.5f * deltaTime;

    if ( speed > 0.0f )
      speed -= speed * 2.0f * deltaTime;
  }
  else
  {
    playSound( sounds.fall, plane->getType(), true );

    if ( vspeed > 0.0f )
    {
      gravity += gravity * 2.0f * deltaTime;
      vspeed -= gravity * deltaTime;

      if ( vspeed < 0.0f )
        gravity = sizes.screen_height * 0.24f; //!!
    }
    else
    {
      gravity += gravity * deltaTime;
      vspeed -= gravity * deltaTime;
    }
  }

  y -= vspeed * deltaTime;


  if ( speed > sizes.screen_width * 0.008f )
    speed -= speed * 1.0f * deltaTime;
  else
    speed = 0.0f;

  if ( speed < 0.0f )
    speed = 0.0f;

  if ( x < 0.0f )
    x = sizes.screen_width;
  else if ( x > sizes.screen_width )
    x = 0.0f;

  HitGroundCheck();

  if ( plane->getType() != (int) srv_or_cli )
    return;

  for ( Cloud &cloud : clouds )
  {
    if ( cloud.isHit( x, y ) )
      cloud.setTransparent();
    else
      cloud.setOpaque();
  }
}

// UPDATE PILOT RUN
void Plane::Pilot::RunUpdate()
{
  if ( !run || dead )
    return;

  if ( x < 0.0f )
    x = sizes.screen_width;
  else if ( x > sizes.screen_width )
    x = 0.0f;

  if ( plane->getType() != (int) srv_or_cli )
    return;

  if  ( x > sizes.barn_x_pilot_left_collision &&
        x < sizes.barn_x_pilot_right_collision )
  {
    if ( opponent_connected && !game_finished )
      stats_recent.rescues++;

    Rescue();
    event_push( (unsigned char) EVENTS::PLANE_RESP );
  }
}

// UPDATE PILOT DEATH
void Plane::Pilot::DeathUpdate()
{
  if ( !dead )
    return;

  if ( angel_anim.isReady() )
  {
    angel_anim.Start();

    if ( angel_frame == 3 )
    {
      if ( plane->getType() == (int) srv_or_cli )
      {
        plane->Respawn();
        plane->ResetSpawnProtection();

        event_push( (unsigned char) EVENTS::PLANE_RESP );
      }
      else
        return;
    }
    else
      angel_frame++;

    if  ( angel_loop < 6 &&
          angel_frame > 2 )
    {
      angel_frame = 0;
      angel_loop++;
    }
    else if ( angel_loop == 6 &&
              angel_frame == 2 )
      angel_frame = 3;
  }

  y -= sizes.angel_ascent_speed * deltaTime;
}

// UPDATE ANIMATIONS
void Plane::Pilot::AnimationsUpdate()
{
  if ( dead )
  {
    DeathAnimUpdate();
    return;
  }

  if ( run )
    RunAnimUpdate();
  else
  {
    if ( chute )
      ChuteAnimUpdate();
    else
      FallAnimUpdate();
  }

  if ( show_hitboxes )
  {
    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &pilot_hitbox );
  }
}

// RESET ANIMATIONS
void Plane::Pilot::AnimationsReset()
{
  fall_anim.Stop();
  fall_frame = 0;

  chute_anim.Stop();
  chute_state = CHUTE_STATE::CHUTE_NONE;

  run_anim.Stop();
  run_frame = 0;

  angel_anim.Stop();
  angel_frame = 0;
  angel_loop = 0;
}

// UPDATE PILOT FALL ANIMATION
void Plane::Pilot::FallAnimUpdate()
{
  if ( fall_anim.isReady() )
  {
    fall_anim.Start();
    fall_frame = !fall_frame;
  }

  textures.destrect.x = x - sizes.pilot_sizex / 2;
  textures.destrect.y = y - sizes.pilot_sizey / 2;
  textures.destrect.w = sizes.pilot_sizex;
  textures.destrect.h = sizes.pilot_sizey;
  if ( plane->type == PLANE_TYPE::RED )
    SDL_RenderCopy( gRenderer,
                    textures.anim_pilot_fall_r,
                    &textures.anim_pilot_fall_rect[fall_frame],
                    &textures.destrect );
  else
    SDL_RenderCopy( gRenderer,
                    textures.anim_pilot_fall_b,
                    &textures.anim_pilot_fall_rect[fall_frame],
                    &textures.destrect );
}

// UPDATE PILOT CHUTE ANIMATION
void Plane::Pilot::ChuteAnimUpdate()
{
  if ( chute_state < CHUTE_STATE::CHUTE_DESTROYED )
  {
    textures.destrect.x = x - sizes.chute_sizex / 2;
    textures.destrect.y = y - sizes.chute_sizey * 1.375f;
    textures.destrect.w = sizes.chute_sizex;
    textures.destrect.h = sizes.chute_sizey;
    SDL_RenderCopy( gRenderer,
                    textures.anim_chute,
                    &textures.anim_chute_rect[chute_state],
                    &textures.destrect );
    if ( chute_anim.isReady() )
    {
      chute_anim.Start();
      fall_frame = !fall_frame;
    }

    textures.destrect.x = x - sizes.pilot_sizex / 2;
    textures.destrect.y = y - sizes.pilot_sizey / 2;
    textures.destrect.w = sizes.pilot_sizex;
    textures.destrect.h = sizes.pilot_sizey;
    if ( plane->type == PLANE_TYPE::RED )
      SDL_RenderCopy( gRenderer,
                      textures.anim_pilot_fall_r,
                      &textures.anim_pilot_fall_rect[fall_frame],
                      &textures.destrect );
    else
      SDL_RenderCopy( gRenderer,
                      textures.anim_pilot_fall_b,
                      &textures.anim_pilot_fall_rect[fall_frame],
                      &textures.destrect );

    if ( show_hitboxes )
    {
      SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
      SDL_RenderDrawRect( gRenderer, &chute_hitbox );
    }
  }
}

// UPDATE PILOT RUN ANIMATION
void Plane::Pilot::RunAnimUpdate()
{
  textures.destrect.x = x - sizes.pilot_sizex / 2;
  textures.destrect.y = y - sizes.pilot_sizey / 2;
  textures.destrect.w = sizes.pilot_sizex;
  textures.destrect.h = sizes.pilot_sizey;

  if ( dir == 270 )
    if ( plane->type == PLANE_TYPE::RED )
      SDL_RenderCopy( gRenderer,
                    textures.anim_pilot_run_r,
                    &textures.anim_pilot_run_rect[run_frame],
                    &textures.destrect );
    else
      SDL_RenderCopy( gRenderer,
                    textures.anim_pilot_run_b,
                    &textures.anim_pilot_run_rect[run_frame],
                    &textures.destrect );
  else
    if ( plane->type == PLANE_TYPE::RED )
      SDL_RenderCopyEx( gRenderer,
                    textures.anim_pilot_run_r,
                    &textures.anim_pilot_run_rect[run_frame],
                    &textures.destrect,
                    NULL,
                    NULL,
                    SDL_FLIP_HORIZONTAL );
    else
      SDL_RenderCopyEx( gRenderer,
                    textures.anim_pilot_run_b,
                    &textures.anim_pilot_run_rect[run_frame],
                    &textures.destrect,
                    NULL,
                    NULL,
                    SDL_FLIP_HORIZONTAL );
}

// UPDATE PILOT DEATH ANIMATION
void Plane::Pilot::DeathAnimUpdate()
{
  textures.destrect.x = x - sizes.angel_sizex / 2;
  textures.destrect.y = y - sizes.angel_sizey / 2;
  textures.destrect.w = sizes.angel_sizex;
  textures.destrect.h = sizes.angel_sizey;
  SDL_RenderCopy( gRenderer,
                  textures.anim_pilot_angel,
                  &textures.anim_pilot_angel_rect[angel_frame],
                  &textures.destrect );

}

// PILOT HITBOX UPDATE
void Plane::Pilot::HitboxUpdate()
{
  if ( dead )
    return;
  pilot_hitbox.x = x - sizes.pilot_sizex / 2;
  pilot_hitbox.y = y - sizes.pilot_sizey / 2;
  pilot_hitbox.w = sizes.pilot_sizex;
  pilot_hitbox.h = sizes.pilot_sizey;
}

// CHUTE HITBOX UPDATE
void Plane::Pilot::ChuteHitboxUpdate()
{
  if ( dead || !chute )
    return;

  chute_hitbox.x = x - sizes.chute_sizex / 2;
  chute_hitbox.y = y - sizes.chute_sizey * 1.375f;
  chute_hitbox.w = sizes.chute_sizex;
  chute_hitbox.h = sizes.chute_sizey;
}

// CHUTE CHECK COLLISION
bool Plane::Pilot::ChuteisHit( float check_x, float check_y )
{
  if ( !chute )
    return false;

  if (  check_x > chute_hitbox.x &&
        check_x < chute_hitbox.x + chute_hitbox.w &&
        check_y > chute_hitbox.y &&
        check_y < chute_hitbox.y + chute_hitbox.h )
          return true;
  else
    return false;
}

float Plane::Pilot::getX()
{
  return x;
}

float Plane::Pilot::getY()
{
  return y;
}

void Plane::Pilot::setX( float new_x )
{
  if ( plane->jump )
    x = new_x;
}

void Plane::Pilot::setY( float new_y )
{
  if ( plane->jump )
    y = new_y;
}


// PILOT DESTROY CHUTE
void Plane::Pilot::ChuteHit()
{
  if ( plane->type != (int) srv_or_cli )
    if ( opponent_connected && !game_finished )
      stats_recent.chute_hits++;

  playSound( sounds.hitChute, -1, false );
  chute_state = CHUTE_STATE::CHUTE_DESTROYED;
  chute = false;
}

// PILOT DEATH
void Plane::Pilot::Death()
{
  playSound( sounds.dead, -1, false );

  run = false;
  chute = false;
  dead = true;

  dir = 0;
  speed = 0.0f;
  vspeed = 0.0f;
  gravity = 0.0f;
  chute_state = CHUTE_STATE::CHUTE_NONE;
}

// PILOT KILL
void Plane::Pilot::Kill( bool killed_by )
{
  if ( plane->type != (int) srv_or_cli )
    if ( opponent_connected && !game_finished )
      stats_recent.pilot_hits++;

  Death();
  if ( killed_by == PLANE_TYPE::BLUE )
    plane_blue.ScoreChange( 2 );
  else
    plane_red.ScoreChange( 2 );
}

// PILOT HIT GROUND
void Plane::Pilot::HitGroundCheck()
{
  if ( y > sizes.ground_y_pilot_collision )
  {
    if ( plane->getType() != (int) srv_or_cli )
      return;

    // SURVIVE LANDING
    if ( vspeed >= -sizes.pilot_chute_gravity )
    {
      FallSurvive();
      event_push( (unsigned char) EVENTS::PILOT_LAND );
    }
    else
    {
      if ( plane->getType() == (int) srv_or_cli )
        if ( opponent_connected && !game_finished )
          stats_recent.falls++;

      Death();
      plane->ScoreChange( -1 );
      event_push( (unsigned char) EVENTS::PILOT_DEATH );
    }
  }
}

// PILOT SURVIVE FALL
void Plane::Pilot::FallSurvive()
{
  y = sizes.ground_y_pilot_collision;

  run = true;
  chute = false;

  speed = 0.0f;
  vspeed = 0.0f;
  gravity = 0.0f;
  chute_state = CHUTE_STATE::CHUTE_IDLE;
}

// PILOT RESCUE
void Plane::Pilot::Rescue()
{
  playSound( sounds.rescue, plane->type, false );

  plane->Respawn();
  plane->ResetSpawnProtection();
}

// PILOT RESPAWN
void Plane::Pilot::Respawn()
{
  run = false;
  chute = false;
  dead = false;

  dir = 0;
  gravity = 0.0f;
  speed = 0.0f;
  vspeed = 0.0f;

  AnimationsReset();
}


bool Plane::Pilot::isDead() const
{
  return dead;
}

// CHECK PILOT COLLISION
bool Plane::Pilot::isHit( float check_x, float check_y )
{
  if ( dead || !plane->jump )
    return false;

  if (  check_x > pilot_hitbox.x &&
        check_x < pilot_hitbox.x + pilot_hitbox.w &&
        check_y > pilot_hitbox.y &&
        check_y < pilot_hitbox.y + pilot_hitbox.h )
          return true;
  else
    return false;
}

// Get coordinates for sending
Plane_Data Plane::getData()
{
  Plane_Data data_buf;
  data_buf.x = x;
  data_buf.y = y;
  data_buf.dir = dir;
  data_buf.pilot_x = pilot.getX();
  data_buf.pilot_y = pilot.getY();

  return data_buf;
}

bool Plane::isDead() const
{
  return ( pilot.isDead() || ( !jump && dead ) );
}

bool Plane::canShoot() const
{
  return !onground && protection.isReady();
}

bool Plane::canJump() const
{
  return  ( !jump && !onground && protection.isReady() ) ||
          ( jump && !pilot.run && !pilot.chute && pilot.chute_state != CHUTE_STATE::CHUTE_DESTROYED );
}

void Plane::setCoords( Plane_Data data )
{
  x = data.x;
  y = data.y;
}

void Plane::setDir( float new_dir )
{
  dir = new_dir;
}

bool Plane::isDead()
{
  return ( pilot->isDead() || ( !jump && dead ) );
}
