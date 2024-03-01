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

#include <include/ai_stuff.hpp>
#include <include/biplanes.hpp>
#include <include/controls.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/menu.hpp>
#include <include/plane.hpp>
#include <include/bullet.hpp>
#include <include/utility.hpp>

#include <lib/SDL_Vector.h>

#include <algorithm>
#include <cassert>


static float
get_distance_between_points(
  const SDL_Vector& p1,
  const SDL_Vector& p2 )
{
  return std::sqrt(
    std::pow( p2.x - p1.x, 2.f ) +
    std::pow( p2.y - p1.y, 2.f ) );
}

static float
get_angle_relative(
  const float angleSource,
  const float angleTarget )
{
  float relativeAngle = angleTarget - angleSource;

  if ( relativeAngle > 180.f )
    relativeAngle -= 360.f;
  else if ( relativeAngle < -180.f )
    relativeAngle += 360.f;

  return relativeAngle;
}

static float
get_angle_to_point(
  const SDL_Vector& source,
  const SDL_Vector& target )
{
  const SDL_Vector dir = target - source;

  float angle = std::atan2(dir.y, dir.x) + M_PI / 2.0;

  angle *= 180.f / M_PI;

  if ( angle < 0.f )
    angle += 360.f;

  return angle;
}


AiTemperature::AiTemperature(
const Weights& sensitivity,
  const float value )
  : mValue {value}
  , mWeights{sensitivity}
{
}

void
AiTemperature::update(
  const float newValue,
  const float factor )
{
  const auto weight = newValue >= mValue
  ? mWeights.positive
  : mWeights.negative;

  const auto valueDiff = newValue - mValue;

  mValue += factor * weight * std::copysign(1.f, valueDiff);
  mValue = std::clamp(mValue, 0.0f, 1.0f);
}

void
AiTemperature::set(
  const float newValue )
{
  mValue = newValue;
}

AiTemperature::operator float () const
{
  return mValue;
}

AiTemperature::Weights
AiTemperature::weights() const
{
  return mWeights;
}

AiTemperature::Weights
AiTemperature::Weights::FromTime(
  const float heatupTime,
  const float cooldownTime )
{
  return
  {
    1.f / heatupTime,
    1.f / cooldownTime,
  };
}


class AiStateTest : public AiState
{
public:
  AiStateTest( const float sensitivity );

  void update(
    const Plane& self,
    const Plane& opponent,
    const std::vector <Bullet>& opponentBullets ) override;

  std::vector <AiAction> actions(
    const Plane& self,
    const Plane& opponent,
    const std::vector <Bullet>& opponentBullets ) const override;
};

AiStateTest::AiStateTest(
  const float sensitivity )
  : AiState(sensitivity)
{}

void
AiStateTest::update(
  const Plane& self,
  const Plane& opponent,
  const std::vector <Bullet>& opponentBullets )
{
  const float priority = 1.f;

  updatePriority(priority);
}

