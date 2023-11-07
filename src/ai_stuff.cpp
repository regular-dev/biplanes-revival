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

#include <include/ai_stuff.hpp>
#include <include/biplanes.hpp>
#include <include/controls.hpp>
#include <include/game_state.hpp>
#include <include/menu.hpp>
#include <include/plane.hpp>
#include <include/utility.hpp>

#include <iomanip>


void
AiStateMonitor::update(
  const Plane& self,
  const Plane& opponent )
{
  if ( self.isDead() == true )
    return;

  ++lifeTime;

  if ( self.hasJumped() == true )
  {
    ++ejectedTime;
    return;
  }

  if (  self.isTakingOff() == true ||
        self.isAirborne() == true )
    ++airborneTime;
}

void
AiStateMonitor::reset()
{
  lifeTime = 0;
  airborneTime = 0;
  ejectedTime = 0;
}

int64_t
AiStateMonitor::airborneScore() const
{
  const auto idlePenalty {2.0};

  const auto idleTime = idlePenalty *
    (lifeTime - airborneTime);

  return airborneTime - idleTime;
}


void
AiDataset::push(
  const std::vector <float>& inputs,
  const size_t output )
{
  mData.push_back({inputs, output});
}

void
AiDataset::merge(
  AiDataset& target )
{
  target.mData.insert(
    target.mData.end(),
    mData.begin(),
    mData.end() );
}

void
AiDataset::shuffle()
{
  std::shuffle(
    mData.begin(),
    mData.end(),
    std::mt19937 {std::random_device{}()} );
}

void
AiDataset::dropEveryNthEntry(
  const size_t n )
{
  for ( size_t i = mData.size() - 1; i > 0; --i )
    if ( (i + 1) % n == 0 )
      mData.erase(mData.begin() + i);
}

void
AiDataset::saveEveryNthEntry(
  const size_t n )
{
  for ( size_t i = mData.size() - 1; i > 0; --i )
    if ( (i + 1) % n != 0 )
      mData.erase(mData.begin() + i);
}

size_t
AiDataset::size() const
{
  return mData.size();
}

AI_Backend::InputBatch
AiDataset::toBatch() const
{
  AI_Backend::InputBatch batch {};
  batch.reserve(mData.size());

  for ( const auto& state : mData )
    batch.push_back(
      {state.inputs.begin(), state.inputs.end()} );

  return batch;
}

AI_Backend::Labels
AiDataset::toLabels() const
{
  AI_Backend::Labels labels {};
  labels.reserve(mData.size());

  for ( const auto& state : mData )
    labels.push_back(state.output);

  return labels;
}

void
AiDataset::printActionStats() const
{
  std::map <AiAction, size_t> actionStatsMap {};

  for ( const auto& entry : mData )
    actionStatsMap[static_cast <AiAction> (entry.output)]++;


  std::vector <std::pair <AiAction, size_t>> actionStats {};

  for ( const auto& actionStat : actionStatsMap )
    actionStats.push_back({actionStat.first, actionStat.second});

  std::sort(actionStats.begin(), actionStats.end(),
  [] ( const auto& lhs, const auto& rhs )
  {
    return lhs.second > rhs.second;
  });

  for ( const auto& [action, count] : actionStats )
  {
    switch (action)
    {
      case AiAction::Idle:
      {
        log_message("idle ");
        break;
      }

      case AiAction::Accelerate:
      {
        log_message("accel ");
        break;
      }

      case AiAction::Decelerate:
      {
        log_message("decel ");
        break;
      }

      case AiAction::TurnLeft:
      {
        log_message("left ");
        break;
      }

      case AiAction::TurnRight:
      {
        log_message("right ");
        break;
      }

      case AiAction::Shoot:
      {
        log_message("shoot ");
        break;
      }

      case AiAction::Jump:
      {
        log_message("jump ");
        break;
      }
    }

    std::stringstream stream {};
    stream <<
      std::fixed <<
      std::setprecision(2) <<
      100.0 * count / mData.size();

    log_message(stream.str() + "% ");
  }

  log_message("\n");
}


void
AiController::init()
{
  for ( auto& [planeType, data] : mAiData )
  {
    data = {};
    data.backend = std::make_shared <AI_Backend> ();
  }

  mRoundDuration.Start();
  mDeathCounter.Reset();
}

