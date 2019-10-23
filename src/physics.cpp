#include "../include/physics.h"
#include "../include/variables.h"
#include <math.h>


// Respawn plane
void plane_respawn( Plane &planebuf )
{
  planebuf.dead_cooldown = 0;
  planebuf.dead = 0;
  planebuf.hp = 2;
  planebuf.landed = 1;
  planebuf.takeoff = 0;
  planebuf.speed = 0;
  planebuf.max_speed_var = sizes.plane_max_speed_def;

  planebuf.pitch_cooldown = 0;
  planebuf.fire = 0;
  planebuf.fire_cooldown = 0;

  planebuf.x = planebuf.type? sizes.plane2_landx : sizes.plane1_landx;
  planebuf.y = sizes.plane_landy;
  planebuf.dir = planebuf.type? 292.5 : 67.5;

  planebuf.jump = 0;
  planebuf.pilot_dead = 0;
  planebuf.pilot_gravity = sizes.screen_height * 0.2;
  planebuf.pilot_run = 0;
  planebuf.pilot_speed = 0;
  planebuf.pilot_vspeed = 0;
  planebuf.pilot_chute = 0;

  planebuf.anim_pilot_fall_frame = 0;
  planebuf.anim_pilot_fall_frame_time = 0;

  planebuf.anim_pilot_run_frame = 0;
  planebuf.anim_pilot_run_frame_time = 0;

  planebuf.anim_pilot_angel_frame = 0;
  planebuf.anim_pilot_angel_frame_time = 0;
  planebuf.anim_pilot_angel_loop = 0;

  planebuf.anim_smk_frame[0] = -3;
  planebuf.anim_smk_frame[1] = -2;
  planebuf.anim_smk_frame[2] = -1;
  planebuf.anim_smk_frame[3] = 0;
  planebuf.anim_smk_frame[4] = -4;
  planebuf.smk_destrect[0] = {};
  planebuf.smk_destrect[1] = {};
  planebuf.smk_destrect[2] = {};
  planebuf.smk_destrect[3] = {};
  planebuf.smk_destrect[4] = {};
  planebuf.anim_smk_frame_time = 0;
  planebuf.anim_smk_period_time = 0;
  planebuf.anim_smk_rect = 4;

  planebuf.anim_fire_frame = 0;
  planebuf.anim_fire_frame_time = 0;

  planebuf.anim_expl_frame = 0;
  planebuf.anim_expl_frame_time = 0;
}


