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
#include <include/sdl.hpp>
#include <include/time.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/menu.hpp>
#include <include/plane.hpp>
#include <include/bullet.hpp>
#include <include/utility.hpp>

#include <lib/SDL_Vector.h>

#include <cassert>
#include <iomanip>
#include <sstream>
#include <algorithm>


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


ContextMap::ContextMap(
  const size_t slotCount )
  : mValues(slotCount)
{
}

void
ContextMap::write(
  const size_t slot,
  const float value )
{
  assert(slot < mValues.size());

  mValues[slot] = value;
}

float&
ContextMap::operator [] (
  const size_t slot )
{
  assert(slot < mValues.size());

  return mValues[slot];
}

float
ContextMap::operator [] (
  const size_t slot ) const
{
  assert(slot < mValues.size());

  return mValues[slot];
}

float
ContextMap::minValue() const
{
  assert(mValues.empty() == false);

  return *std::min_element(
    mValues.cbegin(),
    mValues.cend() );
}

float
ContextMap::maxValue() const
{
  assert(mValues.empty() == false);

  return *std::max_element(
    mValues.cbegin(),
    mValues.cend() );
}

size_t
ContextMap::minValueSlot() const
{
  assert(mValues.empty() == false);

  const auto lowestValueIter = std::min_element(
    mValues.cbegin(),
    mValues.cend() );

  return std::distance(
    mValues.cbegin(),
    lowestValueIter );
}

size_t
ContextMap::maxValueSlot() const
{
  assert(mValues.empty() == false);

  const auto highestValueIter = std::max_element(
    mValues.cbegin(),
    mValues.cend() );

  return std::distance(
    mValues.cbegin(),
    highestValueIter );
}

ContextMap
ContextMap::operator - (
  const ContextMap& other ) const
{
  assert(mValues.size() == other.mValues.size());

  ContextMap result {*this};

  for ( size_t i {}; i < mValues.size(); ++i )
    result.mValues[i] -= other.mValues[i];

  return result;
}

ContextMap
ContextMap::mask(
  const ContextMap& other,
  const float threshold ) const
{
  assert(mValues.empty() == false);
  assert(mValues.size() == mValues.size());

  ContextMap result {*this};

  for ( size_t i {}; i < mValues.size(); ++i )
    if ( other.mValues[i] > threshold )
      result.mValues[i] = {};

  return result;
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
  AiStateTest( const AiTemperature& temperature );

  void update(
    const Plane& self,
    const Plane& opponent,
    const std::vector <Bullet>& opponentBullets ) override;

  std::vector <AiAction> actions() const override;

  void drawDebugLayer( const Plane& self ) const override;
};

AiStateTest::AiStateTest(
  const AiTemperature& temperature )
  : AiState(temperature)
{
  constexpr auto ReactionTimeToWeights = &AiTemperature::Weights::FromTime;

  const auto throttleWeight = ReactionTimeToWeights(0.1f, 0.1f);
  const auto pitchWeight = ReactionTimeToWeights(0.1f, 0.1f);
  const auto shootWeight = ReactionTimeToWeights(0.04f, 0.04f);

  mActions =
  {
    {AiAction::Accelerate, throttleWeight},
    {AiAction::Decelerate, throttleWeight},
    {AiAction::TurnLeft, pitchWeight},
    {AiAction::TurnRight, pitchWeight},
    {AiAction::Shoot, shootWeight},
    {AiAction::Jump, {}},
  };
}

void
AiStateTest::update(
  const Plane& self,
  const Plane& opponent,
  const std::vector <Bullet>& opponentBullets )
{
  const float temperature = 1.f;

  mTemperature.update(temperature, deltaTime);


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
    {
      actions.push_back(AiAction::TurnLeft);
      actions.push_back(AiAction::TurnRight);
    }
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

  for ( auto& [action, temperature] : mActions )
  {
    const bool actionFound = std::find(
      actions.cbegin(),
      actions.cend(),
      action ) != actions.cend();

    if ( actionFound == true )
      temperature.update(1.f, deltaTime);

    else
      temperature.update(0.f, deltaTime);
  }
}

std::vector <AiAction>
AiStateTest::actions() const
{
  const float threshold = 0.95f;

  std::vector <AiAction> actions {};

  for ( auto& [action, temperature] : mActions )
    if ( temperature >= threshold )
      actions.push_back(action);

  return actions;
}

void
AiStateTest::drawDebugLayer(
  const Plane& self ) const
{
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

    const auto aiActions = aiState->actions();

    for ( const auto aiAction : aiActions )
      plane.input.ExecuteAiAction(aiAction);
  }
}

void
AiController::drawDebugLayer() const
{
  for ( auto& [planeType, plane] : planes )
  {
    if ( plane.isBot() == false )
      continue;

    auto& stateController = mStateController.at(plane.type());

    if ( plane.isDead() == false )
      stateController.drawDebugLayer(plane);
  }
}


void
AiStateController::init()
{
  mStates =
  {
    new AiStateTest({{}, 1.f}),
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
        return lhs->temperature() < rhs->temperature();
      });
}

void
AiStateController::drawDebugLayer(
  const Plane& self ) const
{
  assert(mCurrentState != nullptr );

  mCurrentState->drawDebugLayer(self);
}

const AiState*
AiStateController::currentState() const
{
  return mCurrentState;
}


AiState::AiState(
const AiTemperature& temperature )
  : mTemperature{temperature}
{
}

void
AiState::update(
  const Plane& self,
  const Plane& opponent,
  const std::vector <Bullet>& opponentBullets )
{
  mTemperature.update(0.f);
}

std::vector <AiAction>
AiState::actions() const
{
  return {};
}

void
AiState::drawDebugLayer(
  const Plane& self ) const
{
}

void
AiState::printActionTemperatures() const
{
  log_message("action temps: ");

  for ( auto& [action, temperature] : mActions )
  {
    const auto temperatureString = (std::stringstream {}
      << std::fixed
      << std::setprecision(2)
      << temperature).str();

    log_message(temperatureString + ", ");
  }

  log_message("\n");
}

void
AiState::setTemperature(
  const float newTemperature )
{
  mTemperature.set(newTemperature);
}

float
AiState::temperature() const
{
  return mTemperature;
}
