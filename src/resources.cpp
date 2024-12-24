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

#include <include/resources.hpp>
#include <include/sdl.hpp>
#include <include/sounds.hpp>
#include <include/textures.hpp>
#include <include/utility.hpp>

#include <cstdlib>
#include <cstring>
#include <vector>
#include <filesystem>

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


  textures.background = loadTexture( assetsRoot + "/ingame/background.png" );
  textures.barn = loadTexture( assetsRoot + "/ingame/barn.png" );
  textures.plane_blue = loadTexture( assetsRoot + "/ingame/plane_blue.png" );
  textures.plane_red = loadTexture( assetsRoot + "/ingame/plane_red.png" );
  textures.bullet = loadTexture( assetsRoot + "/ingame/bullet.png" );
  textures.cloud = loadTexture( assetsRoot + "/ingame/cloud.png" );
  textures.cloud_opaque = loadTexture( assetsRoot + "/ingame/cloud_opaque.png" );
  textures.zeppelin = loadTexture( assetsRoot + "/ingame/zeppelin.png" );
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


  std::vector <SDL_Texture*> backgroundFrames {};

  for ( size_t i {}; ; ++i )
  {
    const auto filename =
      assetsRoot + "/ingame/background_animation/frame" + std::to_string(i) + ".png";

    if ( std::filesystem::exists(filename) == false )
    {
      textures.anim_background_frame_count = i;
      break;
    }
    else
      backgroundFrames.push_back(loadTexture(filename));
  }

  if ( backgroundFrames.empty() == false )
  {
    textures.anim_background = new SDL_Texture*[backgroundFrames.size()];

    for (  size_t i {}; i < backgroundFrames.size(); ++i )
      textures.anim_background[i] = backgroundFrames[i];
  }


  log_message( "\nRESOURCES: Finished loading textures!\n\n" );
}

void
textures_unload()
{
  log_message( "RESOURCES: Unloading textures..." );


  SDL_DestroyTexture(textures.main_font);
  SDL_DestroyTexture(textures.menu_help);
  SDL_DestroyTexture(textures.menu_button);
  SDL_DestroyTexture(textures.menu_logo);
  SDL_DestroyTexture(textures.font_zeppelin_score);
  SDL_DestroyTexture(textures.background);
  SDL_DestroyTexture(textures.barn);
  SDL_DestroyTexture(textures.plane_red);
  SDL_DestroyTexture(textures.plane_blue);
  SDL_DestroyTexture(textures.bullet);
  SDL_DestroyTexture(textures.cloud);
  SDL_DestroyTexture(textures.cloud_opaque);
  SDL_DestroyTexture(textures.zeppelin);
  SDL_DestroyTexture(textures.anim_smk);
  SDL_DestroyTexture(textures.anim_fire);
  SDL_DestroyTexture(textures.anim_expl);
  SDL_DestroyTexture(textures.anim_hit);
  SDL_DestroyTexture(textures.anim_chute);
  SDL_DestroyTexture(textures.anim_pilot_angel);
  SDL_DestroyTexture(textures.anim_pilot_fall_red);
  SDL_DestroyTexture(textures.anim_pilot_fall_blue);
  SDL_DestroyTexture(textures.anim_pilot_run_red);
  SDL_DestroyTexture(textures.anim_pilot_run_blue);

  for ( size_t i {}; i < textures.anim_background_frame_count; ++i )
    SDL_DestroyTexture(textures.anim_background[i]);

  delete[] textures.anim_background;

  textures = {};


  log_message( "\nRESOURCES: Finished unloading textures!\n\n" );
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

void
sounds_unload()
{
  log_message( "RESOURCES: Unloading sounds..." );


  Mix_FreeChunk(sounds.shoot);
  Mix_FreeChunk(sounds.explosion);
  Mix_FreeChunk(sounds.hitPlane);
  Mix_FreeChunk(sounds.hitChute);
  Mix_FreeChunk(sounds.hitGround);
  Mix_FreeChunk(sounds.pilotFallLoop);
  Mix_FreeChunk(sounds.pilotChuteLoop);
  Mix_FreeChunk(sounds.pilotDeath);
  Mix_FreeChunk(sounds.pilotRescue);
  Mix_FreeChunk(sounds.victory);
  Mix_FreeChunk(sounds.defeat);

  sounds = {};


  log_message( "\nRESOURCES: Finished unloading sounds!\n\n" );
}
