/*
  Biplanes Revival
  Copyright (C) 2019-2025 Regular-dev community
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
#include <include/math.hpp>
#include <include/plane.hpp>
#include <include/bullet.hpp>
#include <include/render.hpp>
#include <include/utility.hpp>

#include <lib/SDL_Vector.h>
#include <lib/godot_math.hpp>

#include <cmath>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <algorithm>


static bool
isPlaneStalling(
  const Plane& plane )
{
  namespace barn = constants::barn;
  namespace barn = constants::barn;

  const auto gravity = plane.maxSpeed() - plane.speed();

  SDL_Vector planeTrajectoryStart
  {
    plane.x(), plane.y(),
  };

  SDL_Vector planeTrajectoryEnd
  {
    plane.x(), plane.y() + gravity,
  };

  const SDL_Vector groundSegment[2]
  {
    {-10.f, constants::plane::groundCollision},
    {10.f, constants::plane::groundCollision},
  };

  SDL_Vector groundContactPoint {};

  const bool collidesWithGround = segment_intersects_segment(
    planeTrajectoryStart, planeTrajectoryEnd,
    groundSegment[0], groundSegment[1],
    &groundContactPoint );

  const std::vector <SDL_Vector> barnBox
  {
    {barn::planeCollisionX, constants::plane::groundCollision},
    {barn::planeCollisionX, barn::planeCollisionY},
    {barn::planeCollisionX + barn::sizeX, barn::planeCollisionY},
    {barn::planeCollisionX + barn::sizeX, constants::plane::groundCollision},
  };

  SDL_Vector barnContactPoint {};

  const bool collidesWithBarn = segment_intersects_polygon(
    planeTrajectoryStart, planeTrajectoryEnd,
    barnBox,
    &barnContactPoint );

  return collidesWithGround || collidesWithBarn;

  return plane.y() >= constants::plane::groundCollision - gravity;
}

static bool
isOpponentCloseBehind(
  const Plane& opponent,
  const float dirToOpponentRelative,
  const float shortestDistanceToOpponent )
{
  const auto opponentSpeedPercent =
    opponent.speed() / constants::plane::maxSpeedBoosted;

  return
    std::abs(dirToOpponentRelative) >= 45.f &&
    opponentSpeedPercent < 0.5f &&
    shortestDistanceToOpponent < 0.25f;
}

static bool
isAboutToCrash(
  const Plane& self )
{
  namespace plane = constants::plane;
  namespace pilot = constants::pilot;
  namespace barn = constants::barn;


  const float dir = self.dir() * M_PI / 180.f;

  const auto speed = 0.5f * std::clamp(
    self.speed() * constants::tickRate,
    plane::maxSpeedBase,
    plane::maxSpeedBoosted );

  const SDL_Vector probeStart {self.x(), self.y()};

  const SDL_Vector probeEnd
  {
    probeStart.x + speed * std::sin(dir),
    probeStart.y - speed * std::cos(dir),
  };

  SDL_Vector closestContact {};

  const SDL_Vector groundSegment[2]
  {
    {-10.f, plane::groundCollision},
    {10.f, plane::groundCollision},
  };

  const bool collidesWithGround = segment_intersects_segment(
    probeStart, probeEnd,
    groundSegment[0], groundSegment[1],
    &closestContact );

  if ( collidesWithGround == true )
  {
    const auto contactDistance =
      (closestContact - probeStart).length();

    if ( contactDistance <= plane::hitboxDiameter )
      return true;
  }


  const std::vector <SDL_Vector> barnBox
  {
    {barn::planeCollisionX, plane::groundCollision},
    {barn::planeCollisionX, barn::planeCollisionY},
    {barn::planeCollisionX + barn::sizeX, barn::planeCollisionY},
    {barn::planeCollisionX + barn::sizeX, plane::groundCollision},
  };

  const bool collidesWithBarn = segment_intersects_polygon(
    probeStart, probeEnd,
    barnBox,
    &closestContact );

  if ( collidesWithBarn == true )
  {
    const auto contactDistance =
      (closestContact - probeStart).length();

    if ( contactDistance <= plane::hitboxDiameter )
      return true;
  }

  return false;
}

static bool
isSafeToJump(
  const Plane& self )
{
  namespace plane = constants::plane;
  namespace pilot = constants::pilot;

  const float jumpDir = self.jumpDir() * M_PI / 180.f;

  const SDL_Vector probeStart {self.x(), self.y()};

  const SDL_Vector probeEnd
  {
    probeStart.x,
    probeStart.y - 0.5f * pilot::ejectSpeed * std::cos(jumpDir),
  };


  const SDL_Vector groundSegment[2]
  {
    {-10.f, plane::groundCollision},
    {10.f, plane::groundCollision},
  };

  const bool collidesWithGround = segment_intersects_segment(
    probeStart, probeEnd,
    groundSegment[0], groundSegment[1],
    {} );

  return collidesWithGround == false;
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

void
ContextMap::reinit()
{
  std::fill(mValues.begin(), mValues.end(), 0.f);
}

void
ContextMap::normalize()
{
  const auto max = maxValue();

  for ( size_t i {}; i < mValues.size(); ++i )
    mValues[i] /= max;
}

size_t
ContextMap::size() const
{
  return mValues.size();
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

float
ContextMap::sum(
  const size_t firstSlot,
  const size_t lastSlot,
  const int8_t dir ) const
{
  assert(dir != 0);
  assert(firstSlot < mValues.size());
  assert(lastSlot < mValues.size());

  float cost {};

  for ( size_t slot {firstSlot}; slot != lastSlot; )
  {
    cost += mValues[slot];

    if ( slot == 0 && dir < 0 )
      slot = mValues.size() - 1;

    else if ( slot == mValues.size() - 1 && dir > 0 )
      slot = 0;

    else
      slot += dir;
  }

  return cost;
}

size_t
ContextMap::countSlotDistance(
  const size_t firstSlot,
  const size_t lastSlot,
  const int8_t dir ) const
{
  assert(dir != 0);
  assert(firstSlot < mValues.size());
  assert(lastSlot < mValues.size());

  size_t steps {};

  for ( size_t slot {firstSlot}; slot != lastSlot; )
  {
    if ( slot == 0 && dir < 0 )
      slot = mValues.size() - 1;

    else if ( slot == mValues.size() - 1 && dir > 0 )
      slot = 0;

    else
      slot += dir;

    ++steps;
  }

  return steps;
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

  if ( std::abs(valueDiff) < std::numeric_limits <float>::epsilon() )
    return;


//  static std::minstd_rand0 eng {std::random_device{}()};
//  static std::mt19937 eng {std::random_device{}()};
//  static std::uniform_real_distribution <float> distr {0.25f, 1.f};

//  const auto randomFactor = distr(eng);
  const auto randomFactor = 1.f;

  mValue += factor * weight * randomFactor * std::copysign(1.f, valueDiff);
  mValue = std::clamp(mValue, 0.f, 1.f);
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

struct AiOscillationFixer
{
  AiAction mPreviousDirection {};
  size_t mSwitchedDirectionsCounter {};


  bool fixOscillation( AiAction& newDirection, const size_t maxDirectionSwitchesAllowed );
};

bool
AiOscillationFixer::fixOscillation(
  AiAction& newDirection,
  const size_t maxDirectionSwitchesAllowed )
{
  bool result {};

  switch (newDirection)
  {
    case AiAction::TurnLeft:
    {
      if ( mPreviousDirection == AiAction::TurnRight )
      {
        if ( ++mSwitchedDirectionsCounter > maxDirectionSwitchesAllowed )
        {
          mSwitchedDirectionsCounter = 0;

          newDirection = mPreviousDirection;
          result = true;
        }
      }
      else
        mSwitchedDirectionsCounter = 0;

      mPreviousDirection = newDirection;

      break;
    }

    case AiAction::TurnRight:
    {
      if ( mPreviousDirection == AiAction::TurnLeft )
      {
        if ( ++mSwitchedDirectionsCounter > maxDirectionSwitchesAllowed )
        {
          mSwitchedDirectionsCounter = 0;

          newDirection = mPreviousDirection;
          result = true;
        }
      }
      else
        mSwitchedDirectionsCounter = 0;

      mPreviousDirection = newDirection;

      break;
    }

    default:
    {
      mPreviousDirection = AiAction::Idle;
      mSwitchedDirectionsCounter = 0;

      break;
    }
  }


  return result;
}


class AiStatePlane : public AiState
{
protected:
  AiOscillationFixer mOscillationFixer {};


public:
  AiStatePlane( const AiTemperature& temperature );

  void update(
    const Plane& self,
    const Plane& opponent,
    const std::vector <Bullet>& opponentBullets ) override;

  std::vector <AiAction> actions() const override;
};

AiStatePlane::AiStatePlane(
  const AiTemperature& temperature )
  : AiState(temperature)
{
  mInterestMap = {constants::plane::directionCount};
  mDangerMap = {constants::plane::directionCount};

  constexpr auto ReactionTimeToWeights = &AiTemperature::Weights::FromTime;

  auto throttleWeight = ReactionTimeToWeights(0.1f, 0.1f);
  auto pitchWeight = ReactionTimeToWeights(0.1f, 0.1f);
  auto shootWeight = ReactionTimeToWeights(0.04f, 0.04f);
  auto jumpWeight = ReactionTimeToWeights(0.04f, 0.04f);

  switch (gameState().botDifficulty)
  {
    case DIFFICULTY::EASY:
    {
      throttleWeight = ReactionTimeToWeights(0.35f, 0.35f);
      pitchWeight = ReactionTimeToWeights(0.45f, 0.45f);
      shootWeight = ReactionTimeToWeights(0.45f, 0.45f);
      jumpWeight = ReactionTimeToWeights(0.1f, 0.1f);

      break;
    }
    case DIFFICULTY::MEDIUM:
    {
      throttleWeight = ReactionTimeToWeights(0.25f, 0.25f);
      pitchWeight = ReactionTimeToWeights(0.35f, 0.35f);
      shootWeight = ReactionTimeToWeights(0.35f, 0.35f);
      jumpWeight = ReactionTimeToWeights(0.08f, 0.08f);

      break;
    }
    case DIFFICULTY::HARD:
    {
      throttleWeight = ReactionTimeToWeights(0.17f, 0.17f);
      pitchWeight = ReactionTimeToWeights(0.25f, 0.25f);
      shootWeight = ReactionTimeToWeights(0.2f, 0.2f);
      jumpWeight = ReactionTimeToWeights(0.06f, 0.06f);

      break;
    }

    case DIFFICULTY::DEVELOPER:
      break;

    case DIFFICULTY::INSANE:
    {
      throttleWeight = ReactionTimeToWeights(0.05f, 0.05f);
      pitchWeight = ReactionTimeToWeights(0.05f, 0.05f);
      shootWeight = ReactionTimeToWeights(0.02f, 0.02f);
      jumpWeight = ReactionTimeToWeights(0.02f, 0.02f);

      break;
    }

    default:
      assert(false);
  }

  mActions =
  {
    {AiAction::Accelerate, throttleWeight},
    {AiAction::Decelerate, throttleWeight},
    {AiAction::TurnLeft, pitchWeight},
    {AiAction::TurnRight, pitchWeight},
    {AiAction::Shoot, shootWeight},
    {AiAction::Jump, jumpWeight},
  };
}

void
AiStatePlane::update(
  const Plane& self,
  const Plane& opponent,
  const std::vector <Bullet>& opponentBullets )
{
  namespace barn = constants::barn;
  namespace pilot = constants::pilot;
  namespace plane = constants::plane;


  if ( self.hasJumped() == true || self.isDead() == true )
  {
    mTemperature.update(0.f);

    for ( auto& [action, temperature] : mActions )
      temperature.set(0.f);

    return;
  }

  const auto botDifficulty = gameState().botDifficulty;

  mTemperature.update(1.f);

  mInterestMap.reinit();
  mDangerMap.reinit();

  const auto speed = 0.5f * std::clamp(
    self.speed() * constants::tickRate,
    plane::maxSpeedBase,
    plane::maxSpeedBoosted );


  for ( size_t i {}; i < plane::directionCount; ++i )
  {
    const float dir = (i * plane::pitchStep) * M_PI / 180.f;

    const SDL_Vector probeStart {self.x(), self.y()};

    const SDL_Vector probeEnd
    {
      probeStart.x + speed * std::sin(dir),
      probeStart.y - speed * std::cos(dir),
    };

    {
      const SDL_Vector groundSegment[2]
      {
        {-10.f, plane::groundCollision},
        {10.f, plane::groundCollision},
      };

      SDL_Vector groundContactPoint {};

      const bool collidesWithGround = segment_intersects_segment(
        probeStart, probeEnd,
        groundSegment[0], groundSegment[1],
        &groundContactPoint );

      if ( collidesWithGround == true )
      {
        const float distance = (groundContactPoint - probeStart).length();

        const float danger = distance;

        mDangerMap.write(i, danger);
      }
    }

    {
      const std::vector <SDL_Vector> barnBox
      {
        {barn::planeCollisionX, plane::groundCollision},
        {barn::planeCollisionX, barn::planeCollisionY},
        {barn::planeCollisionX + barn::sizeX, barn::planeCollisionY},
        {barn::planeCollisionX + barn::sizeX, plane::groundCollision},
      };

      SDL_Vector barnContactPoint {};

      const bool collidesWithBarn = segment_intersects_polygon(
        probeStart, probeEnd,
        barnBox,
        &barnContactPoint );

      if ( collidesWithBarn == true )
      {
        const auto distance = (barnContactPoint - probeStart).length();

        const float danger = distance;

        if ( mDangerMap[i] > 0.f )
          mDangerMap.write(i, std::min(mDangerMap[i], danger));
        else
          mDangerMap.write(i, danger);
      }
    }
  }


  for ( auto& bullet : opponentBullets )
  {
    const SDL_Vector selfPathStart
    {
      self.x(),
      self.y(),
    };

    const SDL_Vector selfPathEnd
    {
      self.x() + self.speedVector().x * constants::tickRate,
      self.y() + self.speedVector().y * constants::tickRate,
    };

//    TODO: zigzag polygon between plane current & future hitboxes

//    line1 = bottomLeft -> bottomRight
//    line2 = bottomRight -> topRight
//    line3 = topRight -> topLeft
//    line4 = topLeft -> bottomLeft
//    line5 = bottomLeft -> topRight
//    line6 = bottomRight -> topLeft

    const float bulletDirLeft = (bullet.dir() - 90.f) * M_PI / 180.f;
    const float bulletDirRight = (bullet.dir() + 90.f) * M_PI / 180.f;
    const float bulletDir = bullet.dir() * M_PI / 180.f;

    const SDL_Vector bulletPathLeftStart
    {
      bullet.x() + plane::hitboxRadius * std::sin(bulletDirLeft),
      bullet.y() - plane::hitboxRadius * std::cos(bulletDirLeft),
    };

    const SDL_Vector bulletPathLeftEnd
    {
      bulletPathLeftStart.x + constants::bullet::speed * std::sin(bulletDir),
      bulletPathLeftStart.y - constants::bullet::speed * std::cos(bulletDir),
    };

    const SDL_Vector bulletPathRightStart
    {
      bullet.x() + plane::hitboxRadius * std::sin(bulletDirRight),
      bullet.y() - plane::hitboxRadius * std::cos(bulletDirRight),
    };

    const SDL_Vector bulletPathRightEnd
    {
      bulletPathRightStart.x + constants::bullet::speed * std::sin(bulletDir),
      bulletPathRightStart.y - constants::bullet::speed * std::cos(bulletDir),
    };

    SDL_Vector contactPoint {};

    bool willCollideWithBullet =
      segment_intersects_segment(
        selfPathStart, selfPathEnd,
        bulletPathLeftStart, bulletPathLeftEnd,
        &contactPoint );

    willCollideWithBullet |= segment_intersects_segment(
      selfPathStart, selfPathEnd,
      bulletPathRightStart, bulletPathRightEnd,
      &contactPoint );

    if ( willCollideWithBullet == false )
      continue;

    const auto dirToContactAbsolute = get_angle_to_point(
      selfPathStart, contactPoint );

    const auto distanceToContact = get_distance_between_points(
      selfPathStart, contactPoint );

    const size_t dirToContactIndex =
      angleToPitchIndex(dirToContactAbsolute);

    const auto dirToContactClamped = dirToContactIndex % mDangerMap.size();

    if ( mDangerMap[dirToContactClamped] > 0.f )
    {
      mDangerMap[dirToContactClamped] = std::min(
        mDangerMap[dirToContactClamped],
        distanceToContact );
    }
    else
      mDangerMap.write(dirToContactClamped, distanceToContact);
  }


  std::vector <AiAction> actions {};

  const SDL_Vector pos {self.x(), self.y()};
  const SDL_Vector opponentPos {opponent.pilot.x(), opponent.pilot.y()};

  const SDL_Vector opponentPosL = opponentPos - SDL_Vector{1.f, 0.f};
  const SDL_Vector opponentPosR = opponentPos + SDL_Vector{1.f, 0.f};

  const auto opponentDistance  = get_distance_between_points(pos, opponentPos);
  const auto opponentDistanceL = get_distance_between_points(pos, opponentPosL);
  const auto opponentDistanceR = get_distance_between_points(pos, opponentPosR);


  std::multimap <float, SDL_Vector> opponentPositionsSorted
  {
    {opponentDistance, opponentPos},
    {opponentDistanceL, opponentPosL},
    {opponentDistanceR, opponentPosR},
  };
  opponentPositionsSorted.erase(--opponentPositionsSorted.end());


  const auto [opponentShortestDistance, opponentShortestPos] =
    *opponentPositionsSorted.begin();

  const auto dirToOpponentAbsolute = get_angle_to_point(
    pos, opponentShortestPos );

  const auto dirToOpponentRelative = get_angle_relative(
    self.dir(), dirToOpponentAbsolute );

  const auto isOpponentBehind = isOpponentCloseBehind(
    opponent,
    dirToOpponentRelative,
    opponentShortestDistance );


  const bool isStalling = isPlaneStalling(self);
  const auto isCrashing = isAboutToCrash(self);

  if ( isOpponentBehind == true && isStalling == true )
    opponentPositionsSorted.erase(opponentPositionsSorted.begin());


  if ( opponent.isDead() == false )
    for ( const auto& [distance, position] : opponentPositionsSorted )
    {
      const auto dirToTargetAbsolute = get_angle_to_point(
        pos, position );

      const auto dirToTargetRelative = get_angle_relative(
        self.dir(), dirToTargetAbsolute );

      const auto bulletOffset = self.bulletSpawnOffset();

      const float aimCone =
        botDifficulty == DIFFICULTY::EASY
          ? constants::ai::aimConeEasy
          : constants::ai::aimConeDefault;

      const bool canHitInstantly = opponent.isHit(
        self.x() + bulletOffset.x,
        self.y() + bulletOffset.y ) &&
          botDifficulty > DIFFICULTY::EASY;

      const bool willBulletHit =
        std::abs(dirToTargetRelative) <= plane::pitchStep * aimCone;

      if ( willBulletHit == true || canHitInstantly == true )
      {
        if ( opponent.hasJumped() == false || botDifficulty > DIFFICULTY::EASY )
          actions.push_back(AiAction::Shoot);
      }

      const size_t dirIndex = std::round(dirToTargetAbsolute / plane::pitchStep);

      if ( isOpponentBehind == true &&
           opponent.y() > self.y() &&
           opponent.speed() < plane::maxSpeedBase )
      {
        mInterestMap[5] = 0.01f;
        mInterestMap[11] = 0.01f;

        continue;
      }

      mInterestMap[dirIndex % mInterestMap.size()] = distance;
    }


  const auto maxDanger = SDL_Vector{plane::maxSpeedBoosted, plane::maxSpeedBoosted}.length();
  const auto maxInterest = SDL_Vector{2.f, pilot::groundCollision}.length();

  for ( size_t i {}; i < mDangerMap.size(); ++i )
  {
    auto& danger = mDangerMap[i];

    if ( danger != 0.f )
      danger = 1.f - danger / maxDanger;

    auto& interest = mInterestMap[i];

    if ( interest != 0.f )
      interest = 1.f - interest / maxInterest;
  }


  if ( isStalling == true )
  {
    mDangerMap[15] += 0.9f;
    mDangerMap[0] += 1.0f;
    mDangerMap[1] += 0.9f;
  }


  const bool shouldClimb =
    opponent.isDead() == true ||
    opponent.protectionRemainder() >= 0.5f * plane::spawnProtectionCooldown;

  const bool shouldRescue =
    botDifficulty > DIFFICULTY::EASY &&
    self.hp() < plane::maxHp &&
    (opponent.isDead() == true || opponent.hasJumped() == true);

//  Climb during opponent's death or spawn protection
  if ( shouldClimb == true && shouldRescue == false )
  {
    mInterestMap.reinit();

    mInterestMap[14] = 1.f;
    mInterestMap[15] = 0.9f;
    mInterestMap[1] = 0.9f;
    mInterestMap[2] = 1.f;
  }

//  Respawn if opponent is dead or out of plane
  else if ( shouldRescue == true )
  {
    mInterestMap.reinit();

    const SDL_Vector barnInterestLeft
    {
      0.5f - barn::sizeX,
      plane::groundCollision,
    };

    const SDL_Vector barnInterestRight
    {
      0.5f + barn::sizeX,
      plane::groundCollision,
    };

    const float angleToBarnLeft = get_angle_to_point(
      {self.x(), self.y()},
      barnInterestLeft );

    const float angleToBarnRight = get_angle_to_point(
      {self.x(), self.y()},
      barnInterestRight );

    const float distanceToBarnLeft = get_distance_between_points(
      {self.x(), self.y()},
      barnInterestLeft );

    const float distanceToBarnRight = get_distance_between_points(
      {self.x(), self.y()},
      barnInterestRight );

    const float maxBarnDistance =
      SDL_Vector {1.f, plane::groundCollision}.length();

    mInterestMap[angleToPitchIndex(angleToBarnLeft) % mInterestMap.size()] =
      1.f - distanceToBarnLeft / maxBarnDistance;

    mInterestMap[angleToPitchIndex(angleToBarnRight) % mInterestMap.size()] =
      1.f - distanceToBarnRight / maxBarnDistance;
  }


//  Avoid opponent's line of fire
  const auto opponentDistanceFactor = opponentShortestDistance / plane::maxSpeedBase;

  if (  opponent.isDead() == false &&
        opponent.hasJumped() == false &&
        opponentDistanceFactor < 1.f &&
        std::abs(dirToOpponentRelative) >= 7.f * plane::pitchStep )
  {
    const auto dirToOpponentInverted = std::fmod(
      dirToOpponentAbsolute + 180.f, 360.f );

    const size_t dirToOpponentInvertedIndex =
      angleToPitchIndex(dirToOpponentInverted);

    mDangerMap[dirToOpponentInvertedIndex % mDangerMap.size()] +=
      0.5f * (1.f - opponentDistanceFactor);
  }


//  const auto filteredMap = mInterestMap - mDangerMap;
  auto filteredMap = mInterestMap.mask(mDangerMap, 0.7f);
//  filteredMap = filteredMap - mDangerMap;

  const size_t selfDirIndex = angleToPitchIndex(self.dir());

  const auto maxInterestSlot = filteredMap.maxValueSlot();

  const auto maxInterestDir = maxInterestSlot * plane::pitchStep;

  const auto interestOpponentDirDiff = get_angle_relative(
    maxInterestDir, dirToOpponentAbsolute );

  const float threatThreshold {0.9f};
  const auto currentThreat = mDangerMap[selfDirIndex % mDangerMap.size()];

  if ( selfDirIndex == maxInterestSlot && currentThreat < threatThreshold )
  {
    if (  isStalling == true ||
          opponentShortestDistance > 0.25f ||
          self.speed() <= opponent.speed() ||
          std::abs(interestOpponentDirDiff) >= 45.f )
      actions.push_back(AiAction::Accelerate);
    else
      actions.push_back(AiAction::Decelerate);
  }
  else
  {
    std::multimap <float, size_t> sortedInterestDirs {};

    for ( size_t i {}; i < filteredMap.size(); ++i )
      sortedInterestDirs.insert({filteredMap[i], i});

    while ( sortedInterestDirs.size() > 2 )
      sortedInterestDirs.erase(sortedInterestDirs.begin());

    const auto pathStartLeft = std::clamp(
      selfDirIndex - size_t{1},
      size_t{},
      mDangerMap.size() - size_t{1});

    const auto pathStartRight = (selfDirIndex + 1) % mDangerMap.size();

    float lowestDangerSumLeft {1000.f};
    float lowestDangerSumRight {1000.f};
    float highestDangerSumLeft {0.f};
    float highestDangerSumRight {0.f};

    for ( auto&& [interest, slot] : sortedInterestDirs )
    {
      if ( slot == selfDirIndex )
        continue;

      const auto slotStepsLeft = mDangerMap.countSlotDistance(selfDirIndex, slot, -1);
      const auto slotStepsRight = mDangerMap.countSlotDistance(selfDirIndex, slot, +1);

      const auto dangerSumLeft = mDangerMap.sum(pathStartLeft, slot, -1) / slotStepsLeft;
      const auto dangerSumRight = mDangerMap.sum(pathStartRight, slot, +1) / slotStepsRight;

      lowestDangerSumLeft = std::min(lowestDangerSumLeft, dangerSumLeft);
      lowestDangerSumRight = std::min(lowestDangerSumRight, dangerSumRight);

      highestDangerSumLeft = std::max(highestDangerSumLeft, dangerSumLeft);
      highestDangerSumRight = std::max(highestDangerSumRight, dangerSumRight);
    }

    if ( std::abs(lowestDangerSumLeft - lowestDangerSumRight) < 0.1f )
    {
      const auto dirToInterestRelative = get_angle_relative(
        self.dir(),
        maxInterestSlot * plane::pitchStep );

      if ( dirToInterestRelative == 180.f || dirToInterestRelative == -180.f )
      {
        if ( highestDangerSumRight > highestDangerSumLeft )
          actions.push_back(AiAction::TurnLeft);
        else
          actions.push_back(AiAction::TurnRight);
      }
      else
      if ( dirToInterestRelative > 0 )
        actions.push_back(AiAction::TurnRight);
      else
        actions.push_back(AiAction::TurnLeft);
    }

    else if ( lowestDangerSumLeft < lowestDangerSumRight )
      actions.push_back(AiAction::TurnLeft);

    else if ( lowestDangerSumLeft > lowestDangerSumRight )
      actions.push_back(AiAction::TurnRight);


//    TODO: decelerate to get on opponent's tail
    if (  isStalling == true ||
          isOpponentBehind == false ||
          std::abs(interestOpponentDirDiff) >= 45.f )
      actions.push_back(AiAction::Accelerate);
    else
      actions.push_back(AiAction::Decelerate);
  }


//  Eject to avoid crash
  if ( isSafeToJump(self) == true )
  {
    const float selfHp =
      gameState().features.oneShotKills == true
        ? 0.0f
        : static_cast <float> (self.hp()) /
          constants::plane::maxHp;

    const bool facesGround =
      self.jumpDir() >= 90.f && self.jumpDir() <= 270.f;


    if ( isCrashing == true && botDifficulty > DIFFICULTY::MEDIUM )
      actions.push_back(AiAction::Jump);

//    Eject if no danger is present
    else if ( shouldRescue == true )
    {
      const SDL_Vector barnPos
      {
        0.5f,
        pilot::groundCollision,
      };

      const auto distanceToBarn = get_distance_between_points(
        pos, barnPos );

      const auto angleToBarn = get_angle_to_point(
        pos, barnPos );

      const bool closeToBarn = distanceToBarn < 0.5f * pilot::ejectSpeed;

      const bool facesBarn =
        std::abs(self.jumpDir() - angleToBarn) <= 3.f * plane::pitchStep;

      switch (botDifficulty)
      {
        case DIFFICULTY::EASY:
          break;

        case DIFFICULTY::MEDIUM:
        {
          actions.push_back(AiAction::Jump);
          break;
        }

        case DIFFICULTY::HARD:
        {
          if ( closeToBarn == true )
            actions.push_back(AiAction::Jump);

          break;
        }

        case DIFFICULTY::DEVELOPER:
        case DIFFICULTY::INSANE:
        {
          if ( facesBarn == true || facesGround == true )
            actions.push_back(AiAction::Jump);

          break;
        }
      }
    }

//    Eject during combat
    else if ( selfHp == 0.f && botDifficulty > DIFFICULTY::EASY &&
              botDifficulty != DIFFICULTY::INSANE )
    {
      bool allowJump =
        gameState().features.oneShotKills == false;

      for ( const auto& bullet : opponentBullets )
      {
        const float bulletDir = bullet.dir() * M_PI / 180.0;

        const auto bulletPathLength =
          0.5f * constants::bullet::speed;

        const SDL_Vector bulletPathStart
        {
          bullet.x(),
          bullet.y(),
        };

        const SDL_Vector bulletPathEnd
        {
          bullet.x() + bulletPathLength * std::sin(bulletDir),
          bullet.y() - bulletPathLength * std::cos(bulletDir),
        };

        const bool willCollide = segment_intersects_circle(
          bulletPathStart, bulletPathEnd,
          pos, 0.5f * pilot::ejectSpeed ); // or plane::hitboxRadius ?

        if ( willCollide == false )
          continue;


        const auto angleToBullet = get_angle_to_point(
          pos, bulletPathStart );

        if ( std::abs(angleToBullet - self.jumpDir()) <= 3.f * plane::pitchStep )
          allowJump = false;
      }

      const SDL_Vector barnPos
      {
        0.5f,
        pilot::groundCollision,
      };

      const auto distanceToBarn = get_distance_between_points(
        pos, barnPos );

      const auto angleToBarn = get_angle_to_point(
        pos, barnPos );

      const bool closeToBarn = distanceToBarn < 0.75f * pilot::ejectSpeed;

      const bool facesBarn =
        std::abs(self.jumpDir() - angleToBarn) <= 2.f * plane::pitchStep;


      if ( closeToBarn == false && facesGround == false )
        allowJump = false;

      if (  self.hp() == opponent.hp() && facesBarn == false )
        allowJump = false;

      if ( allowJump == true )
      {
        switch (botDifficulty)
        {
          case DIFFICULTY::EASY:
          case DIFFICULTY::MEDIUM:
            break;

          case DIFFICULTY::HARD:
          {
            if ( closeToBarn == true )
              actions.push_back(AiAction::Jump);

            break;
          }

          case DIFFICULTY::DEVELOPER:
          {
            if ( facesBarn == true || facesGround == true )
              actions.push_back(AiAction::Jump);

            break;
          }
        }
      }
    }
  }


  std::sort(actions.begin(), actions.end());
  actions.erase(std::unique(actions.begin(), actions.end()), actions.end());


  if ( false && self.canTurn() == true && self.type() == PLANE_TYPE::RED )
  {
    auto wantsLeftTurn = std::find(
      actions.begin(), actions.end(),
      AiAction::TurnLeft );

    if ( wantsLeftTurn != actions.end() )
    {
      if ( mOscillationFixer.fixOscillation(*wantsLeftTurn, 2) )
        mActions[AiAction::TurnLeft].set(0.f);
    }


    auto wantsRightTurn = std::find(
      actions.begin(), actions.end(),
      AiAction::TurnRight );

    if ( wantsRightTurn != actions.end() )
    {
      if ( mOscillationFixer.fixOscillation(*wantsRightTurn, 2) )
        mActions[AiAction::TurnRight].set(0.f);
    }


    if ( wantsLeftTurn == actions.end() &&
         wantsRightTurn == actions.end() )
    {
      auto dummyAction = AiAction::Idle;
      mOscillationFixer.fixOscillation(dummyAction, 2);
    }
  }


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
AiStatePlane::actions() const
{
  const float threshold = 0.95f;

  std::vector <AiAction> actions {};

  for ( auto& [action, temperature] : mActions )
    if ( temperature >= threshold )
      actions.push_back(action);

  return actions;
}


class AiStatePilot : public AiState
{
public:
  AiStatePilot( const AiTemperature& temperature );

  void update(
    const Plane& self,
    const Plane& opponent,
    const std::vector <Bullet>& opponentBullets ) override;

  std::vector <AiAction> actions() const override;
};

AiStatePilot::AiStatePilot(
  const AiTemperature& temperature )
  : AiState(temperature)
{
  mInterestMap = {2};
  mDangerMap = {2};

  constexpr auto ReactionTimeToWeights = &AiTemperature::Weights::FromTime;

  auto movementWeight = ReactionTimeToWeights(0.02f, 0.02f);
  auto chuteWeight = ReactionTimeToWeights(0.004f, 0.004f);

  switch (gameState().botDifficulty)
  {
    case DIFFICULTY::EASY:
    case DIFFICULTY::MEDIUM:
    {
      movementWeight = ReactionTimeToWeights(0.2f, 0.2f);
      break;
    }
    case DIFFICULTY::HARD:
    {
      movementWeight = ReactionTimeToWeights(0.16f, 0.16f);
      break;
    }

    case DIFFICULTY::DEVELOPER:
      break;

    case DIFFICULTY::INSANE:
      break;

    default:
      assert(false);
  }

  mActions =
  {
    {AiAction::TurnLeft, movementWeight},
    {AiAction::TurnRight, movementWeight},
    {AiAction::Jump, chuteWeight},
  };
}

void
AiStatePilot::update(
  const Plane& self,
  const Plane& opponent,
  const std::vector <Bullet>& opponentBullets )
{
  namespace pilot = constants::pilot;
  namespace chute = pilot::chute;


  if ( self.hasJumped() == false || self.isDead() == true )
  {
    mTemperature.update(0.f);

    for ( auto& [action, temperature] : mActions )
      temperature.set(0.f);

    return;
  }

  const auto botDifficulty = gameState().botDifficulty;

  mTemperature.update(1.f);

  mInterestMap.reinit();
  mDangerMap.reinit();

//  Run to rescue zone
  if ( self.pilot.x() > constants::barn::pilotCollisionRightX )
    mInterestMap[0] = 1.0f;

  else if ( self.pilot.x() < constants::barn::pilotCollisionLeftX )
    mInterestMap[1] = 1.0f;


  const SDL_Vector pilotPos
  {
    self.pilot.x(),
    self.pilot.y(),
  };

  const auto pilotSpeed = self.pilot.speedVec();

  const auto timeToAvoidBullet =
    pilot::sizeX / pilot::runSpeed;

  const auto runDistance = std::abs(pilotSpeed.x) * constants::tickRate * timeToAvoidBullet;

//  const std::vector <SDL_Vector> pilotHitbox
//  {
//    {pilotPos.x - 0.5f * pilot::sizeX + runDistance * (runDistance < 0.f), pilotPos.y + 0.5f * pilot::sizeY},
//    {pilotPos.x - 0.5f * pilot::sizeX + runDistance * (runDistance < 0.f), pilotPos.y - 0.5f * pilot::sizeY},
//    {pilotPos.x + 0.5f * pilot::sizeX + runDistance * (runDistance > 0.f), pilotPos.y - 0.5f * pilot::sizeY},
//    {pilotPos.x + 0.5f * pilot::sizeX + runDistance * (runDistance > 0.f), pilotPos.y + 0.5f * pilot::sizeY},
//  };

  const std::vector <SDL_Vector> pilotHitbox
  {
    {pilotPos.x - 0.5f * pilot::sizeX - runDistance, pilotPos.y + 0.5f * pilot::sizeY},
    {pilotPos.x - 0.5f * pilot::sizeX - runDistance, pilotPos.y - 0.5f * pilot::sizeY},
    {pilotPos.x + 0.5f * pilot::sizeX + runDistance, pilotPos.y - 0.5f * pilot::sizeY},
    {pilotPos.x + 0.5f * pilot::sizeX + runDistance, pilotPos.y + 0.5f * pilot::sizeY},
  };

//  Avoid bullets
  if ( botDifficulty > DIFFICULTY::MEDIUM )
    for ( const auto& bullet : opponentBullets )
    {
      const float bulletDir = bullet.dir() * M_PI / 180.f;
      const auto bulletSpeed = constants::bullet::speed;

      const SDL_Vector bulletPathStart
      {
        bullet.x(),
        bullet.y(),
      };

      const SDL_Vector bulletPathEnd
      {
        bullet.x() + bulletSpeed * timeToAvoidBullet * std::sin(bulletDir),
        bullet.y() - bulletSpeed * timeToAvoidBullet * std::cos(bulletDir),
      };

      SDL_Vector contactPoint {};

      const bool willCollide = segment_intersects_polygon(
        bulletPathStart, bulletPathEnd,
        pilotHitbox,
        &contactPoint );

      if ( willCollide == false )
        continue;


      const auto distanceToCollision =
        (contactPoint - pilotPos).length();

      const auto angleToCollision = get_angle_relative(
        0.f, get_angle_to_point(pilotPos, contactPoint) );

      const bool dirIndex = angleToCollision > 0;

      const auto danger = distanceToCollision;

      if ( mDangerMap[dirIndex] > 0.f )
      {
        mDangerMap[dirIndex] = std::min(
          mDangerMap[dirIndex], danger );
      }
      else
        mDangerMap.write(dirIndex, danger);
    }

//  Avoid opponent's LoS
  if ( opponent.canShoot() == true && botDifficulty > DIFFICULTY::HARD )
  {
    const float bulletDir = opponent.dir() * M_PI / 180.f;
    const auto bulletSpeed = constants::bullet::speed;

    const auto bulletOffset = opponent.bulletSpawnOffset();

    const SDL_Vector bulletPathStart
    {
      opponent.x() + bulletOffset.x,
      opponent.y() + bulletOffset.y,
    };

    const SDL_Vector bulletPathEnd
    {
      bulletPathStart.x + bulletSpeed * timeToAvoidBullet * std::sin(bulletDir),
      bulletPathStart.y - bulletSpeed * timeToAvoidBullet * std::cos(bulletDir),
    };

    SDL_Vector contactPoint {};

    const bool willCollide = segment_intersects_polygon(
      bulletPathStart, bulletPathEnd,
      pilotHitbox,
      &contactPoint );

    if ( willCollide == true )
    {
      const auto distanceToCollision =
        (contactPoint - pilotPos).length();

      const auto angleToCollision = get_angle_relative(
        0.f, get_angle_to_point(pilotPos, contactPoint) );

      const bool dirIndex = angleToCollision > 0;

      const auto danger = distanceToCollision;

      if ( mDangerMap[dirIndex] > 0.f )
      {
        mDangerMap[dirIndex] = std::min(
          mDangerMap[dirIndex], danger );
      }
      else
        mDangerMap.write(dirIndex, danger);
    }
  }

//  const auto maxDanger = mDangerMap.maxValue();
  const auto maxDanger = SDL_Vector
  {
     0.5f * pilot::sizeX + runDistance,
     0.5f * pilot::sizeY
  }.length();

  for ( size_t i {}; i < mDangerMap.size(); ++i )
  {
    auto& danger = mDangerMap[i];

    if ( danger != 0.f )
      danger = 1.f - danger / maxDanger;
  }


  std::vector <AiAction> actions {};

  const auto filteredMap = mInterestMap - mDangerMap;

  if ( mDangerMap[0] < mDangerMap[1] )
    actions.push_back(AiAction::TurnLeft);

  else if ( mDangerMap[0] > mDangerMap[1] )
    actions.push_back(AiAction::TurnRight);

  else if ( filteredMap[0] > filteredMap[1] )
    actions.push_back(AiAction::TurnLeft);

  else if ( filteredMap[0] < filteredMap[1] )
    actions.push_back(AiAction::TurnRight);


  if ( self.pilot.isRunning() == false )
  {
    const auto timeToSlowdown =
      std::abs(pilotSpeed.y * constants::tickRate + pilot::gravity - chute::baseSpeedY)
      / chute::speedYSlowdownFactor;

    const auto ticksToSlowdown =
      timeToSlowdown * constants::tickRate;

    const auto timeToLand =
      (pilot::groundCollision - pilotPos.y)
      / chute::baseSpeedY;

    if ( pilotSpeed.y * constants::tickRate > chute::baseSpeedY )
    {
      const auto slowdownDistance =
        (pilotSpeed.y * ticksToSlowdown - 0.5f * chute::speedYSlowdownFactor * timeToSlowdown) * timeToSlowdown;

      if ( pilotPos.y + slowdownDistance + chute::sizeY >= pilot::groundCollision )
        actions.push_back(AiAction::Jump);
    }
    else if ( timeToLand < 1.f )
      actions.push_back(AiAction::Jump);
  }


  std::sort(actions.begin(), actions.end());
  actions.erase(std::unique(actions.begin(), actions.end()), actions.end());

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
AiStatePilot::actions() const
{
  const float threshold = 0.7f;

  std::vector <AiAction> actions {};

  for ( auto& [action, temperature] : mActions )
    if ( temperature >= threshold )
      actions.push_back(action);

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
    if ( plane.isBot() == false && gameState().debug.ai == false )
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

    if ( plane.isBot() == false )
      continue;


    const auto aiActions =
      stateController.currentState()->actions();

    for ( const auto aiAction : aiActions )
      plane.input.ExecuteAiAction(aiAction);
  }
}

void
AiController::drawDebugLayer() const
{
  for ( auto& [planeType, plane] : planes )
  {
    auto& stateController = mStateController.at(plane.type());

    if ( plane.isDead() == false )
      stateController.drawDebugLayer(plane);
  }
}


AiStateController::~AiStateController()
{
  for ( const auto state : mStates )
    if ( state != nullptr )
      delete state;
}

void
AiStateController::init()
{
  for ( const auto state : mStates )
    if ( state != nullptr )
      delete state;

  mStates =
  {
    new AiStatePlane({{1.f, 1.f}, 1.f}),
    new AiStatePilot({{1.f, 1.f}, 1.f}),
  };

  mCurrentState = mStates.front();
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
  namespace plane = constants::plane;
  namespace aiDebug = constants::ai::debug;
  namespace aiColors = constants::colors::debug::ai;
  namespace collisionColors = constants::colors::debug::collisions;


  assert(mDangerMap.size() == mInterestMap.size());

  for ( size_t i {}; i < mInterestMap.size(); ++i )
  {
    float dir {};

    if ( self.hasJumped() == true )
      dir = (-90.f + i * 180.f) * M_PI / 180.f;
    else
      dir = (i * plane::pitchStep) * M_PI / 180.f;


    const SDL_Vector pos {self.pilot.x(), self.pilot.y()};

    if ( mDangerMap[i] != 0.f )
    {
      const SDL_Vector target
      {
        pos.x + aiDebug::dangerMagnitude * mDangerMap[i] * std::sin(dir),
        pos.y - aiDebug::dangerMagnitude * mDangerMap[i] * std::cos(dir),
      };

      setRenderColor(aiColors::danger);
      SDL_RenderDrawLine(
        gRenderer,
        toWindowSpaceX(self.pilot.x()) - 2,
        toWindowSpaceY(self.pilot.y()) - 2,
        toWindowSpaceX(target.x) - 2,
        toWindowSpaceY(target.y) - 2 );
    }


    if ( mInterestMap[i] != 0.f )
    {
      const SDL_Vector target
      {
        pos.x + aiDebug::interestMagnitude * mInterestMap[i] * std::sin(dir),
        pos.y - aiDebug::interestMagnitude * mInterestMap[i] * std::cos(dir),
      };

      setRenderColor(aiColors::interest);
      SDL_RenderDrawLine(
        gRenderer,
        toWindowSpaceX(self.pilot.x()) + 2,
        toWindowSpaceY(self.pilot.y()) + 2,
        toWindowSpaceX(target.x) + 2,
        toWindowSpaceY(target.y) + 2 );
    }


    const auto heat = mInterestMap[i] - mDangerMap[i];

    if ( heat > 0.f )
    {
      const SDL_Vector target
      {
        pos.x + aiDebug::heatMagnitude * heat * std::sin(dir),
        pos.y - aiDebug::heatMagnitude * heat * std::cos(dir),
      };

      setRenderColor(aiColors::seek);
      SDL_RenderDrawLine(
        gRenderer,
        toWindowSpaceX(self.pilot.x()),
        toWindowSpaceY(self.pilot.y()),
        toWindowSpaceX(target.x),
        toWindowSpaceY(target.y) );
    }
  }


  for ( auto& [action, temperature] : mActions )
  {
    SDL_FRect actionBox
    {
      aiDebug::actionGridOffsetX,
      aiDebug::actionGridOffsetY,
      aiDebug::actionBoxSizeX,
      aiDebug::actionBoxSizeY,
    };

    switch(action)
    {
      case AiAction::Accelerate:
      {
        actionBox.x += 1.f * aiDebug::actionBoxStepX;
        actionBox.y += 0.f * aiDebug::actionBoxStepY;

        break;
      }

      case AiAction::Decelerate:
      {
        actionBox.x += 1.f * aiDebug::actionBoxStepX;
        actionBox.y += 1.f * aiDebug::actionBoxStepY;

        break;
      }

      case AiAction::TurnLeft:
      {
          actionBox.x += 0.f * aiDebug::actionBoxStepX;
          actionBox.y += 1.f * aiDebug::actionBoxStepY;

          break;
      }

      case AiAction::TurnRight:
      {
          actionBox.x += 2.f * aiDebug::actionBoxStepX;
          actionBox.y += 1.f * aiDebug::actionBoxStepY;

          break;
      }

      case AiAction::Shoot:
      {
          actionBox.x += 2.f * aiDebug::actionBoxStepX;
          actionBox.y += 0.f * aiDebug::actionBoxStepY;

          break;
      }

      case AiAction::Jump:
      {
          actionBox.x += 0.f * aiDebug::actionBoxStepX;
          actionBox.y += 0.f * aiDebug::actionBoxStepY;

          break;
      }

      default:
        assert(false);
    }

    actionBox.x += self.pilot.x();
    actionBox.y += self.pilot.y();

    const auto actionGridRightBorder =
      self.pilot.x() + aiDebug::actionGridOffsetX + 3.f * aiDebug::actionBoxStepX;

    if ( actionGridRightBorder > 1.f )
      actionBox.x -= actionGridRightBorder - 1.f;

    const auto actionGridTopBorder =
      self.pilot.y() + aiDebug::actionGridOffsetY;

    if ( actionGridTopBorder < 0.f )
      actionBox.y -= actionGridTopBorder;

    actionBox =
    {
      toWindowSpaceX(actionBox.x),
      toWindowSpaceY(actionBox.y),
      scaleToScreenX(actionBox.w),
      scaleToScreenY(actionBox.h),
    };


    setRenderColor(aiColors::actionBox);
    SDL_RenderDrawRectF( gRenderer, &actionBox );


    if ( temperature <= 0.f )
      continue;

    SDL_BlendMode currentBlendMode {};
    SDL_GetRenderDrawBlendMode(
      gRenderer,
      &currentBlendMode );

    SDL_SetRenderDrawBlendMode(
      gRenderer,
      SDL_BLENDMODE_BLEND );

    auto actionBoxColor = aiColors::actionBox;
    actionBoxColor.a *= temperature;

    setRenderColor(actionBoxColor);
    SDL_RenderFillRectF( gRenderer, &actionBox );

    SDL_SetRenderDrawBlendMode(
      gRenderer,
      currentBlendMode );
  }


  if ( self.hasJumped() == false )
  {
    namespace barn = constants::barn;

    setRenderColor(aiColors::planeSpeedVector);
    SDL_RenderDrawLine(
      gRenderer,
      toWindowSpaceX(self.x()) + 4,
      toWindowSpaceY(self.y()) + 4,
      toWindowSpaceX(self.x() + self.speedVector().x * constants::tickRate) + 4,
      toWindowSpaceY(self.y() + self.speedVector().y * constants::tickRate) + 4 );

    const auto gravity = (self.maxSpeed() - self.speed());

    setRenderColor(aiColors::planeGravityVector);
    SDL_RenderDrawLine(
      gRenderer,
      toWindowSpaceX(self.x()) + 4,
      toWindowSpaceY(self.y()) + 4,
      toWindowSpaceX(self.x()) + 4,
      toWindowSpaceY(self.y() + gravity) + 4 );

    return;
  }

  setRenderColor(aiColors::pilotSpeedVector);
  SDL_RenderDrawLine(
    gRenderer,
    toWindowSpaceX(self.pilot.x()) + 4,
    toWindowSpaceY(self.pilot.y()) + 4,
    toWindowSpaceX(self.pilot.x() + self.pilot.speedVec().x * constants::tickRate) + 4,
    toWindowSpaceY(self.pilot.y() + self.pilot.speedVec().y * constants::tickRate) + 4 );
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
