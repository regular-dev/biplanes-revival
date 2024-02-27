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

#include <map>
#include <vector>
#include <cstddef>


class AiState
{
  float mPriority {};
  float mSensitivity {};


public:
  AiState( const float sensitivity );

  virtual void update(
    const Plane& self,
    const Plane& opponent,
    const std::vector <Bullet>& opponentBullets );

  virtual std::vector <AiAction> actions(
    const Plane& self,
    const Plane& opponent,
    const std::vector <Bullet>& opponentBullets ) const;

  void resetPriority( const float newPriority = 0.f );

  float priority() const;


protected:
  void updatePriority( const float priority );
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
};

extern AiController aiController;
