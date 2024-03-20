#pragma once

#include <lib/SDL_Vector.h>

#include <vector>


bool segment_intersects_polygon(
  const SDL_Vector& from,
  const SDL_Vector& to,
  const std::vector <SDL_Vector>& polygon,
  SDL_Vector* contact );

float get_distance_between_points(
  const SDL_Vector& p1,
  const SDL_Vector& p2 );

float clamp_angle(
  const float degrees,
  const float maxDegrees );

float get_angle_relative(
  const float degreesSource,
  const float degreesTarget );

float get_angle_to_point(
  const SDL_Vector& from,
  const SDL_Vector& to );

size_t angleToPitchIndex( const float degrees );

