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

#include <include/resources.hpp>
#include <include/sdl.hpp>
#include <include/sounds.hpp>
#include <include/textures.hpp>
#include <include/utility.hpp>

#include <cstdlib>
#include <cstring>

#define ASSETS_DIRNAME "assets"


static std::string
get_assets_root()
{
#if defined(_WIN32) || defined(__APPLE__) || defined(__MACH__)
  return ASSETS_DIRNAME;
#endif

  const auto assetsRootPath = std::getenv("BIPLANES_ASSETS_ROOT");

  if (  assetsRootPath == nullptr ||
        std::strcmp(assetsRootPath, "") == 0 )
    return ASSETS_DIRNAME;

  return std::string{assetsRootPath} + "/" ASSETS_DIRNAME;
}


void
textures_load()
{
  log_message( "RESOURCES: Loading textures..." );


  const auto assetsRoot = get_assets_root();

  textures.main_font = loadTexture( assetsRoot + "/menu/font.png" );

  for ( uint8_t i = 0; i < 95; ++i )
  {
    textures.font_rect[i] =
    {
      (i % 19) * 8,
      (i / 19) * 8,
      8,
      8,
    };
  }


  textures.menu_help = loadTexture( assetsRoot + "/menu/screen_help.png" );
  textures.menu_button = loadTexture( assetsRoot + "/menu/button.png" );
  textures.menu_logo = loadTexture( assetsRoot + "/menu/screen_logo.png" );


  textures.texture_background = loadTexture( assetsRoot + "/ingame/background.png" );
  textures.texture_barn = loadTexture( assetsRoot + "/ingame/barn.png" );
  textures.texture_plane_blue = loadTexture( assetsRoot + "/ingame/plane_blue.png" );
  textures.texture_plane_red = loadTexture( assetsRoot + "/ingame/plane_red.png" );
  textures.texture_bullet = loadTexture( assetsRoot + "/ingame/bullet.png" );
  textures.texture_cloud = loadTexture( assetsRoot + "/ingame/cloud.png" );
  textures.texture_cloud_opaque = loadTexture( assetsRoot + "/ingame/cloud_opaque.png" );
  textures.texture_zeppelin = loadTexture( assetsRoot + "/ingame/zeppelin.png" );
  textures.font_zeppelin_score = loadTexture( assetsRoot + "/ingame/font_zeppelin_score.png" );

  textures.anim_smk = loadTexture( assetsRoot + "/ingame/smoke.png" );
  textures.anim_smk_rect[0] =
  {
    0, 0,
    13, 13,
  };

  for ( size_t i = 1; i < 6; ++i )
  {
    textures.anim_smk_rect[i] =
    {
      textures.anim_smk_rect[i - 1].x + textures.anim_smk_rect[i - 1].w,
      0,
      13, 13,
    };
  }

  textures.anim_fire = loadTexture( assetsRoot + "/ingame/fire.png" );
  textures.anim_fire_rect[0] =
  {
    0, 0,
    13, 13,
  };

  for ( size_t i = 1; i < 3; ++i )
  {
    textures.anim_fire_rect[i] =
    {
      textures.anim_fire_rect[i - 1].x + textures.anim_fire_rect[i - 1].w,
      0,
      13, 13,
    };
  }

  textures.anim_expl = loadTexture( assetsRoot + "/ingame/explosion.png" );
  textures.anim_expl_rect[0] =
  {
    0, 0,
    40, 40,
  };

  for ( size_t i = 1; i < 7; ++i )
  {
    textures.anim_expl_rect[i] =
    {
      textures.anim_expl_rect[i - 1].x + textures.anim_expl_rect[i - 1].w,
      0,
      40, 40,
    };
  }

  textures.anim_hit = loadTexture( assetsRoot + "/ingame/bullet_hit.png" );
  textures.anim_hit_rect[0] =
  {
    0, 0,
    9, 8,
  };

  for ( size_t i = 1; i < 5; ++i )
  {
    textures.anim_hit_rect[i] =
    {
      textures.anim_hit_rect[i - 1].x + textures.anim_hit_rect[i - 1].w,
      0,
      9, 8,
    };
  }

  textures.anim_chute = loadTexture( assetsRoot + "/ingame/chute.png" );
  textures.anim_chute_rect[0] =
  {
    0, 0,
    20, 12,
  };

  for ( size_t i = 1; i < 3; ++i )
  {
    textures.anim_chute_rect[i] =
    {
      textures.anim_chute_rect[i - 1].x + textures.anim_chute_rect[i - 1].w,
      0,
      20, 12,
    };
  }

  textures.anim_pilot_angel = loadTexture( assetsRoot + "/ingame/pilot_angel.png" );
  textures.anim_pilot_angel_rect[0] =
  {
    0, 0,
    10, 8,
  };

  for ( size_t i = 1; i < 4; ++i )
  {
    textures.anim_pilot_angel_rect[i] =
    {
      textures.anim_pilot_angel_rect[i-1].x + textures.anim_pilot_angel_rect[i-1].w,
      0,
      10, 8,
    };
  }

  textures.anim_pilot_fall_red = loadTexture( assetsRoot + "/ingame/pilot_fall_red.png" );
  textures.anim_pilot_fall_blue = loadTexture( assetsRoot + "/ingame/pilot_fall_blue.png" );
  textures.anim_pilot_fall_rect[0] = { 0, 0, 7, 7 };
  textures.anim_pilot_fall_rect[1] = { 7, 0, 7, 7 };

  textures.anim_pilot_run_red = loadTexture( assetsRoot + "/ingame/pilot_run_red.png" );
  textures.anim_pilot_run_blue = loadTexture( assetsRoot + "/ingame/pilot_run_blue.png" );
  textures.anim_pilot_run_rect[0] =
  {
    0, 0,
    7, 7,
  };

  for ( size_t i = 1; i < 3; ++i )
  {
    textures.anim_pilot_run_rect[i] =
    {
      textures.anim_pilot_run_rect[i - 1].x + textures.anim_pilot_run_rect[i - 1].w,
      0,
      7, 7,
    };
  }

  textures.zeppelin_score_rect[0] =
  {
    0, 0,
    5, 6,
  };

  for ( size_t i = 1; i < 10; ++i )
  {
    textures.zeppelin_score_rect[i] =
    {
      textures.zeppelin_score_rect[i - 1].x + textures.zeppelin_score_rect[i - 1].w,
      0,
      5, 6,
    };
  }


  textures.zeppelin_score_rect[10] =
  {
    0, 6,
    5, 6,
  };

  for ( size_t i = 11; i < 20; ++i )
  {
    textures.zeppelin_score_rect[i] =
    {
      textures.zeppelin_score_rect[i - 1].x + textures.zeppelin_score_rect[i - 1].w,
      6,
      5, 6,
    };
  }


  log_message( "\nRESOURCES: Finished loading textures!\n\n" );
}

