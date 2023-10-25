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

#include <include/enums.hpp>

#include <cstdint>


struct Sizes
{
  int32_t screen_height {};
  int32_t screen_width {};

  int32_t screen_height_new {};
  int32_t screen_width_new {};

  uint16_t text_sizex {};
  uint16_t text_sizey {};

  float button_x {127.0f};
  MENU_BUTTON_DIR button_dir {MENU_BUTTON_DIR::RIGHT};
  uint16_t button_sizex {};
  uint16_t button_sizey {};


//  PLANE
  const uint8_t plane_hp_max {2};
  float ground_y_collision {};
  uint16_t plane_sizex {};
  uint16_t plane_sizey {};
  float plane_max_speed_def {};
  float plane_max_speed_acc {};
  float plane_incr_spd {};
  float plane_incr_rot {};

  float plane_dead_cooldown_time {};
  float plane_spawn_protection_time {};

  float plane_pitch_cooldown_time {};
  float plane_fire_cooldown_time {};

  uint16_t plane_blue_landx {};
  uint16_t plane_red_landx {};
  uint16_t plane_landy {};


//  SMOKE ANIM
  uint16_t smk_sizex {};
  uint16_t smk_sizey {};
  float smk_frame_time {};
  float smk_anim_period {};

//  FIRE ANIM
  float fire_frame_time {};

//  EXPLOSION ANIM
  uint16_t expl_sizex {};
  uint16_t expl_sizey {};
  float expl_frame_time {};


//  PILOT
  float ground_y_pilot_collision {};
  float pilot_eject_speed {};
  float pilot_gravity {};
  float pilot_chute_gravity {};
  float pilot_chute_speed {};
  float pilot_run_speed {};
  float pilot_run_frame_time {};
  float pilot_fall_frame_time {};

  float angel_ascent_speed {};
  uint16_t pilot_sizex {};
  uint16_t pilot_sizey {};

//  CHUTE
  uint16_t chute_sizex {};
  uint16_t chute_sizey {};
  float chute_frame_time {};

//  PILOT DEATH
  uint16_t angel_sizex {};
  uint16_t angel_sizey {};
  float angel_frame_time {};


//  BULLET
  uint16_t bullet_sizex {};
  uint16_t bullet_sizey {};
  float bullet_speed {};
  uint16_t bullet_count {};

  uint16_t hit_sizex {};
  uint16_t hit_sizey {};
  float bullet_hit_frame_time {};
  uint16_t bullet_ground_collision {};


  // BARN
  uint16_t barn_sizex {};
  uint16_t barn_sizey {};

  uint16_t barn_x_collision {};
  uint16_t barn_y_collision {};

  uint16_t barn_x_pilot_left_collision {};
  uint16_t barn_x_pilot_right_collision {};

  uint16_t barn_x_bullet_collision {};
  uint16_t barn_y_bullet_collision {};


  // CLOUDS
  uint16_t cloud_sizex {};
  uint16_t cloud_sizey {};
  float cloud_speed {};
  uint16_t cloud_left_spawn_x {};
  uint16_t cloud_right_spawn_x {};
  int16_t cloud_highest_y {};
  int16_t cloud_lowest_y {};


  // ZEPPELIN
  uint16_t zeppelin_sizex {};
  uint16_t zeppelin_sizey {};
  float zeppelin_speed {};

  uint16_t zeppelin_spawn_x {};
  uint16_t zeppelin_highest_y {};
  uint16_t zeppelin_lowest_y {};

  // ZEPPELIN SCORE
  uint16_t zeppelin_score_sizex {};
  uint16_t zeppelin_score_sizey {};


  Sizes() = default;
};

extern Sizes sizes;
