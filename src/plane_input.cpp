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

#include <include/plane.hpp>
#include <include/enums.hpp>


void
Plane::Input::setPlane(
  Plane* parentPlane )
{
  plane = parentPlane;
}

void
Plane::Input::Accelerate()
{
  plane->Accelerate();
}

void
Plane::Input::Decelerate()
{
  plane->Decelerate();
}

void
Plane::Input::TurnLeft()
{
  if ( plane->mHasJumped == true )
    plane->pilot.Move(PLANE_PITCH::PITCH_LEFT);

  else if ( plane->mIsLocal == true )
    plane->Turn(PLANE_PITCH::PITCH_LEFT);
}

void
Plane::Input::TurnRight()
{
  if ( plane->mHasJumped == true )
    plane->pilot.Move(PLANE_PITCH::PITCH_RIGHT);

  else if ( plane->mIsLocal == true )
    plane->Turn(PLANE_PITCH::PITCH_RIGHT);
}

void
Plane::Input::TurnIdle()
{
  if ( plane->mHasJumped == true )
    plane->pilot.MoveIdle();
}

void
Plane::Input::Shoot()
{
  plane->Shoot();
}

void
Plane::Input::Jump()
{
  if ( plane->mHasJumped == true )
  {
    plane->pilot.OpenChute();

    if ( plane->isBot() == true )
      plane->pilot.ChuteUnlock();
  }
  else
    plane->Jump();
}

void
Plane::Input::ExecuteAiAction(
  const AiAction action )
{
  switch (action)
  {
    case AiAction::Idle:
      break;

    case AiAction::Accelerate:
      Accelerate();
      break;

    case AiAction::Decelerate:
      Decelerate();
      break;

    case AiAction::TurnLeft:
      TurnLeft();
      break;

    case AiAction::TurnRight:
      TurnRight();
      break;

    case AiAction::Shoot:
      Shoot();
      break;

    case AiAction::Jump:
      Jump();
      break;

    default:
      break;
  }
}