std::vector <AiAction>
AiStateTest::actions(
  const Plane& self,
  const Plane& opponent,
  const std::vector <Bullet>& opponentBullets ) const
{
  namespace plane = constants::plane;


  std::vector <AiAction> actions {};

  const SDL_Vector pos {self.x(), self.y()};
  const SDL_Vector opponentPos {opponent.pilot.x(), opponent.pilot.y()};

  const SDL_Vector opponentPosL = opponentPos - SDL_Vector{1.f, 0.f};
  const SDL_Vector opponentPosR = opponentPos + SDL_Vector{1.f, 0.f};

  const auto opponentDistance  = get_distance_between_points(pos, opponentPos);
  const auto opponentDistanceL = get_distance_between_points(pos, opponentPosL);
  const auto opponentDistanceR = get_distance_between_points(pos, opponentPosR);

  SDL_Vector opponentPosClosest {opponentPos};

  if ( opponentDistanceL < opponentDistance )
    opponentPosClosest = opponentPosL;

  if ( opponentDistanceR < opponentDistance )
    opponentPosClosest = opponentPosR;

  const auto targetDirAbsolute = get_angle_to_point(
    pos, opponentPosClosest );

  const auto targetDirRelative = get_angle_relative(
    self.dir(), targetDirAbsolute );


  bool needClimb {};

  if ( self.speedVector().y >= 0.f &&
       self.speed() <= 0.5f * self.maxSpeed() )
    needClimb = true;


  if ( needClimb == true )
  {
    if (  self.dir() >= -3.f * plane::pitchStep &&
          self.dir() <= 3.f * plane::pitchStep )
    actions.push_back(AiAction::TurnRight);
  }

  else if ( targetDirRelative >= plane::pitchStep * 0.5f )
    actions.push_back(AiAction::TurnRight);

  else if ( targetDirRelative <= -plane::pitchStep * 0.5f )
    actions.push_back(AiAction::TurnLeft);

  else
    actions.push_back(AiAction::Shoot);


  const auto opponentDistanceClosest = get_distance_between_points(
    pos, opponentPosClosest );

  if (  needClimb == true ||
        opponentDistanceClosest > 2.f * constants::plane::sizeX )
    actions.push_back(AiAction::Accelerate);

  else
    actions.push_back(AiAction::Decelerate);


  return actions;
}


void
AiController::init()
{
  for ( auto& [planeType, controller] : mStateController )
    controller.init();
}

void
AiController::update()
{
  for ( auto& [planeType, plane] : planes )
  {
    if ( plane.isBot() == false )
      continue;

    auto& stateController = mStateController.at(plane.type());

    if ( plane.isDead() == true )
    {
      stateController.init();
      continue;
    }


    const auto& opponentPlane =
      planes.at(static_cast <PLANE_TYPE> (!plane.type()));

    const auto opponentBullets = bullets.GetClosestBullets(
      plane.x(), plane.y(),
      plane.type() );

    stateController.update(
      plane, opponentPlane,
      opponentBullets );

    const auto aiState = stateController.currentState();

    const auto aiActions = aiState->actions(
      plane, opponentPlane,
      opponentBullets );

    for ( const auto aiAction : aiActions )
      plane.input.ExecuteAiAction(aiAction);
  }
}


void
AiStateController::init()
{
  mStates =
  {
    new AiStateTest(1.f),
  };

  mCurrentState = mStates.back();
}

void
AiStateController::update(
  const Plane& self,
  const Plane& opponent,
  const std::vector <Bullet>& opponentBullets )
{
  assert(mStates.empty() == false);
  assert(mCurrentState != nullptr );


  for ( const auto state : mStates )
    state->update(
      self, opponent,
      opponentBullets );

  mCurrentState = *std::max_element(
    mStates.begin(), mStates.end(),
      [] ( const AiState* lhs, const AiState* rhs )
      {
        return lhs->priority() < rhs->priority();
      });
}

const AiState*
AiStateController::currentState() const
{
  return mCurrentState;
}


AiState::AiState(
  const float sensitivity )
  : mSensitivity{sensitivity}
//  reactionTime = tickTime / sensitivity
//  sensitivity = tickTime / reactionTime
{
}

void
AiState::update(
  const Plane& self,
  const Plane& opponent,
  const std::vector <Bullet>& opponentBullets )
{
  updatePriority(0.0);
}

std::vector <AiAction>
AiState::actions(
  const Plane& self,
  const Plane& opponent,
  const std::vector <Bullet>& opponentBullets ) const
{
  return {};
}

void
AiState::updatePriority(
  const float newPriority )
{
  mPriority += (newPriority - mPriority) * mSensitivity;
  mPriority = std::clamp(mPriority, 0.0f, 1.0f);
}

void
AiState::resetPriority(
  const float newPriority )
{
  mPriority = newPriority;
}

float
AiState::priority() const
{
  return mPriority;
}
