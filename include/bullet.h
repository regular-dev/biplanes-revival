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

#include <include/plane.h>

#include <vector>


class Bullet;
class Plane;

class BulletSpawner
{
  std::vector <Bullet> instances;

public:
  void SpawnBullet( float planeX, float planeY, float planeDir, bool planeType );
  void Clear();
  void UpdateBullets();
  void Draw();

  Bullet GetClosestBullet( const float planeX, const float planeY, const bool planeType );
};

class Bullet
{
  bool alive;
  float x;
  float y;
  float dir;
  bool fired_by;
  char hit_frame;
  Timer hit_anim;
  SDL_Rect hit_destrect;

public:
  Bullet( const float planeX, const float planeY, const float planeDir, const bool planeType );

  bool isDead();

  void Update();
  void UpdateCoordinates();
  void AnimUpdate();
  void HitAnimUpdate();
  void HitGround();
  void Draw();
  void Destroy();

  friend Bullet BulletSpawner::GetClosestBullet( const float, const float, const bool );
  friend void BulletSpawner::Draw();
};
