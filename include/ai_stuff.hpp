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

#include <include/fwd.hpp>
#include <include/enums.hpp>
#include <include/timer.hpp>
#include <include/ai_backend.hpp>

#include <map>
#include <vector>
#include <cstddef>


struct AiStateMonitor
{
  size_t winCount {};
  size_t lifeTime {};
  size_t airborneTime {};
  size_t ejectedTime {};
  float maxHeight {};


  AiStateMonitor() = default;

  void update( const Plane& self, const Plane& opponent );
  void reset();

  void printState() const;

  int64_t airborneScore() const;
};


struct AiDatasetEntry
{
  std::vector <float> inputs {};
  size_t output {};

  AiDatasetEntry() = default;
};


class AiDataset
{
  std::vector <AiDatasetEntry> mData {};


public:
  AiDataset() = default;


  void push(
    const std::vector <float>& inputs,
    const size_t output );

  void merge( AiDataset& target );

  void shuffle();
  void dropEveryNthEntry( const size_t n );
  void saveEveryNthEntry( const size_t n );

  size_t size() const;

  AI_Backend::InputBatch toBatch() const;
  AI_Backend::Labels toLabels() const;

  void printActionStats() const;
};


struct AiData
{
  AiStateMonitor state {};

  AiDataset roundDataset {};
  AiDataset epochDataset {};

  std::shared_ptr <AI_Backend> backend {};

  size_t actionConstraint {};


  AiData() = default;
};


class AiController
{
  std::map <PLANE_TYPE, AiData> mAiData
  {
    {PLANE_TYPE::BLUE, {}},
    {PLANE_TYPE::RED, {}},
  };

  Timer mRoundDuration {5.0};
  Timer mDeathCounter {1.0};

  const static size_t mWinCountRequirement {3};
  size_t mEpochsTrained {};


  void train();
  void newEpoch();
  void restartRound();
  void printEpochActionStats();

  void raiseActionConstraint( AiData& );
  void lowerActionConstraint( AiData& );
  void resetActionConstraint( AiData& );

  bool hasRoundFinished();
  void evaluateWinner();
  void processWinner( AiData& );

  void filterValidActions( const Plane&, std::vector <size_t>& ) const;
  Controls actionToControls( const AiAction ) const;


public:
  AiController() = default;

  void init();
  void save();
  void load();

  void update();
  void processInput();
};

extern AiController aiController;
