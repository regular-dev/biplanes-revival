/*
  Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md).
  Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <lib/SDL_Vector.h>


constexpr auto epsilon = std::numeric_limits <float>::epsilon();

inline SDL_Vector
get_closest_point_to_segment(
  const SDL_Vector& p_point,
  const SDL_Vector& p_s_from,
  const SDL_Vector& p_s_to )
{
  const SDL_Vector p = p_point - p_s_from;
  const SDL_Vector n = p_s_to - p_s_from;
  const float l2 = n.length_squared();

  if ( l2 < 1e-20f )
//  Both points are the same, just give any.
    return p_s_from;

  const float d = n.dot(p) / l2;

  if ( d <= 0.0f )
//  Before first point.
    return p_s_from;

  else if ( d >= 1.0f )
//  After first point.
    return p_s_to;

//  Inside.
  return p_s_from + n * d;
}

inline SDL_Vector
get_closest_point_to_segment_uncapped(
  const SDL_Vector& p_point,
  const SDL_Vector& p_s_from,
  const SDL_Vector& p_s_to )
{
  const SDL_Vector p = p_point - p_s_from;
  const SDL_Vector n = p_s_to - p_s_from;

  float l2 = n.length_squared();

  if ( l2 < 1e-20f )
//    Both points are the same, just give any.
    return p_s_from;


  const float d = n.dot(p) / l2;

//  Inside.
  return p_s_from + n * d;
}

inline bool
line_intersects_line(
  const SDL_Vector& p_from_a,
  const SDL_Vector& p_dir_a,
  const SDL_Vector& p_from_b,
  const SDL_Vector& p_dir_b,
  SDL_Vector& result )
{
  // See http://paulbourke.net/geometry/pointlineplane/

  const float denom = p_dir_b.y * p_dir_a.x - p_dir_b.x * p_dir_a.y;

  if ( denom < epsilon )
    return false; // Parallel?


  const SDL_Vector v = p_from_a - p_from_b;

  const float t = (p_dir_b.x * v.y - p_dir_b.y * v.x) / denom;

  result = p_from_a + t * p_dir_a;

  return true;
}

inline bool
segment_intersects_segment(
  const SDL_Vector& p_from_a,
  const SDL_Vector& p_to_a,
  const SDL_Vector& p_from_b,
  const SDL_Vector& p_to_b,
  SDL_Vector* result )
{
  const SDL_Vector B = p_to_a - p_from_a;
  SDL_Vector C = p_from_b - p_from_a;
  SDL_Vector D = p_to_b - p_from_a;

  const auto ABlen = B.dot(B);

  if ( ABlen <= 0.f )
    return false;


  const SDL_Vector Bn = B / ABlen;

  C =
  {
    C.x * Bn.x + C.y * Bn.y,
    C.y * Bn.x - C.x * Bn.y,
  };

  D =
  {
    D.x * Bn.x + D.y * Bn.y,
    D.y * Bn.x - D.x * Bn.y,
  };


//  Fail if C x B and D x B have the same sign (segments don't intersect).
  if ( (C.y < -epsilon && D.y < -epsilon) || (C.y > epsilon && D.y > epsilon) )
    return false;


//  Fail if segments are parallel or colinear.
//  (when A x B == zero, i.e (C - D) x B == zero, i.e C x B == D x B)
  if ( std::fabs(C.y - D.y) < epsilon )
    return false;


  const auto ABpos = D.x + (C.x - D.x) * D.y / (D.y - C.y);

//  Fail if segment C-D crosses line A-B outside of segment A-B.
  if ( ABpos < 0 || ABpos > 1 )
    return false;


//  Apply the discovered position to line A-B in the original coordinate system.
  if ( result != nullptr )
    *result = p_from_a + B * ABpos;

  return true;
}

inline float
segment_intersects_circle(
  const SDL_Vector& p_from,
  const SDL_Vector& p_to,
  const SDL_Vector& p_circle_pos,
  float p_circle_radius )
{
  const SDL_Vector line_vec = p_to - p_from;
  const SDL_Vector vec_to_line = p_from - p_circle_pos;

//  Create a quadratic formula of the form ax^2 + bx + c = 0
  const float a = line_vec.dot(line_vec);
  const float b = 2 * vec_to_line.dot(line_vec);
  const float c = vec_to_line.dot(vec_to_line) - p_circle_radius * p_circle_radius;

//  Solve for t.
  float sqrtterm = b * b - 4 * a * c;

//  If the term we intend to square root is less than 0 then the answer won't be real,
//  so it definitely won't be t in the range 0 to 1.
  if ( sqrtterm < 0 )
    return -1.f;


//  If we can assume that the line segment starts outside the circle (e.g. for continuous time collision detection)
//  then the following can be skipped and we can just return the equivalent of res1.
  sqrtterm = std::sqrt(sqrtterm);

  float res1 = (-b - sqrtterm) / (2 * a);
  float res2 = (-b + sqrtterm) / (2 * a);

  if ( res1 >= 0 && res1 <= 1 )
    return res1;

  if ( res2 >= 0 && res2 <= 1 )
    return res2;

  return -1.f;
}
