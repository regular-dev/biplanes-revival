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


Controls aiActionToControls( const AiAction );


struct AiRewardDatasetEntry
{
  std::vector <float> state {};
  float reward {};

  AiRewardDatasetEntry() = default;
};


struct AiRewardDataset
{
  AI_Backend::InputBatch states {};
  AI_Backend::InputBatch rewards {};

  AiRewardDataset() = default;
};


struct AiDatasetEntry
{
  std::vector <float> inputs {};
  AiAction action {};

  AiDatasetEntry() = default;


  std::vector <float> merge() const;
};

struct AiRewardedAction
{
  AiAction action {};
  float reward {};
};


float calcReward(
  const std::vector <float>& inputs,
  const AiAction output );

std::vector <float> calcRewards(
  const std::vector <float>& inputs,
  const std::vector <AiAction>& outputs );

std::vector <AiRewardedAction> predictActionRewards(
  const std::vector <float>& inputs,
  const AI_Backend& );


class AiDataset
{
  std::vector <AiDatasetEntry> mData {};


public:
  AiDataset() = default;


  void push( const AiDatasetEntry& );

  void push(
    const std::vector <float>& inputs,
    const AiAction );

  void merge( AiDataset& target );

  void shuffle();
  void removeDuplicates( const float margin = 0.0025f );
  void dropEveryNthEntry( const size_t n );
  void saveEveryNthEntry( const size_t n );

  size_t size() const;

  AiRewardDataset toRewardDataset( const AI_Backend& ) const;

  void printActionStats() const;
};


class AiInputFilter
{
public:
  AiInputFilter() = default;
  virtual ~AiInputFilter() = default;


  virtual AiDatasetEntry filterInput(
    const std::vector <float>& inputs,
    const Plane&,
    AI_Backend& );

  virtual void filterActions(
    const Plane&,
    std::vector <size_t>& ) const;

  virtual void filterActions(
    const Plane&,
    std::vector <std::pair< size_t, float >>& ) const;
};


class AiTrainingPhase
{
protected:
  AiInputFilter* mInputProcessor {};


public:
  AiTrainingPhase() = default;
  virtual ~AiTrainingPhase();


  void train( AI_Backend&, AiDataset& );

  virtual void update();
  virtual Controls getInput( const Plane& );

  virtual void init();
  virtual void initNewRound();

  virtual void save() const;
  virtual void load();


  virtual bool hasRoundFinished() const;
  virtual bool hasPhaseFinished() const;

  virtual bool isReadyForTraining( const Plane& ) const;
};


class AiController
{
  AiTrainingPhase* mTrainingPhase {};

public:
  AiController() = default;

  void init();
  void save();
  void load();

  void update();
  void processInput();
};

extern AiController aiController;
