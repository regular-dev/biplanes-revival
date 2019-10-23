#include "../include/init_vars.h"
#include "../include/variables.h"
#include "../include/utility.h"


// Specify sizes
void init_vars()
{
  sizes.text_sizex = sizes.screen_width / 32;
  sizes.text_sizey = sizes.screen_height / 26;

  sizes.button_sizex = sizes.screen_width - sizes.screen_width * 0.016f;
  sizes.button_sizey = sizes.screen_height * 0.0577f;

  // Plane
  plane_blue.InitTimers();
  plane_red.InitTimers();
  plane_blue.AnimationsReset();
  plane_red.AnimationsReset();
  sizes.ground_y_collision = sizes.screen_height * 0.875f;
  sizes.plane_sizex = sizes.screen_width * 0.0935f;
  sizes.plane_sizey = sizes.screen_height * 0.11538f;

  sizes.plane_max_speed_def = sizes.screen_width * 0.303f;
  sizes.plane_max_speed_acc = sizes.screen_width * 0.43478f;

  sizes.plane_incr_spd = sizes.screen_height * 0.5f;
  sizes.plane_incr_rot = 22.5f;

  sizes.plane_dead_cooldown_time = 3.0f;
  sizes.plane_pitch_cooldown_time = 0.1f;
  sizes.plane_fire_cooldown_time = 0.65f;

  sizes.plane_blue_landx = sizes.screen_width * 0.0625f;
  sizes.plane_red_landx = sizes.screen_width * 0.9375f;
  sizes.plane_landy = sizes.screen_height * 0.8675f;

  // Smoke animation
  sizes.smk_frame_time = 0.1f;
  sizes.smk_anim_period = 1.0f;
  sizes.smk_sizex = sizes.screen_width * 0.05f;
  sizes.smk_sizey = sizes.screen_height * 0.0625f;

  // Fire animation
  sizes.fire_frame_time = 0.075f;

  // Explosion animation
  sizes.expl_sizex = sizes.screen_width * 0.15625f;
  sizes.expl_sizey = sizes.screen_height * 0.1923f;
  sizes.expl_frame_time = 0.075f;


  // Bullet
  sizes.bullet_sizex = sizes.screen_width * 0.0117f;
  sizes.bullet_sizey = sizes.screen_height * 0.0144f;
  sizes.bullet_speed = sizes.screen_width * 0.77f;

  sizes.hit_sizex = sizes.screen_width * 0.035f;
  sizes.hit_sizey = sizes.screen_height * 0.038f;

  sizes.bullet_ground_collision = sizes.screen_height * 0.895f;
  sizes.bullet_hit_frame_time = 0.08f;


  // Pilot
  sizes.ground_y_pilot_collision = sizes.screen_height * 0.8925f;
  sizes.pilot_eject_speed = sizes.screen_height * 0.45f;
  sizes.pilot_gravity = sizes.screen_height * 0.2;
  sizes.pilot_chute_gravity = sizes.screen_height * 0.2f;
  sizes.pilot_chute_speed = sizes.screen_width * 0.04f;
  sizes.pilot_run_speed = sizes.screen_width * 0.1f;
  sizes.pilot_run_frame_time = 0.075f;
  sizes.pilot_fall_frame_time = 0.1f;
  sizes.chute_frame_time = 0.25f;

  sizes.pilot_sizex = sizes.screen_width * 0.0273f;
  sizes.pilot_sizey = sizes.screen_height * 0.03365f;

  // Pilot parachute
  sizes.chute_sizex = sizes.screen_width * 0.078;
  sizes.chute_sizey = sizes.screen_height * 0.057;

  // Pilot death animation
  sizes.angel_frame_time = 0.138f;
  sizes.angel_ascent_speed = sizes.screen_height * 0.035f;
  sizes.angel_sizex = sizes.screen_width * 0.0273f;
  sizes.angel_sizey = sizes.screen_width * 0.0273f;


  // Barn
  sizes.barn_sizex = sizes.screen_width * 0.1367f;
  sizes.barn_sizey = sizes.screen_height * 0.105769f;

  sizes.barn_x_collision = sizes.screen_width * 0.426f;
  sizes.barn_y_collision = sizes.screen_height * 0.788f;

  sizes.barn_x_pilot_left_collision = sizes.screen_width * 0.44f;
  sizes.barn_x_pilot_right_collision = sizes.screen_width * 0.422f + sizes.barn_sizex;

  sizes.barn_x_bullet_collision = sizes.screen_width * 0.42f;
  sizes.barn_y_bullet_collision = sizes.screen_height * 0.81f;


  // Clouds
  if ( clouds.empty() )
  {
    clouds.push_back( Cloud( false ) );
    clouds.push_back( Cloud( true ) );
  }
  sizes.cloud_sizex = sizes.screen_width * 0.2695f;
  sizes.cloud_sizey = sizes.screen_height * 0.1538f;
  sizes.cloud_speed = sizes.screen_width * 0.09765625f;
  sizes.cloud_left_spawn_x = sizes.screen_width * 0.1f;
  sizes.cloud_right_spawn_x = sizes.screen_width * 0.6f;
  sizes.cloud_highest_y = sizes.screen_height * 0.05f;
  sizes.cloud_lowest_y = sizes.screen_height * 0.35;


  // Zeppelin
  sizes.zeppelin_sizex = sizes.screen_width * 0.199f;
  sizes.zeppelin_sizey = sizes.screen_height * 0.13f;
  sizes.zeppelin_speed = sizes.screen_width * 0.014f;

  sizes.zeppelin_spawn_x = sizes.screen_width * 0.5f;
  sizes.zeppelin_highest_y = sizes.screen_height * 0.1f;
  sizes.zeppelin_lowest_y = sizes.screen_height * 0.35f;

  sizes.zeppelin_score_sizex = sizes.screen_width * 0.0195f;
  sizes.zeppelin_score_sizey = sizes.screen_height * 0.0288f;
}


