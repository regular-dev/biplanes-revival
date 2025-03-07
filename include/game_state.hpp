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

#include <include/enums.hpp>
#include <include/stats.hpp>

#include <map>


struct GameState
{
private:
  using DIFFICULTY = DIFFICULTY::DIFFICULTY;


public:

  struct
  {
    Statistics total {};

    std::map <PLANE_TYPE, Statistics> recent {};

  } stats {};


  struct
  {
    bool toConsole {};
    bool toFile {true};
    bool stats {true};

  } output {};


  struct
  {
    bool collisions {};
    bool ai {};

    bool stepByStepMode {};
    bool advanceOneTick {};

  } debug {};


  bool isPaused {};
  bool isExiting {};
  bool isRoundRunning {};
  bool isRoundFinished {};

  struct
  {
    bool extraClouds {};
    bool oneShotKills {};
    bool alternativeHitboxes {};

  } features {}, featuresLocal {};

  bool autoSkipIntro {};
  bool isVSyncEnabled {true};
  bool isAudioEnabled {true};
  uint8_t audioVolume {75};
  uint8_t stereoDepth {40};

  GAME_MODE gameMode {};
  DIFFICULTY botDifficulty {DIFFICULTY::EASY};
  uint8_t winScore {10};


  GameState() = default;
};

GameState& gameState();
