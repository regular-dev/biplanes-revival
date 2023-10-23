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

#pragma once

#include <SDL_mixer.h>


struct Sounds
{
  Mix_Chunk* shoot {};
  Mix_Chunk* hit {};
  Mix_Chunk* hitChute {};
  Mix_Chunk* hitMiss {};
  Mix_Chunk* expl {};
  Mix_Chunk* fall {};
  Mix_Chunk* chute {};
  Mix_Chunk* dead {};
  Mix_Chunk* victory {};
  Mix_Chunk* loss {};
  Mix_Chunk* rescue {};


  Sounds() = default;
};

extern Sounds sounds;