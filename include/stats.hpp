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

#pragma once

#include <cstdint>


struct Statistics
{
  uint32_t shots {};
  uint32_t plane_hits {};
  uint32_t chute_hits {};
  uint32_t pilot_hits {};

  uint32_t jumps {};
  uint32_t crashes {};
  uint32_t falls {};
  uint32_t rescues {};

  uint32_t plane_kills {};
  uint32_t plane_deaths {};
  uint32_t pilot_deaths {};

  uint32_t wins {};
  uint32_t losses {};
  uint32_t wins_vs_developer {};
  uint32_t wins_vs_insane {};


//  These are calculated
  uint32_t totalHits {};
  uint32_t misses {};
  float accuracy {};
  float shotsPerKill {};

  uint32_t suicides {};
  float selfPreservation {};

  uint32_t totalKills {};
  uint32_t totalDeaths {};
  float kdRatio {};
  float survivability {};


  Statistics() = default;
};


void calcDerivedStats( Statistics& );

void updateRecentStats();
void resetRecentStats();

void updateTotalStats();
