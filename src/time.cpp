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

#include <include/time.hpp>
#include <include/platform.hpp>

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
  #include <time.h>
  #include <cstdint>

#elif PLATFORM == PLATFORM_WINDOWS
  #include <windows.h>

#endif


double deltaTime {};


#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
static double
getCurrentTime()
{
//  POSIX implementation
  timespec time;
  clock_gettime( CLOCK_MONOTONIC, &time );
  return static_cast <uint64_t> (time.tv_sec) * 1'000'000 + time.tv_nsec / 1000;
}
#endif

double
countDelta()
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

  static double StartingTime {};
  static double EndingTime {};
  static double Frequency {};

  EndingTime = getCurrentTime() / 1'000'000.0;
  deltaTime = EndingTime - StartingTime;
  StartingTime = getCurrentTime() / 1'000'000.0;

#elif PLATFORM == PLATFORM_WINDOWS

  static LARGE_INTEGER StartingTime {};
  static LARGE_INTEGER EndingTime {};
  static LARGE_INTEGER Frequency {};

  QueryPerformanceFrequency(&Frequency);
  QueryPerformanceCounter(&EndingTime);

  deltaTime = static_cast <double> (EndingTime.QuadPart - StartingTime.QuadPart)
              / Frequency.QuadPart;

  QueryPerformanceCounter(&StartingTime);

#endif

  return deltaTime;
}
