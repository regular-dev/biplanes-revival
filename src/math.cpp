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

#include <include/math.hpp>
#include <include/constants.hpp>

#include <lib/godot_math.hpp>

#include <cmath>


bool
segment_intersects_polygon(
  const SDL_Vector& from,
  const SDL_Vector& to,
  const std::vector <SDL_Vector>& polygon,
  SDL_Vector* contact )
{
  const auto delta = to - from;

  if ( delta == SDL_Vector{} )
    return false;

  SDL_Vector closestContactPoint {from + delta * 2.f};

  for ( size_t i = 0, j = 1; j < polygon.size(); ++i, ++j )
  {
    SDL_Vector contactPoint {};

    const auto intersects = segment_intersects_segment(
      from, to,
      polygon.at(i), polygon.at(j),
      &contactPoint );

    if ( intersects == false )
      continue;

    if ( (contactPoint - from).length() < (closestContactPoint - from).length() )
      closestContactPoint = contactPoint;
  }

  const auto contactDistance =
    (closestContactPoint - from).length();

  if ( contactDistance > delta.length() )
    return false;


  if ( contact != nullptr )
    *contact = closestContactPoint;

  return true;
}

float
get_distance_between_points(
  const SDL_Vector& p1,
  const SDL_Vector& p2 )
{
  return std::sqrt(
    std::pow( p2.x - p1.x, 2.f ) +
    std::pow( p2.y - p1.y, 2.f ) );
}

float
clamp_angle(
  const float angle,
  const float constraint )
{
  return std::fmod(angle + constraint, constraint);
}

float
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

float
get_angle_to_point(
  const SDL_Vector& from,
  const SDL_Vector& to )
{
  const SDL_Vector dir = to - from;

  const float angle =
    (std::atan2(dir.y, dir.x) + M_PI_2)
    * 180.f / M_PI;

  return clamp_angle(angle, 360.f);
}

size_t angleToPitchIndex(
  const float degrees )
{
  return std::round(
    degrees / constants::plane::pitchStep );
}