void
AiController::newEpoch()
{
  for ( auto& [planeType, data] : mAiData )
  {
    data.state = {};
    data.roundDataset = {};
    data.epochDataset = {};
  }
}

void
AiController::restartRound()
{
  game_reset();

  mRoundDuration.Start();
  mDeathCounter.Stop();
  mDeathCounter.Reset();

  for ( auto& [planeType, data] : mAiData )
  {
    data.state.reset();
    data.roundDataset = {};
  }
}

void
AiController::raiseActionConstraint(
  AiData& data )
{
  if ( data.actionConstraint >= static_cast <size_t> (AiAction::ActionCount) )
    return;

  ++data.actionConstraint;

//  log_message("increased action randomness to " + std::to_string(data.actionConstraint) + "\n");
}

void
AiController::lowerActionConstraint(
  AiData& data )
{
  if ( data.actionConstraint == 0 )
    return;

  --data.actionConstraint;
//  log_message("decreased action randomness to " + std::to_string(data.actionConstraint) + "\n");
}

void
AiController::resetActionConstraint(
  AiData& data )
{
  data.actionConstraint = 0;
}

void
AiController::evaluateWinner()
{
  auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  auto& planeRed = planes.at(PLANE_TYPE::RED);

  auto& blueData = mAiData[planeBlue.type()];
  auto& redData = mAiData[planeRed.type()];

  size_t blueScore {};
  size_t redScore {};

  blueScore += planeBlue.isDead() == false;
  redScore += planeRed.isDead() == false;

  blueScore += blueData.state.airborneTime > 0;
  redScore += redData.state.airborneTime > 0;

  blueScore += redData.state.airborneScore() <= 0;
  redScore += blueData.state.airborneScore() <= 0;

  blueScore += blueData.state.airborneScore() > redData.state.airborneScore();
  redScore += redData.state.airborneScore() > blueData.state.airborneScore();

  blueScore += planeBlue.isDead() == false && planeBlue.isAirborne() == true;
  redScore += planeRed.isDead() == false && planeRed.isAirborne() == true;

  log_message("blue/red: " + std::to_string(blueScore) + " " + std::to_string(redScore), "\n");


  if ( blueScore > redScore )
  {
    menu.setMessage(MESSAGE_TYPE::BLUE_SIDE_WON);
    log_message("BLUE wins (airborne score " + std::to_string(blueData.state.airborneScore()) + ")\n");
    blueData.state.wins++;
    resetActionConstraint(blueData);
    raiseActionConstraint(redData);

    if ( blueData.state.wins <= mWinCountRequirement )
    {
      log_message("merging blue dataset\n");
      log_message("blue action stats:\n");
      blueData.roundDataset.printActionStats();
      blueData.roundDataset.merge(blueData.epochDataset);
    }

    return;
  }

  if ( redScore > blueScore )
  {
    menu.setMessage(MESSAGE_TYPE::RED_SIDE_WON);
    log_message("RED wins (airborne score " + std::to_string(redData.state.airborneScore()) + ")\n");
    redData.state.wins++;
    resetActionConstraint(redData);
    raiseActionConstraint(blueData);

    if ( redData.state.wins <= mWinCountRequirement )
    {
      log_message("merging red dataset\n");
      log_message("red action stats:\n");
      redData.roundDataset.printActionStats();
      redData.roundDataset.merge(redData.epochDataset);
    }

    return;
  }

  menu.setMessage(MESSAGE_TYPE::ROUND_DRAW);
  log_message(
    "NOBODY wins (score " +
    std::to_string(blueScore) +
    "/" +
    std::to_string(redScore) +
    ", airborne score " +
    std::to_string(blueData.state.airborneScore()) +
    "/" +
    std::to_string(redData.state.airborneScore()) +
    "\n");

  raiseActionConstraint(redData);
  raiseActionConstraint(blueData);
}

