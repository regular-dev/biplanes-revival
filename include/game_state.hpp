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
    bool aiInputs {};

  } debug {};


  bool isPaused {};
  bool isExiting {};
  bool isRoundFinished {};
  bool isHardcoreEnabled {};

  bool autoSkipIntro {};
  bool isVSyncEnabled {true};
  bool isAudioEnabled {true};
  float audioPanDepth {0.4f};

  bool deltaTimeResetRequested {};
  bool fastforwardGameLoop {};
  bool disableRendering {};

  GAME_MODE gameMode {};
  DIFFICULTY botDifficulty {DIFFICULTY::MEDIUM};
  uint8_t winScore {10};


  GameState() = default;
};

GameState& gameState();