// Plane physics
void plane_physics( Plane &planebuf )
{
  if ( !planebuf.dead )
  {
    if ( !planebuf.landed )
    {
      if ( planebuf.speed > planebuf.max_speed_var )
        planebuf.speed = planebuf.max_speed_var;


      // Decrease vertical speed
      if ( planebuf.dir <= 70 || planebuf.dir >= 290 )
      {
        if ( planebuf.dir == 0 )                                    // 90 climb
        {
          planebuf.speed -= deltaTime * sizes.plane_incr_spd*0.17;
          planebuf.max_speed_var -= deltaTime * sizes.plane_incr_spd * 0.17;
        }
        else if ( planebuf.dir <= 25 || planebuf.dir >= 330 )     // 75 climb
        {
          planebuf.speed -= deltaTime * sizes.plane_incr_spd * 0.1;
          planebuf.max_speed_var -= deltaTime * sizes.plane_incr_spd * 0.1;
        }
        else if ( planebuf.dir <= 50 || planebuf.dir >= 310 )     // 45 climb
        {
          planebuf.speed -= deltaTime * sizes.plane_incr_spd * 0.065;
          planebuf.max_speed_var -= deltaTime * sizes.plane_incr_spd * 0.065;
        }
        else                                                      // 30 climb
        {
          planebuf.speed -= deltaTime * sizes.plane_incr_spd * 0.02;
          planebuf.max_speed_var -= deltaTime * sizes.plane_incr_spd * 0.02;
        }

        if ( planebuf.max_speed_var < sizes.plane_max_speed_def )
          planebuf.max_speed_var = sizes.plane_max_speed_def;

        if ( planebuf.speed < 0 )
          planebuf.speed = 0;
      }


      // Increase vertical speed
      if ( planebuf.dir > 113 && planebuf.dir < 246 )
      {
        if ( planebuf.speed < sizes.plane_max_speed_acc )
        {
          planebuf.speed += deltaTime * sizes.plane_incr_spd * 0.2;
          if ( planebuf.speed > planebuf.max_speed_var )
            planebuf.max_speed_var += deltaTime * sizes.plane_incr_spd * 0.2;

          if ( planebuf.speed > sizes.plane_max_speed_acc )
            planebuf.speed = sizes.plane_max_speed_acc;

          if ( planebuf.max_speed_var > sizes.plane_max_speed_acc )
            planebuf.max_speed_var = sizes.plane_max_speed_acc;
        }
      }


      // Change coordinates
      planebuf.x += deltaTime * planebuf.speed*sin(planebuf.dir*PI/180);
      planebuf.y -= deltaTime * planebuf.speed*cos(planebuf.dir*PI/180);


      // Gravity
      if ( planebuf.speed < planebuf.max_speed_var )
        planebuf.y += deltaTime * ( planebuf.max_speed_var - planebuf.speed );


      // Screen borders teleport
      if ( planebuf.x > sizes.screen_width )
        planebuf.x = 0;
      else if ( planebuf.x < 0 )
        planebuf.x = sizes.screen_width;
      if ( planebuf.y < 0 )
        planebuf.y = 0;
    }
    else if ( planebuf.takeoff )
    {
      planebuf.x += deltaTime * planebuf.speed*sin( planebuf.dir*PI/180 );

      if ( planebuf.speed >= sizes.plane_max_speed_def )
      {
        planebuf.landed = 0;
        planebuf.takeoff = 0;
      }
    }


    // Collisions
    if  ( planebuf.y > sizes.screen_height * 0.788 &&
          planebuf.x > sizes.screen_width * 0.426 &&
          planebuf.x < sizes.screen_width * 0.426 + sizes.barn_sizex )
    {
      playSound( sounds.expl, -1, false );
      planebuf.dead = 1;
      planebuf.speed = 0;

      if ( !planebuf.jump && planebuf.score > 0 )
        planebuf.score--;
    }
    else if ( planebuf.y > sizes.screen_height * 0.875 )
    {
      playSound( sounds.expl, -1, false );
      planebuf.dead = 1;
      planebuf.speed = 0;

      if ( !planebuf.jump && planebuf.score > 0 )
        planebuf.score--;
    }
  }
  else
  {
    // Respawn biplane after explosion
    planebuf.dead_cooldown += deltaTime;
    if ( planebuf.dead_cooldown >= 3.0 && !planebuf.jump )
      plane_respawn( planebuf );
  }


  // Bail out
  if ( planebuf.jump )
  {
    // Plane fall
    if ( !planebuf.dead )
    {
      if ( planebuf.dir != 180 )
      {
        if ( planebuf.speed > sizes.plane_max_speed_def / 2 )
          planebuf.speed -= deltaTime * sizes.plane_incr_spd * 0.2;

        if ( planebuf.pitch_cooldown >= 0.375 )
          planebuf.pitch_cooldown = 0;

        if ( planebuf.pitch_cooldown == 0 )
        {
          if ( planebuf.type )
          {
            if( planebuf.dir == 0 )
              planebuf.dir = 360 - sizes.plane_incr_rot;
            else
              planebuf.dir += planebuf.dir < 180 ? sizes.plane_incr_rot : -sizes.plane_incr_rot;
          }
          else
            planebuf.dir += planebuf.dir < 180 ? sizes.plane_incr_rot : -sizes.plane_incr_rot;
        }
        planebuf.pitch_cooldown += deltaTime;
      }
    }
    else
      planebuf.pitch_cooldown = 0;


    // Pilot fall
    if ( !planebuf.pilot_run && !planebuf.pilot_dead )
    {
      planebuf.pilot_x += deltaTime * planebuf.pilot_speed*sin( planebuf.pilot_dir*PI/180 );

      if ( !planebuf.pilot_chute )
      {
        playSound( sounds.fall, planebuf.type, true );

        if ( planebuf.pilot_vspeed > 0 )
        {
          planebuf.pilot_gravity += deltaTime * planebuf.pilot_gravity * 2;
          planebuf.pilot_vspeed -= deltaTime * planebuf.pilot_gravity;

          if ( planebuf.pilot_vspeed < 0 )
            planebuf.pilot_gravity = sizes.screen_height * 0.24; //!!
        }
        else
        {
          planebuf.pilot_gravity += deltaTime * planebuf.pilot_gravity;
          planebuf.pilot_vspeed -= deltaTime * planebuf.pilot_gravity;
        }
      }
      else    // Parachute
      {
        playSound( sounds.chute, planebuf.type, true );

        if ( planebuf.pilot_vspeed > 0 )
          planebuf.pilot_vspeed = -planebuf.pilot_gravity * 0.4;
        else if ( planebuf.pilot_vspeed < -sizes.screen_height * 0.08 )
        {
          planebuf.pilot_vspeed += deltaTime * sizes.screen_height * 0.5; // 0.375 ?

          if ( planebuf.pilot_vspeed > -sizes.screen_height * 0.08 )
            planebuf.pilot_vspeed = -sizes.screen_height * 0.08;
        }
        else if ( planebuf.pilot_vspeed > -sizes.screen_height * 0.08 )
          planebuf.pilot_vspeed -= deltaTime * sizes.screen_height * 0.5;

        if ( planebuf.pilot_speed > 0 )
          planebuf.pilot_speed -= deltaTime * planebuf.pilot_speed * 2.0;
      }


      planebuf.pilot_y -= deltaTime * planebuf.pilot_vspeed;

      if ( planebuf.pilot_speed > sizes.screen_width * 0.008 )
        planebuf.pilot_speed -= deltaTime * planebuf.pilot_speed * 1.0;
      else
        planebuf.pilot_speed = 0;

      if ( planebuf.pilot_speed < 0 )
        planebuf.pilot_speed = 0;


      // Fall death/survive
      if ( planebuf.pilot_y > sizes.screen_height * 0.8925 )
      {
        if ( planebuf.pilot_vspeed >= -sizes.screen_height * 0.2 )
        {
          planebuf.pilot_y = sizes.screen_height * 0.8925;
          planebuf.pilot_speed = 0;
          planebuf.pilot_run = 1;
          planebuf.pilot_gravity = sizes.screen_height * 0.2;
          planebuf.pilot_chute = 0;
          planebuf.pilot_chute_frame = 4;
        }
        else
        {
          playSound( sounds.dead, -1, false );
          planebuf.pilot_dead = 1;
          planebuf.pilot_speed = 0;
          planebuf.pilot_run = 0;
          planebuf.pilot_gravity = sizes.screen_height * 0.2;
          planebuf.pilot_chute = 0;
          planebuf.pilot_chute_frame = 4;

          if ( planebuf.score > 0 )
            planebuf.score--;
        }
      }
    }
    else if ( !planebuf.pilot_dead )
    {
      if  ( planebuf.pilot_x > sizes.screen_width * 0.44 &&
            planebuf.pilot_x < sizes.screen_width * 0.422 + sizes.barn_sizex )
        plane_respawn( planebuf );
    }
    if ( planebuf.pilot_dead )
    {
      if ( planebuf.anim_pilot_angel_frame_time > 0.138 )
      {
        planebuf.anim_pilot_angel_frame_time = 0;

        if ( planebuf.anim_pilot_angel_frame == 3 )
          plane_respawn( planebuf );
        else
          planebuf.anim_pilot_angel_frame++;

        if  ( planebuf.anim_pilot_angel_loop < 6 &&
              planebuf.anim_pilot_angel_frame > 2 )
        {
          planebuf.anim_pilot_angel_frame = 0;
          planebuf.anim_pilot_angel_loop++;
        }
        else if ( planebuf.anim_pilot_angel_loop == 6 &&
                  planebuf.anim_pilot_angel_frame == 2 )
          planebuf.anim_pilot_angel_frame = 3;
      }
      planebuf.pilot_y -= deltaTime * sizes.screen_height * 0.035;
      planebuf.anim_pilot_angel_frame_time += deltaTime;
    }


    if ( planebuf.pilot_x < 0 )
      planebuf.pilot_x = sizes.screen_width;
    else if ( planebuf.pilot_x > sizes.screen_width )
      planebuf.pilot_x = 0;
  }

  // Plane collisions
  if ( PLANE_COLLISIONS )
  {
    if  ( plane1.x > plane2.x - sizes.plane_sizex / 3 &&
          plane1.x < plane2.x + sizes.plane_sizex / 3 &&
          plane1.y > plane2.y - sizes.plane_sizey / 3 &&
          plane1.y < plane2.y + sizes.plane_sizey / 3 )
    {
      playSound( sounds.expl, -1, false );
      plane1.speed = 0;
      plane1.dead = 1;
      if ( !plane1.jump && plane1.score > 0 )
        plane1.score--;

      plane2.speed = 0;
      plane2.dead = 1;
      if ( !plane2.jump && plane2.score > 0 )
        plane2.score--;
    }
  }
}