// Load textures
void textures_load()
{
  log_message( "RESOURCES: Loading textures..." );


  SDL_GetRendererOutputSize( gRenderer, &sizes.screen_width, &sizes.screen_height );
  sizes.screen_width = sizes.screen_height * 1.23;

  SDL_SetWindowSize( gWindow, sizes.screen_width, sizes.screen_height );

  textures.main_font = loadTexture( "assets/menu/font.png" );
  for ( int i = 0; i < 95; i++ )
  {
    textures.font_rect[i].w = 8;
    textures.font_rect[i].h = 8;
    textures.font_rect[i].x = (i % 19) * 8;
    textures.font_rect[i].y = int (i / 19) * 8;
  }


  textures.menu_box = loadTexture( "assets/menu/menu_box.png" );
  textures.menu_settings_controls_box = loadTexture( "assets/menu/menu_settings_controls_box.png" );
  textures.menu_help = loadTexture( "assets/menu/menu_help.png" );
  textures.menu_moving_button = loadTexture( "assets/menu/menu_moving_button.png" );
  textures.menu_logo = loadTexture( "assets/menu/menu_logo.png" );


  textures.texture_background = loadTexture( "assets/ingame/ingame_background.png" );
  textures.texture_barn = loadTexture( "assets/ingame/ingame_barn.png" );
  textures.texture_biplane_b = loadTexture( "assets/ingame/biplane/ingame_biplane_b.png" );
  textures.texture_biplane_r = loadTexture( "assets/ingame/biplane/ingame_biplane_r.png" );
  textures.texture_bullet = loadTexture( "assets/ingame/biplane/ingame_bullet.png" );
  textures.texture_cloud = loadTexture( "assets/ingame/ingame_cloud.png" );
  textures.texture_cloud_opaque = loadTexture( "assets/ingame/ingame_cloud_opaque.png" );
  textures.texture_zeppelin = loadTexture( "assets/ingame/ingame_zeppelin.png" );
  textures.font_zeppelin_score = loadTexture( "assets/ingame/font_zeppelin_score.png" );

  textures.anim_smk = loadTexture( "assets/ingame/biplane/ingame_smk.png" );
  textures.anim_smk_rect[0] = { 0, 0, 13, 13 };
  for ( int i = 1; i < 6; i++ )
  {
    textures.anim_smk_rect[i].x = textures.anim_smk_rect[i-1].x + textures.anim_smk_rect[i-1].w;
    textures.anim_smk_rect[i].y = 0;
    textures.anim_smk_rect[i].w = 13;
    textures.anim_smk_rect[i].h = 13;
  }

  textures.anim_fire = loadTexture( "assets/ingame/biplane/ingame_fire.png" );
  textures.anim_fire_rect[0] = { 0, 0, 13, 13 };
  for ( int i = 1; i < 3; i++ )
  {
    textures.anim_fire_rect[i].x = textures.anim_fire_rect[i-1].x + textures.anim_fire_rect[i-1].w;
    textures.anim_fire_rect[i].y = 0;
    textures.anim_fire_rect[i].w = 13;
    textures.anim_fire_rect[i].h = 13;
  }

  textures.anim_expl = loadTexture( "assets/ingame/biplane/ingame_expl.png" );
  textures.anim_expl_rect[0] = { 0, 0, 40, 40 };
  for ( int i = 1; i < 7; i++ )
  {
    textures.anim_expl_rect[i].x = textures.anim_expl_rect[i-1].x + textures.anim_expl_rect[i-1].w;
    textures.anim_expl_rect[i].y = 0;
    textures.anim_expl_rect[i].w = 40;
    textures.anim_expl_rect[i].h = 40;
  }

  textures.anim_hit = loadTexture( "assets/ingame/biplane/ingame_hit.png" );
  textures.anim_hit_rect[0] = { 0, 0, 9, 8 };
  for ( int i = 1; i < 5; i++ )
  {
    textures.anim_hit_rect[i].x = textures.anim_hit_rect[i-1].x + textures.anim_hit_rect[i-1].w;
    textures.anim_hit_rect[i].y = 0;
    textures.anim_hit_rect[i].w = 9;
    textures.anim_hit_rect[i].h = 8;
  }

  textures.anim_chute = loadTexture( "assets/ingame/pilot/ingame_chute.png" );
  textures.anim_chute_rect[0] = { 0, 0, 20, 12 };
  for ( int i = 1; i < 3; i++ )
  {
    textures.anim_chute_rect[i].x = textures.anim_chute_rect[i-1].x + textures.anim_chute_rect[i-1].w;
    textures.anim_chute_rect[i].y = 0;
    textures.anim_chute_rect[i].w = 20;
    textures.anim_chute_rect[i].h = 12;
  }

  textures.anim_pilot_angel = loadTexture( "assets/ingame/pilot/ingame_angel.png" );
  textures.anim_pilot_angel_rect[0] = { 0, 0, 10, 8 };
  for ( int i = 1; i < 4; i++ )
  {
    textures.anim_pilot_angel_rect[i].x = textures.anim_pilot_angel_rect[i-1].x + textures.anim_pilot_angel_rect[i-1].w;
    textures.anim_pilot_angel_rect[i].y = 0;
    textures.anim_pilot_angel_rect[i].w = 10;
    textures.anim_pilot_angel_rect[i].h = 8;
  }

  textures.anim_pilot_fall = loadTexture( "assets/ingame/pilot/ingame_fall.png" );
  textures.anim_pilot_fall_rect[0] = { 0, 0, 7, 7 };
  textures.anim_pilot_fall_rect[1] = { 7, 0, 7, 7 };

  textures.anim_pilot_run = loadTexture( "assets/ingame/pilot/ingame_run.png" );
  textures.anim_pilot_run_rect[0] = { 0, 0, 7, 7 };
  for ( int i = 1; i < 3; i++ )
  {
    textures.anim_pilot_run_rect[i].x = textures.anim_pilot_run_rect[i-1].x + textures.anim_pilot_run_rect[i-1].w;
    textures.anim_pilot_run_rect[i].y = 0;
    textures.anim_pilot_run_rect[i].w = 7;
    textures.anim_pilot_run_rect[i].h = 7;
  }

  textures.zeppelin_score_rect[0] = { 0, 0, 5, 6 };
  for ( int i = 1; i < 10; i++ )
  {
    textures.zeppelin_score_rect[i].y = 0;
    textures.zeppelin_score_rect[i].w = 5;
    textures.zeppelin_score_rect[i].h = 6;
    textures.zeppelin_score_rect[i].x = textures.zeppelin_score_rect[i-1].x + textures.zeppelin_score_rect[i].w;
  }
  textures.zeppelin_score_rect[10] = { 0, 6, 5, 6 };
  for ( int i = 11; i < 20; i++ )
  {
    textures.zeppelin_score_rect[i].y = 6;
    textures.zeppelin_score_rect[i].w = 5;
    textures.zeppelin_score_rect[i].h = 6;
    textures.zeppelin_score_rect[i].x = textures.zeppelin_score_rect[i-1].x + textures.zeppelin_score_rect[i].w;
  }


  log_message( "\nRESOURCES: Finished loading textures!\n\n" );
}


// Load sounds
void sounds_load()
{
    log_message( "RESOURCES: Loading sounds..." );

    sounds.shoot = loadSound( "assets/sounds/shoot.wav" );
    sounds.hit = loadSound( "assets/sounds/hit.wav" );
    sounds.hitChute = loadSound( "assets/sounds/hit_chute.wav" );
    sounds.hitMiss = loadSound( "assets/sounds/hit_miss.wav" );
    sounds.expl = loadSound( "assets/sounds/expl.wav" );
    sounds.fall = loadSound( "assets/sounds/fall.wav" );
    sounds.chute = loadSound( "assets/sounds/chute.wav" );
    sounds.dead = loadSound( "assets/sounds/dead.wav" );

    log_message( "\nRESOURCES: Finished loading sounds!\n\n" );
}
