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

#include <cstdint>


struct Canvas
{
//  Virtual screen
  int32_t width {};
  int32_t height {};

  int32_t originX {};
  int32_t originY {};


//  SDL window
  int32_t windowWidth {};
  int32_t windowHeight {};

  int32_t windowWidthNew {};
  int32_t windowHeightNew {};


  Canvas() = default;
};

extern Canvas canvas;
