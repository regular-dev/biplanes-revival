#ifndef PLANE_H
#define PLANE_H

#include "sdl.h"


enum BUTTON_DIR
{
  LEFT,
  RIGHT
};

enum PLANE_THROTTLE
{
  THROTTLE_IDLE,
  THROTTLE_DECREASE,
  THROTTLE_INCREASE
};

enum PLANE_PITCH
{
  PITCH_IDLE,
  PITCH_LEFT,
  PITCH_RIGHT
};

enum PLANE_TYPE
{
  BLUE,
  RED
};

enum CHUTE_STATE
{
  CHUTE_IDLE,
  CHUTE_LEFT,
  CHUTE_RIGHT,
  CHUTE_DESTROYED,
  CHUTE_NONE
};

enum HIT_STATE
{
  HIT_NONE,
  HIT_PLANE,
  HIT_CHUTE,
  HIT_PILOT
};

enum DEATH_STATE
{
  DEATH_NONE,
  PLANE_DEATH,
  PILOT_DEATH,
  PILOT_RESP
};


namespace SPECIFY
{
  enum SPECIFY
  {
    IP,
    PORT,
    PASSWORD
  };
}


enum class MESSAGE_TYPE
{
  NONE,

  SOCKET_INIT_FAILED,
  CANT_START_CONNECTION,

  CLIENT_CONNECTING,
  CONNECTION_FAILED,
  CONNECTION_TIMED_OUT,
  HOST_CEASED_CONNECTION,

  HOST_LISTENING,
  CLIENT_DISCONNECTED,

  SUCCESSFULL_CONNECTION,

  SEARCHING_FOR_OPP,
  MMAKE_PTP_TIMEOUT
};

enum class ROOMS
{
  MENU_COPYRIGHT,
  MENU_SPLASH,
  MENU_MAIN,
  MENU_MP,
  MENU_MP_HELP,
  MENU_MP_MMAKE,
  MENU_MP_MMAKE_FIND_GAME,
  MENU_MP_MMAKE_HELP,
  MENU_MP_DC,
  MENU_MP_DC_HOST,
  MENU_MP_DC_JOIN,
  MENU_MP_DC_HELP,
  MENU_SETTINGS_CONTROLS,
  MENU_HELP,
  MENU_PAUSE,
  GAME
};

namespace MENU_MAIN
{
  enum MENU_MAIN
  {
    MULTIPLAYER,
    SETTINGS,
    HELP,
    EXIT
  };
}

namespace MENU_MP
{
  enum MENU_MP
  {
    MMAKE,
    DC,
    HELP,
    BACK
  };
}

namespace MENU_MP_MMAKE
{
  enum MENU_MP_MMAKE
  {
    FIND_GAME,
    SPECIFY_PASSWORD,
    HELP,
    BACK
  };
}

namespace MENU_MP_MMAKE_FIND_GAME
{
  enum MENU_MP_MMAKE_FIND_GAME
  {
    BACK
  };
}

namespace MENU_MP_DC
{
  enum MENU_MP_DC
  {
    HOST,
    JOIN,
    HELP,
    BACK
  };
}

namespace MENU_MP_DC_HOST
{
  enum MENU_MP_DC_HOST
  {
    HOST_START,
    SPECIFY_PORT,
    HARDCORE_MODE,
    BACK
  };
}

namespace MENU_MP_DC_JOIN
{
  enum MENU_MP_DC_JOIN
  {
    JOIN,
    SPECIFY_IP,
    SPECIFY_PORT,
    BACK
  };
}

namespace MENU_SETTINGS_CONTROLS
{
  enum MENU_SETTINGS_CONTROLS
  {
    ACCELERATE,
    DECELERATE,
    LEFT,
    RIGHT,
    SHOOT,
    EJECT,
    BACK
  };
}

namespace MENU_PAUSE
{
  enum MENU_PAUSE
  {
    CONTINUE,
    CONTROLS,
    HELP,
    DISCONNECT
  };
}


struct Sizes
{
  int screen_height;
  int screen_width;

  int screen_height_new;
  int screen_width_new;

  unsigned short text_sizex;
  unsigned short text_sizey;

  float button_x = 127;
  bool button_dir = BUTTON_DIR::RIGHT;
  unsigned short button_sizex;
  unsigned short button_sizey;


  // PLANE
  float ground_y_collision;
  unsigned short plane_sizex;
  unsigned short plane_sizey;
  float plane_max_speed_def;
  float plane_max_speed_acc;
  float plane_incr_spd;
  float plane_incr_rot;

  float plane_dead_cooldown_time;
  float plane_pitch_cooldown_time;
  float plane_fire_cooldown_time;

