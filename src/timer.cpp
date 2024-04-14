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

#include <include/timer.hpp>
#include <include/time.hpp>


Timer::Timer(
  const float timeout )
{
  mCounter = 0.0f;
  mTimeout = timeout;
  mIsCounting = false;
}

void
Timer::Update()
{
  if ( mIsCounting == false )
    return;


  if ( mCounter > 0.0f )
  {
    mCounter -= deltaTime;

    if ( mCounter < 0.0f )
      mCounter = 0.0f;
  }
  else
    Stop();
}

void
Timer::Start()
{
  mIsCounting = true;
  Reset();
}

void
Timer::Stop()
{
  mIsCounting = false;
  mCounter = 0.0f;
}

void
Timer::Pause()
{
  mIsCounting = false;
}

void
Timer::Continue()
{
  mIsCounting = true;
}

void
Timer::Reset()
{
  mCounter = mTimeout;
}

void
Timer::SetNewTimeout(
  const float timeout )
{
  mTimeout = timeout;
}

void
Timer::SetNewRemainder(
  const float remainder )
{
  mCounter = remainder;
}


float
Timer::remainderTime() const
{
  return mCounter;
}

float
Timer::timeout() const
{
  return mTimeout;
}

bool
Timer::isReady() const
{
  return mCounter <= 0.0f;
}

bool
Timer::isCounting() const
{
  return mIsCounting;
}