void
AiController::processInput()
{
  for ( auto& [planeType, plane] : planes )
  {
    if ( plane.isBot() == false )
      continue;


    const auto inputs = plane.aiState();

    for ( size_t i = 0; i < inputs.size(); ++i )
      if ( inputs[i] < 0.0f || inputs[i] > 1.0f )
      {
        log_message(
          "plane " + std::to_string(plane.type()),
          " invalid input " + std::to_string(i) +
          ": " + std::to_string(inputs[i]), "\n");

        assert(false);
      }

    auto& data = mAiData[plane.type()];

    const auto output = data.backend->predictDistLabel(
      {inputs.begin(), inputs.end()},
      data.actionConstraint );


    Controls aiControls {};

    const auto action = static_cast <AiAction> (output);

    switch (action)
    {
      case AiAction::Idle:
      {
//        log_message(std::to_string(plane.type()), ": idle", "\n");
        break;
      }

      case AiAction::Accelerate:
      {
//        log_message(std::to_string(plane.type()), ": fwd", "\n");
        aiControls.throttle = THROTTLE_INCREASE;
        break;
      }

      case AiAction::Decelerate:
      {
//        log_message(std::to_string(plane.type()), ": back", "\n");
        aiControls.throttle = THROTTLE_DECREASE;
        break;
      }

      case AiAction::TurnLeft:
      {
//        log_message(std::to_string(plane.type()), ": left", "\n");
        aiControls.pitch = PITCH_LEFT;

        break;
      }

      case AiAction::TurnRight:
      {
//        log_message(std::to_string(plane.type()), ": right", "\n");
        aiControls.pitch = PITCH_RIGHT;

        break;
      }

      case AiAction::Shoot:
      {
//        log_message(std::to_string(plane.type()), ": shoot", "\n");
        aiControls.shoot = true;
        break;
      }

      case AiAction::Jump:
      {
//        log_message(std::to_string(plane.type()), ": jump", "\n");
//        aiControls.jump = true;
        break;
      }

      default:
      {
        log_message("ERROR: AI backend predicted out-of-range label "
          + std::to_string(output), "\n");
        break;
      }
    }

    processLocalControls(plane, aiControls);


    if ( gameState().gameMode != GAME_MODE::BOT_VS_BOT )
      continue;

    if ( data.state.wins < mWinCountRequirement )
    {
      data.roundDataset.push(inputs, static_cast <size_t> (action));
      data.state.actionStats[action]++;
    }
  }
}

void
AiController::update()
{
  mRoundDuration.Update();
  mDeathCounter.Update();

  auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  auto& planeRed = planes.at(PLANE_TYPE::RED);

  auto& blueData = mAiData[planeBlue.type()];
  auto& redData = mAiData[planeRed.type()];

  bool roundFinished = mRoundDuration.isReady();

  if ( planeBlue.isDead() == true || planeRed.isDead() == true )
  {
    if ( mDeathCounter.isCounting() == false )
      mDeathCounter.Start();

    if (  mDeathCounter.isReady() == true ||
          ( planeBlue.isDead() == true &&
            planeRed.isDead() == true) )
      roundFinished = true;
  }


  blueData.state.update(planeBlue, planeRed);
  redData.state.update(planeRed, planeBlue);


  if ( roundFinished == false )
    return;


  evaluateWinner();

  if ( mEpochsTrained == 0 )
  {
    if ( blueData.state.wins == 0 && blueData.state.airborneTime == 0 )
    {
      resetActionConstraint(blueData);
      blueData.backend->initNet();
      log_message("BLUE reinit\n");
    }

    if ( redData.state.wins == 0 && redData.state.airborneTime == 0 )
    {
      resetActionConstraint(redData);
      redData.backend->initNet();
      log_message("RED reinit\n");
    }
  }

  if (  blueData.state.wins >= mWinCountRequirement &&
        redData.state.wins >= mWinCountRequirement )
  {
    log_message("\n");
    printEpochActionStats();
    train();
    newEpoch();
    resetActionConstraint(blueData);
    resetActionConstraint(redData);
  }

  log_message("\n");
  restartRound();
}

void
AiController::train()
{
  const size_t epochs {1};
  const size_t batchSize {8};


  log_message("training epoch " + std::to_string(mEpochsTrained + 1) + "\n");

  for ( auto& [planeType, data] : mAiData )
  {
    auto& dataset = data.epochDataset;

    dataset.saveEveryNthEntry(3);
    dataset.shuffle();

    data.backend->train(
      dataset.toBatch(),
      dataset.toLabels(),
      batchSize, epochs );
  }

  ++mEpochsTrained;
  gameState().deltaTimeResetRequested = true;
}

void
AiController::printEpochActionStats()
{
  for ( const auto& [planeType, aiData] : mAiData )
  {
    log_message(std::to_string(planeType) + " action stats:", "\n");
    aiData.epochDataset.printActionStats();
  }
}