void
sounds_load()
{
  log_message( "RESOURCES: Loading sounds..." );


  const auto assetsRoot = get_assets_root();

  sounds.shoot = loadSound( assetsRoot + "/sounds/shoot.ogg" );
  sounds.explosion = loadSound( assetsRoot + "/sounds/explosion.ogg" );
  sounds.hitPlane = loadSound( assetsRoot + "/sounds/hit_plane.ogg" );
  sounds.hitChute = loadSound( assetsRoot + "/sounds/hit_chute.ogg" );
  sounds.hitGround = loadSound( assetsRoot + "/sounds/hit_ground.ogg" );
  sounds.pilotFallLoop = loadSound( assetsRoot + "/sounds/fall_loop.ogg" );
  sounds.pilotChuteLoop = loadSound( assetsRoot + "/sounds/chute_loop.ogg" );
  sounds.pilotDeath = loadSound( assetsRoot + "/sounds/pilot_death.ogg" );
  sounds.pilotRescue = loadSound( assetsRoot + "/sounds/pilot_rescue.ogg" );
  sounds.victory = loadSound( assetsRoot + "/sounds/victory.ogg" );
  sounds.defeat = loadSound( assetsRoot + "/sounds/defeat.ogg" );

  log_message( "\nRESOURCES: Finished loading sounds!\n\n" );
}
