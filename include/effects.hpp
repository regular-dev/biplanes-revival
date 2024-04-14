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

#include <include/timer.hpp>

#include <vector>
#include <cstdint>


class Effect
{
protected:
  float mX {};
  float mY {};

  Timer mAnim {0.0};
  uint8_t mFrame {};
  uint8_t mFrameCount {};


public:
  Effect(
    const float x,
    const float y,
    const double frameTime,
    const uint8_t frameCount );


  virtual ~Effect() = default;


  virtual void Update();
  void Draw() const;

  bool hasFinished() const;


protected:
  virtual void DrawImpl() const = 0;
};


class Effects
{
  std::vector <Effect*> mEffects {};


public:
  Effects() = default;
  ~Effects();

  void Spawn( Effect* );
  void Clear();

  void Update();
  void Draw() const;
};

extern Effects effects;


class SmokePuff : public Effect
{
public:
  SmokePuff(
    const float x,
    const float y );


protected:
  void DrawImpl() const override;
};


class Explosion : public Effect
{
public:
  Explosion(
    const float x,
    const float y );


protected:
  void DrawImpl() const override;
};


class ExplosionSpark : public Effect
{
protected:
  float mSpeedX {};
  float mSpeedY {};

  uint8_t mBounces {};


public:
  ExplosionSpark(
    const float x,
    const float y,
    const float speed,
    const float dir );

  void Update() override;


protected:
  void DrawImpl() const override;
};


class BulletImpact : public Effect
{
public:
  BulletImpact(
    const float x,
    const float y );


protected:
  void DrawImpl() const override;
};
