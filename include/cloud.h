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

#include <include/sdl.h>


class Cloud
{
  float x {};
  float y {};
  bool dir {};

  uint8_t id {};
  bool opaque {};
  SDL_Rect collision_box {};

public:
  Cloud() = default;
  Cloud( bool newDir, uint8_t newId );

  void Update();
  void UpdateCoordinates();
  void UpdateHeight();
  void UpdateCollisionBox();

  void Draw();
  void setTransparent();
  void setOpaque();
  void Respawn();

  bool isHit( float hitX, float hitY );
};