  unsigned short plane_blue_landx;
  unsigned short plane_red_landx;
  unsigned short plane_landy;

  // SMOKE ANIM
  unsigned short smk_sizex;
  unsigned short smk_sizey;
  float smk_frame_time;
  float smk_anim_period;

  // FIRE ANIM
  float fire_frame_time;

  // EXPLOSION ANIM
  unsigned short expl_sizex;
  unsigned short expl_sizey;
  float expl_frame_time;


  // BULLET
  unsigned short bullet_sizex;
  unsigned short bullet_sizey;
  float bullet_speed;
  unsigned short bullet_count;

  unsigned short hit_sizex;
  unsigned short hit_sizey;
  float bullet_hit_frame_time;
  unsigned short bullet_ground_collision;


  // PILOT
  float ground_y_pilot_collision;
  float pilot_eject_speed;
  float pilot_gravity;
  float pilot_chute_gravity;
  float pilot_chute_speed;
  float pilot_run_speed;
  float pilot_run_frame_time;
  float pilot_fall_frame_time;

  float angel_ascent_speed;
  unsigned short pilot_sizex;
  unsigned short pilot_sizey;

  // CHUTE
  unsigned short chute_sizex;
  unsigned short chute_sizey;
  float chute_frame_time;

  // PILOT DEATH
  unsigned short angel_sizex;
  unsigned short angel_sizey;
  float angel_frame_time;


  // BARN
  unsigned short barn_sizex;
  unsigned short barn_sizey;

  unsigned short barn_x_collision;
  unsigned short barn_y_collision;

  unsigned short barn_x_pilot_left_collision;
  unsigned short barn_x_pilot_right_collision;

  unsigned short barn_x_bullet_collision;
  unsigned short barn_y_bullet_collision;


  // CLOUDS
  unsigned short cloud_sizex;
  unsigned short cloud_sizey;
  float cloud_speed;
  unsigned short cloud_left_spawn_x;
  unsigned short cloud_right_spawn_x;
  unsigned short cloud_highest_y;
  unsigned short cloud_lowest_y;


  // ZEPPELIN
  unsigned short zeppelin_sizex;
  unsigned short zeppelin_sizey;
  float zeppelin_speed;

  unsigned short zeppelin_spawn_x;
  unsigned short zeppelin_highest_y;
  unsigned short zeppelin_lowest_y;

  // ZEPPELIN SCORE
  unsigned short zeppelin_score_sizex;
  unsigned short zeppelin_score_sizey;
};


struct Controls
{
  char pitch = 0;
  char throttle = 0;
  bool fire = 0;
  bool jump = 0;
};


struct Textures
{
  SDL_Texture *main_font;

  SDL_Texture *menu_box;
  SDL_Texture *menu_settings_box;
  SDL_Texture *menu_settings_controls_box;
  SDL_Texture *menu_help;
  SDL_Texture *menu_moving_button;
  SDL_Texture *menu_logo;

  SDL_Texture *font_zeppelin_score;

  SDL_Texture *texture_background;
  SDL_Texture *texture_barn;
  SDL_Texture *texture_biplane_r;
  SDL_Texture *texture_biplane_b;
  SDL_Texture *texture_bullet;
  SDL_Texture *texture_cloud;
  SDL_Texture *texture_cloud_opaque;
  SDL_Texture *texture_zeppelin;

  SDL_Texture *anim_smk;
  SDL_Rect anim_smk_rect[6];
  SDL_Texture *anim_fire;
  SDL_Rect anim_fire_rect[3];
  SDL_Texture *anim_expl;
  SDL_Rect anim_expl_rect[7];

  SDL_Texture *anim_hit;
  SDL_Rect anim_hit_rect[5];

  SDL_Texture *anim_chute;
  SDL_Rect anim_chute_rect[3];
  SDL_Texture *anim_pilot_angel;
  SDL_Rect anim_pilot_angel_rect[4];
  SDL_Texture *anim_pilot_fall;
  SDL_Rect anim_pilot_fall_rect[2];
  SDL_Texture *anim_pilot_run;
  SDL_Rect anim_pilot_run_rect[3];

  SDL_Rect destrect;
  SDL_Rect font_rect[ 95 ];
  SDL_Rect zeppelin_score_rect[20];
};


struct Sounds
{
  Mix_Chunk *shoot = NULL;
  Mix_Chunk *hit = NULL;
  Mix_Chunk *hitChute = NULL;
  Mix_Chunk *hitMiss = NULL;
  Mix_Chunk *expl = NULL;
  Mix_Chunk *fall = NULL;
  Mix_Chunk *chute = NULL;
  Mix_Chunk *dead = NULL;
};

#endif //STRUCTURES_H
