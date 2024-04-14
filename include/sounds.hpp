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

#pragma once

#include <SDL_mixer.h>


struct Sounds
{
  Mix_Chunk* shoot {};
  Mix_Chunk* explosion {};

  Mix_Chunk* hitPlane {};
  Mix_Chunk* hitChute {};
  Mix_Chunk* hitGround {};

  Mix_Chunk* pilotFallLoop {};
  Mix_Chunk* pilotChuteLoop {};

  Mix_Chunk* pilotDeath {};
  Mix_Chunk* pilotRescue {};

  Mix_Chunk* victory {};
  Mix_Chunk* defeat {};


  Sounds() = default;
};

extern Sounds sounds;
