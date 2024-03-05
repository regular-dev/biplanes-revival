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

#include <map>
#include <vector>
#include <cstddef>


class AiTemperature
{
public:

  struct Weights
  {
      float positive {};
      float negative {};

      Weights() = default;

      static Weights FromTime(
        const float heatupTime,
        const float cooldownTime );
  };


  AiTemperature() = default;
  AiTemperature( const Weights&, const float value = {} );

  void update( const float newValue, const float factor = 1.f );
  void set( const float newValue );

  operator float () const;
  Weights weights() const;


private:
  float mValue {};
  Weights mWeights {};
};


class ContextMap
{
  std::vector <float> mValues {};


public:
  ContextMap( const size_t slotCount );

  void write( const size_t slot, const float value );
  float value( const size_t slot ) const;

  float minValue() const;
  float maxValue() const;

  size_t minValueSlot() const;
  size_t maxValueSlot() const;

  ContextMap operator - ( const ContextMap& ) const;
  ContextMap mask( const ContextMap& other, const float threshold ) const;
};


class AiState
{
protected:

  AiTemperature mTemperature {};

  std::map <AiAction, AiTemperature> mActions {};


public:
  AiState( const AiTemperature& temperature );

  virtual void update(
    const Plane& self,
    const Plane& opponent,
    const std::vector <Bullet>& opponentBullets );

  virtual std::vector <AiAction> actions() const;

  virtual void drawDebugLayer( const Plane& self ) const;
  void printActionTemperatures() const;

  void setTemperature( const float );
  float temperature() const;
};


class AiStateController
{
  std::vector <AiState*> mStates {};
  AiState* mCurrentState {};

public:
  AiStateController() = default;

  void init();

  void update(
    const Plane& self,
    const Plane& opponent,
    const std::vector <Bullet>& opponentBullets );

  void drawDebugLayer( const Plane& self ) const;

  const AiState* currentState() const;
};


class AiController
{
  std::map <PLANE_TYPE, AiStateController> mStateController
  {
    {PLANE_TYPE::BLUE, {}},
    {PLANE_TYPE::RED, {}},
  };


public:
  AiController() = default;

  void init();
  void update();

  void drawDebugLayer() const;
};

extern AiController aiController;
