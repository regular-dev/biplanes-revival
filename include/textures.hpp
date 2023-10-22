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

#pragma once

#include <SDL_image.h>


struct Textures
{
  SDL_Texture* main_font {};

  SDL_Texture* menu_box {};
  SDL_Texture* menu_settings_box {};
  SDL_Texture* menu_settings_controls_box {};
  SDL_Texture* menu_help {};
  SDL_Texture* menu_moving_button {};
  SDL_Texture* menu_logo {};

  SDL_Texture* font_zeppelin_score {};

  SDL_Texture* texture_background {};
  SDL_Texture* texture_barn {};
  SDL_Texture* texture_biplane_r {};
  SDL_Texture* texture_biplane_b {};
  SDL_Texture* texture_bullet {};
  SDL_Texture* texture_cloud {};
  SDL_Texture* texture_cloud_opaque {};
  SDL_Texture* texture_zeppelin {};

  SDL_Texture* anim_smk {};
  SDL_Rect anim_smk_rect[6] {};
  SDL_Texture* anim_fire {};
  SDL_Rect anim_fire_rect[3] {};
  SDL_Texture* anim_expl {};
  SDL_Rect anim_expl_rect[7] {};

  SDL_Texture* anim_hit {};
  SDL_Rect anim_hit_rect[5] {};

  SDL_Texture* anim_chute {};
  SDL_Rect anim_chute_rect[3] {};
  SDL_Texture* anim_pilot_angel {};
  SDL_Rect anim_pilot_angel_rect[4] {};
  SDL_Texture* anim_pilot_fall_r {};
  SDL_Texture* anim_pilot_fall_b {};
  SDL_Rect anim_pilot_fall_rect[2] {};
  SDL_Texture* anim_pilot_run_r {};
  SDL_Texture* anim_pilot_run_b {};
  SDL_Rect anim_pilot_run_rect[3] {};

  SDL_Rect font_rect[95] {};
  SDL_Rect zeppelin_score_rect[20] {};


  Textures() = default;
};

extern Textures textures;
