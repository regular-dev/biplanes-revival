/*
  Biplanes Revival
  Copyright (C) 2019-2025 Regular-dev community
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

  SDL_Texture* menu_help {};
  SDL_Texture* menu_button {};
  SDL_Texture* menu_logo {};

  SDL_Texture* font_zeppelin_score {};

  SDL_Texture* background {};
  SDL_Texture* barn {};
  SDL_Texture* plane_red {};
  SDL_Texture* plane_blue {};
  SDL_Texture* bullet {};
  SDL_Texture* cloud {};
  SDL_Texture* cloud_opaque {};
  SDL_Texture* zeppelin {};

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
  SDL_Texture* anim_pilot_fall_red {};
  SDL_Texture* anim_pilot_fall_blue {};
  SDL_Rect anim_pilot_fall_rect[2] {};
  SDL_Texture* anim_pilot_run_red {};
  SDL_Texture* anim_pilot_run_blue {};
  SDL_Rect anim_pilot_run_rect[3] {};

  SDL_Rect font_rect[95] {};
  SDL_Rect zeppelin_score_rect[20] {};

  SDL_Texture** anim_background {};
  size_t anim_background_frame_count {};


  Textures() = default;
};

extern Textures textures;
