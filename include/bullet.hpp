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

#include <include/fwd.hpp>
#include <include/enums.hpp>
#include <include/timer.hpp>

#include <vector>


class Bullet
{
  float mX {};
  float mY {};
  float mDir {};

  bool mIsMoving {true};
  PLANE_TYPE mFiredBy {};

  int8_t mHitFrame {};
  Timer mHitAnim {0.0f};


public:
  Bullet(
    const float planeX,
    const float planeY,
    const float planeDir,
    const PLANE_TYPE );


  void Update();
  void UpdateCoordinates();
  void AnimUpdate();
  void HitAnimUpdate();
  void HitSurface();
  void Draw();
  void Destroy();


  bool isMoving() const;
  bool isDead() const;


  float x() const;
  float y() const;
  float dir() const;
  PLANE_TYPE firedBy() const;
};


class BulletSpawner
{
  std::vector <Bullet> mInstances {};


public:
  BulletSpawner() = default;


  void SpawnBullet(
    const float planeX,
    const float planeY,
    const float planeDir,
    const PLANE_TYPE );

  void Clear();
  void Update();
  void Draw();

  Bullet GetClosestBullet(
    const float planeX,
    const float planeY,
    const PLANE_TYPE ) const;
};

extern class BulletSpawner bullets;
