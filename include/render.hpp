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

#pragma once

#include <include/color.hpp>

#include <string>


void draw_text( const char text[], const float x, const float y );
void draw_text( const std::string&, const float x, const float y );
void draw_circle( const float x, const float y, const float radius, const size_t segments = 18 );

void draw_background();
void draw_ground_collision_layer();
void draw_barn();
void draw_barn_collision_layer();
void draw_score();
void draw_window_letterbox();

void display_update();