// Bullets physics
void bullets_physics()
{
  for ( int i = 0; i < max_bullet_count; i++ )
  {
    if ( bullets[i].alive == 1 )
    {
      bullets[i].x += deltaTime * sizes.bullet_speed*sin( bullets[i].dir*PI/180 );
      bullets[i].y -= deltaTime * sizes.bullet_speed*cos( bullets[i].dir*PI/180 );

      // Bullet pass screen bounds
      if ( bullets[i].x > sizes.screen_width ||
           bullets[i].x < 0 ||
           bullets[i].y < 0 )
      {
        bullets[i].alive = 0;
      }
      else if   ( bullets[i].y > sizes.screen_height * 0.895 ||
                ( bullets[i].y > sizes.screen_height * 0.81 &&
                  bullets[i].x > sizes.screen_width * 0.43 &&
                  bullets[i].x < sizes.screen_width * 0.42 + sizes.barn_sizex ) )
      {
        // Bullet hit ground
        playSound( sounds.hitMiss, -1, false );
        bullets[i].anim_hit_frame = 1;
        bullets[i].alive = 0;
      }
      else if ( !bullets[i].fired_by )  // Plane hit by blue
      {
        if ( !plane2.dead  &&
              bullets[i].x > plane2.x - sizes.plane_sizex / 3 &&
              bullets[i].x < plane2.x + sizes.plane_sizex / 3 &&
              bullets[i].y > plane2.y - sizes.plane_sizey / 3 &&
              bullets[i].y < plane2.y + sizes.plane_sizey / 3 )
        {
          bullets[i].alive = 0;

          if ( plane2.hp > 0 )
          {
            playSound( sounds.hit, -1, false );
            plane2.hp--;
          }
          else if ( plane2.hp == 0 )
          {
            playSound( sounds.expl, -1, false );
            plane2.dead = 1;
            plane1.score++;
          }
        }


        // Chute hit
        if ( !plane2.pilot_dead && plane2.jump && plane2.pilot_chute &&
              bullets[i].x > plane2.pilot_x - sizes.chute_sizex / 2 &&
              bullets[i].x < plane2.pilot_x + sizes.chute_sizex / 2 &&
              bullets[i].y > plane2.pilot_y - sizes.chute_sizey * 1.375 &&
              bullets[i].y < plane2.pilot_y - sizes.chute_sizey * 1.375 + sizes.chute_sizey )
        {
          playSound( sounds.hitChute, -1, false );
          bullets[i].alive = 0;
          plane2.pilot_chute = 0;
        }


        // Pilot hit
        if ( !plane2.pilot_dead && plane2.jump &&
              bullets[i].x > plane2.pilot_x - sizes.pilot_sizex / 2 &&
              bullets[i].x < plane2.pilot_x + sizes.pilot_sizex / 2 &&
              bullets[i].y > plane2.pilot_y - sizes.pilot_sizey / 2 &&
              bullets[i].y < plane2.pilot_y + sizes.pilot_sizey / 2 )
        {
          playSound( sounds.dead, -1, false );
          bullets[i].alive = 0;
          plane2.pilot_dead = 1;
          plane1.score += 2;
        }
      }
      else                              // Plane hit by red
      {
        if ( !plane1.dead &&
              bullets[i].x > plane1.x - sizes.plane_sizex / 3 &&
              bullets[i].x < plane1.x + sizes.plane_sizex / 3 &&
              bullets[i].y > plane1.y - sizes.plane_sizey / 3 &&
              bullets[i].y < plane1.y + sizes.plane_sizey / 3 )
        {
          bullets[i].alive = 0;

          if ( plane1.hp > 0 )
          {
            playSound( sounds.hit, -1, false );
            plane1.hp--;
          }
          else if ( plane1.hp == 0 )
          {
            playSound( sounds.expl, -1, false );
            plane1.dead = 1;
            plane2.score++;
          }
        }


        // Chute hit
        if ( !plane1.pilot_dead && plane1.jump && plane1.pilot_chute &&
              bullets[i].x > plane1.pilot_x - sizes.chute_sizex / 2 &&
              bullets[i].x < plane1.pilot_x + sizes.chute_sizex / 2 &&
              bullets[i].y > plane1.pilot_y - sizes.chute_sizey * 1.375 &&
              bullets[i].y < plane1.pilot_y - sizes.chute_sizey * 1.375 + sizes.chute_sizey )
        {
          playSound( sounds.hitChute, -1, false );
          bullets[i].alive = 0;
          plane1.pilot_chute = 0;
        }


        // Pilot hit
        if ( !plane1.pilot_dead && plane1.jump &&
              bullets[i].x > plane1.pilot_x - sizes.pilot_sizex / 2 &&
              bullets[i].x < plane1.pilot_x + sizes.pilot_sizex / 2 &&
              bullets[i].y > plane1.pilot_y - sizes.pilot_sizey / 2 &&
              bullets[i].y < plane1.pilot_y + sizes.pilot_sizey / 2 )
        {
          playSound( sounds.dead, -1, false );
          bullets[i].alive = 0;
          plane1.pilot_dead = 1;
          plane2.score += 2;
        }
      }
    }
  }
}


// Client sounds
void sounds_client( Plane &planebuf )
{
  if ( !planebuf.dead )
  {
    // Collisions
    if  ( planebuf.y > sizes.screen_height * 0.788 &&
          planebuf.x > sizes.screen_width * 0.426 &&
          planebuf.x < sizes.screen_width * 0.426 + sizes.barn_sizex )
    {
      playSound( sounds.expl, -1, false );
      planebuf.dead = 1;
      planebuf.speed = 0;
    }
    else if ( planebuf.y > sizes.screen_height * 0.875 )
    {
      playSound( sounds.expl, -1, false );
      planebuf.dead = 1;
      planebuf.speed = 0;
    }
  }


  // Bail out
  if ( planebuf.jump )
  {
    // Pilot fall
    if ( !planebuf.pilot_run && !planebuf.pilot_dead )
    {
      if ( !planebuf.pilot_chute )
        playSound( sounds.fall, planebuf.type, true );
      else    // Parachute
        playSound( sounds.chute, planebuf.type, true );


      // Fall death/survive
      if ( planebuf.pilot_y > sizes.screen_height * 0.8925 )
      {
        if ( planebuf.pilot_vspeed < -sizes.screen_height * 0.2 )
          playSound( sounds.dead, -1, false );
      }
    }
  }


  // Plane collisions
  if ( PLANE_COLLISIONS )
  {
    if( plane1.x > plane2.x - sizes.plane_sizex / 3 &&
        plane1.x < plane2.x + sizes.plane_sizex / 3 &&
        plane1.y > plane2.y - sizes.plane_sizey / 3 &&
        plane1.y < plane2.y + sizes.plane_sizey / 3 )
    {
      playSound( sounds.expl, -1, false );
    }
  }


  // Bullets
  for ( int i = 0; i < max_bullet_count; i++ )
  {
    if ( bullets[i].alive == 1 )
    {
      if  ( ( bullets[i].y > sizes.screen_height * 0.895 &&
              bullets[i].y < sizes.screen_height &&
              bullets[i].x < sizes.screen_width &&
              bullets[i].x > 0 ) ||
            ( bullets[i].y > sizes.screen_height * 0.81 &&
              bullets[i].x > sizes.screen_width * 0.43 &&
              bullets[i].x < sizes.screen_width * 0.42 + sizes.barn_sizex ) )
      {
        // Bullet hit ground
        playSound( sounds.hitMiss, -1, false );
      }
      else if ( !bullets[i].fired_by )  // Plane hit by blue
      {
        if ( !plane2.dead  &&
              bullets[i].x > plane2.x - sizes.plane_sizex / 3 &&
              bullets[i].x < plane2.x + sizes.plane_sizex / 3 &&
              bullets[i].y > plane2.y - sizes.plane_sizey / 3 &&
              bullets[i].y < plane2.y + sizes.plane_sizey / 3 )
        {
          if ( plane2.hp <= 0 )
            playSound( sounds.expl, -1, false );
          else
            playSound( sounds.hit, -1, false );
        }


        // Chute hit
        if ( !plane2.pilot_dead && plane2.jump && plane2.pilot_chute &&
              bullets[i].x > plane2.pilot_x - sizes.chute_sizex / 2 &&
              bullets[i].x < plane2.pilot_x + sizes.chute_sizex / 2 &&
              bullets[i].y > plane2.pilot_y - sizes.chute_sizey * 1.375 &&
              bullets[i].y < plane2.pilot_y - sizes.chute_sizey * 1.375 + sizes.chute_sizey )
        {
          playSound( sounds.hitChute, -1, false );
        }


        // Pilot hit
        if ( !plane2.pilot_dead && plane2.jump &&
              bullets[i].x > plane2.pilot_x - sizes.pilot_sizex / 2 &&
              bullets[i].x < plane2.pilot_x + sizes.pilot_sizex / 2 &&
              bullets[i].y > plane2.pilot_y - sizes.pilot_sizey / 2 &&
              bullets[i].y < plane2.pilot_y + sizes.pilot_sizey / 2 )
        {
          playSound( sounds.dead, -1, false );
        }
      }
      else                              // Plane hit by red
      {
        if ( !plane1.dead &&
              bullets[i].x > plane1.x - sizes.plane_sizex / 3 &&
              bullets[i].x < plane1.x + sizes.plane_sizex / 3 &&
              bullets[i].y > plane1.y - sizes.plane_sizey / 3 &&
              bullets[i].y < plane1.y + sizes.plane_sizey / 3 )
        {


          if ( plane1.hp > 0 )
            playSound( sounds.hit, -1, false );
          else if ( plane1.hp == 0 )
            playSound( sounds.expl, -1, false );
        }


        // Chute hit
        if ( !plane1.pilot_dead && plane1.jump && plane1.pilot_chute &&
              bullets[i].x > plane1.pilot_x - sizes.chute_sizex / 2 &&
              bullets[i].x < plane1.pilot_x + sizes.chute_sizex / 2 &&
              bullets[i].y > plane1.pilot_y - sizes.chute_sizey * 1.375 &&
              bullets[i].y < plane1.pilot_y - sizes.chute_sizey * 1.375 + sizes.chute_sizey )
        {
          playSound( sounds.hitChute, -1, false );
        }


        // Pilot hit
        if ( !plane1.pilot_dead && plane1.jump &&
              bullets[i].x > plane1.pilot_x - sizes.pilot_sizex / 2 &&
              bullets[i].x < plane1.pilot_x + sizes.pilot_sizex / 2 &&
              bullets[i].y > plane1.pilot_y - sizes.pilot_sizey / 2 &&
              bullets[i].y < plane1.pilot_y + sizes.pilot_sizey / 2 )
        {
          playSound( sounds.dead, -1, false );
        }
      }
    }
  }
}
