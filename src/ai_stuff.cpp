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
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/menu.hpp>
#include <include/plane.hpp>
#include <include/utility.hpp>

#include <iomanip>

#define AI_BLUE_PATH "assets/blue.ai"
#define AI_RED_PATH "assets/red.ai"


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

  if ( self.isAirborne() == true )
  {
    ++airborneTime;

//    y is inverted
    maxHeight = std::max(maxHeight, 1.0f - self.y());
  }
}

void
AiStateMonitor::reset()
{
  const auto winCountBackup = winCount;

  *this = {};

  winCount = winCountBackup;
}

void
AiStateMonitor::printState() const
{
  log_message("winCount: : " + std::to_string(winCount) + "\n");
  log_message("lifeTime: : " + std::to_string(lifeTime) + "\n");
  log_message("takeoffTime: : " + std::to_string(takeoffTime()) + "\n");
  log_message("airborneTime: : " + std::to_string(airborneTime) + "\n");
  log_message("airborneScore: : " + std::to_string(airborneScore()) + "\n");
  log_message("ejectedTime: : " + std::to_string(ejectedTime) + "\n");
  log_message("max height: " + std::to_string(maxHeight) + "\n");
}

int64_t
AiStateMonitor::takeoffTime() const
{
  return lifeTime - airborneTime;
}

int64_t
AiStateMonitor::airborneScore() const
{
  const int64_t takeoffTime = lifeTime - airborneTime;

  if ( takeoffTime > 0.5 * constants::tickRate )
    return -1;

  return airborneTime - takeoffTime;
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
  static std::mt19937 rng {std::random_device{}()};

  std::shuffle(
    mData.begin(),
    mData.end(),
    rng );
}

void
AiDataset::removeDuplicates(
  const float margin )
{
  if ( mData.size() < 2 )
    return;


  size_t removedElements {};
  size_t initialSize = mData.size();

  for ( size_t iLhs = mData.size() - 2; iLhs > 0; --iLhs )
  {
    const auto& inputLhs = mData[iLhs].inputs;
    const auto& inputRhs = mData[iLhs + 1].inputs;

    bool discardElement {true};


    for ( size_t i = 0; i < inputLhs.size(); ++i )
    {
      const auto baseIndexSelf = AiDatasetIndices::SelfState;
      const auto baseIndexOpponent = AiDatasetIndices::OpponentState;
      const auto dynamicStateIndex = AiDatasetPlaneIndices::PosX;

      if (  (i >= baseIndexSelf && i < baseIndexSelf + dynamicStateIndex) ||
            (i >= baseIndexOpponent && i < baseIndexOpponent + dynamicStateIndex) )
      {
        const auto distance =
          std::sqrt(std::pow(inputLhs[i] - inputRhs[i], 2.f) );

        if ( distance > 0.5f )
        {
          discardElement = false;
          break;
        }

        continue;
      }


      const auto distance = std::sqrt(
        std::pow(inputLhs[i] - inputRhs[i], 2.f));

      if ( distance > margin )
      {
        discardElement = false;
        break;
      }
    }


    if ( discardElement == true )
    {
      mData.erase(mData.begin() + iLhs + 1);
      ++removedElements;
    }
  }

  log_message("removed " + std::to_string(removedElements) + + "/" + std::to_string(initialSize) + " elements\n");
}

void
AiDataset::dropEveryNthEntry(
  const size_t n )
{
  if ( mData.empty() == true )
    return;


  for ( size_t i = mData.size() - 1; i > 0; --i )
    if ( (i + 1) % n == 0 )
      mData.erase(mData.begin() + i);
}

void
AiDataset::saveEveryNthEntry(
  const size_t n )
{
  if ( mData.empty() == true )
    return;


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

AI_Backend::InputBatch
AiDataset::toOneHotLabels() const
{
  AI_Backend::InputBatch labels {};
  labels.reserve(mData.size());

  for ( const auto& state : mData )
  {
    AI_Backend::EvalInput labelsVector (
      static_cast <size_t> (AiAction::ActionCount));

    labelsVector[state.output] = 1.0f;
    labels.push_back(labelsVector);
  }

  return labels;
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
AiController::save()
{
  auto& blueBackend = mAiData[PLANE_TYPE::BLUE].backend;
  auto& redBackend = mAiData[PLANE_TYPE::RED].backend;

  log_message("Saving blue AI model\n");
  blueBackend->saveModel(AI_BLUE_PATH);

  log_message("Saving red AI model\n");
  redBackend->saveModel(AI_RED_PATH);
}

void
AiController::load()
{
  init();
  restartRound();

  auto& blueBackend = mAiData[PLANE_TYPE::BLUE].backend;
  auto& redBackend = mAiData[PLANE_TYPE::RED].backend;

  log_message("Loading blue AI model\n");

  if ( blueBackend->loadModel(AI_BLUE_PATH) == false )
    log_message("ERROR: Failed to load blue AI model '" AI_BLUE_PATH "'\n");

  log_message("Loading red AI model\n");

  if ( redBackend->loadModel(AI_RED_PATH) == false )
    log_message("ERROR: Failed to load red AI model '" AI_RED_PATH "'\n");
}

void
AiController::train()
{
  const size_t epochs {1};
  const size_t batchSize {4};
  const float minLoss {2.0f};


  log_message("training epoch " + std::to_string(mEpochsTrained + 1) + "\n");

  for ( auto& [planeType, data] : mAiData )
  {
    auto& dataset = data.epochDataset;

    dataset.removeDuplicates();
    dataset.saveEveryNthEntry(3);
    dataset.shuffle();

    const auto inputs = dataset.toBatch();
    const auto labels = dataset.toOneHotLabels();

    float maxLoss {};

    do
    {
      maxLoss = data.backend->getLoss(inputs, labels);

      if ( maxLoss < 2.0f )
        break;

      data.backend->train(
        inputs, labels,
        batchSize, 1 );
    }
    while ( data.backend->getLoss(inputs, labels) <= maxLoss );
  }

  ++mEpochsTrained;
  gameState().deltaTimeResetRequested = true;
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
AiController::printEpochActionStats()
{
  for ( const auto& [planeType, aiData] : mAiData )
  {
    log_message(std::to_string(planeType) + " action stats:", "\n");
    aiData.epochDataset.printActionStats();
  }
}

void
AiController::raiseActionConstraint(
  AiData& data )
{
  if ( data.actionConstraint < static_cast <size_t> (AiAction::ActionCount) - 1 )
    ++data.actionConstraint;
}

void
AiController::lowerActionConstraint(
  AiData& data )
{
  if ( data.actionConstraint > 0 )
    --data.actionConstraint;
}

void
AiController::resetActionConstraint(
  AiData& data )
{
  data.actionConstraint = 0;
}

void
AiController::randomizeActionConstraint(
  AiData& data )
{
  data.actionConstraint =
    std::rand() % static_cast <size_t> (AiAction::ActionCount) - 1;
}

bool
AiController::hasRoundFinished()
{
  if ( mRoundDuration.isReady() == true )
    return true;


  auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  auto& planeRed = planes.at(PLANE_TYPE::RED);

  if ( planeBlue.isDead() == false && planeRed.isDead() == false )
    return false;

  if ( mDeathCounter.isCounting() == false )
    mDeathCounter.Start();

  if (  mDeathCounter.isReady() == true ||
        ( planeBlue.isDead() == true &&
          planeRed.isDead() == true ) )
    return true;

  return false;
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

  blueScore += planeBlue.isDead() == false && planeBlue.isAirborne() == true;
  redScore += planeRed.isDead() == false && planeRed.isAirborne() == true;

  if ( planeBlue.isDead() == true )
    blueScore = 0;

  if ( planeRed.isDead() == true )
    redScore = 0;

  if ( blueData.state.airborneScore() <= 0 )
    blueScore = 0;

  if ( redData.state.airborneScore() <= 0 )
    redScore = 0;

  const auto spawnHeight = 1.0f - constants::plane::spawnY;

  if ( blueData.state.maxHeight <= spawnHeight )
  {
    blueScore = 0;
    log_message("blue is too low\n");
  }

  if ( redData.state.maxHeight <= spawnHeight )
  {
    redScore = 0;
    log_message("red is too low\n");
  }

  log_message("blue/red: " +
    std::to_string(blueScore) + " " +
    std::to_string(redScore) + "\n");


  if ( mEpochsTrained == 0 )
  {
    if ( blueData.state.winCount == 0 && blueScore == 0 )
    {
      resetActionConstraint(blueData);
      blueData.backend->initNet();
      log_message("BLUE reinit\n");
    }

    if ( redData.state.winCount == 0 && redScore == 0 )
    {
      resetActionConstraint(redData);
      redData.backend->initNet();
      log_message("RED reinit\n");
    }
  }

  if ( blueScore > redScore )
  {
    menu.setMessage(MESSAGE_TYPE::BLUE_SIDE_WON);
    log_message("BLUE wins\n");
    blueData.state.printState();

    processWinner(blueData);
    raiseActionConstraint(redData);

    return;
  }

  if ( redScore > blueScore )
  {
    menu.setMessage(MESSAGE_TYPE::RED_SIDE_WON);
    log_message("RED wins\n");
    redData.state.printState();

    processWinner(redData);
    raiseActionConstraint(blueData);

    return;
  }

  if (  planeBlue.isDead() == false && blueData.state.maxHeight > spawnHeight &&
        planeRed.isDead() == false && redData.state.maxHeight > spawnHeight )
  {
    menu.setMessage(MESSAGE_TYPE::EVERY_SIDE_WON);
    log_message("EVERYONE wins\n");

    log_message("blue state:\n");
    blueData.state.printState();
    processWinner(blueData);

    log_message("red state:\n");
    redData.state.printState();
    processWinner(redData);

    return;
  }

  menu.setMessage(MESSAGE_TYPE::ROUND_DRAW);
  log_message("NOBODY wins\n");

  log_message("blue state:\n");
  blueData.state.printState();

  log_message("red state:\n");
  redData.state.printState();

  raiseActionConstraint(redData);
  raiseActionConstraint(blueData);
}

void
AiController::processWinner(
  AiData& aiData )
{
  aiData.state.winCount++;
  resetActionConstraint(aiData);

  if ( aiData.state.winCount <= mWinCountRequirement )
  {
    log_message("merging dataset\n");
    log_message("action stats:\n");
    aiData.roundDataset.printActionStats();
    aiData.roundDataset.merge(aiData.epochDataset);
  }
}

void
AiController::processInput()
{
  for ( auto& [planeType, plane] : planes )
  {
    if ( plane.isBot() == false || plane.isDead() == true )
      continue;


    const auto inputs = plane.aiState();

#if false || BIPLANES_CHECK_AI_INPUTS
    for ( size_t i = 0; i < inputs.size(); ++i )
      if ( inputs[i] < 0.0f || inputs[i] > 1.0f )
      {
        log_message(
          "plane " + std::to_string(plane.type()),
          " invalid input " + std::to_string(i) +
          ": " + std::to_string(inputs[i]), "\n");

        assert(false);
      }
#endif

    auto& data = mAiData[plane.type()];

//    const auto output = data.backend->predictDistLabel(
//      {inputs.begin(), inputs.end()},
//      data.actionConstraint );

//    auto outputs = data.backend->predictDistLabels(
//      {inputs.begin(), inputs.end()} );

//    const auto output = data.backend->getRandomIndex(
//      outputs,
//      data.actionConstraint );

    auto outputs = data.backend->predictDistLabelsProb(
      {inputs.begin(), inputs.end()} );

    filterValidActions(plane, outputs);

    const auto output = data.backend->getIndexByProb(outputs);

    const auto action = static_cast <AiAction> (output);

    processLocalControls(
      plane,
      actionToControls(action) );


    if ( gameState().gameMode != GAME_MODE::BOT_VS_BOT )
      continue;

    if ( data.state.winCount < mWinCountRequirement )
      data.roundDataset.push(inputs, static_cast <size_t> (action));
  }
}

void
AiController::filterValidActions(
  const Plane& plane,
  std::vector <size_t>& actions ) const
{
  for ( size_t i = 0; i < actions.size(); )
  {
    const auto action =
      static_cast <AiAction> (actions[i++]);

    if (  action == AiAction::Idle )
      continue;

    if (  action == AiAction::Accelerate &&
          plane.canAccelerate() == true )
      continue;

    if (  action == AiAction::Decelerate &&
          plane.canDecelerate() == true )
      continue;

    if (  action == AiAction::TurnLeft &&
          plane.canTurn() == true )
      continue;

    if (  action == AiAction::TurnRight &&
          plane.canTurn() == true )
      continue;

    if (  action == AiAction::Shoot &&
          plane.canShoot() == true )
      continue;

    if (  action == AiAction::Jump &&
          plane.canJump() == true )
      continue;


    actions.erase(actions.begin() + --i);
  }
}

void
AiController::filterValidActions(
  const Plane& plane,
  std::vector <std::pair< size_t, float >>& actions ) const
{
  for ( size_t i = 0; i < actions.size(); )
  {
    const auto action =
      static_cast <AiAction> (actions[i++].first);

    if (  action == AiAction::Idle )
      continue;

    if (  action == AiAction::Accelerate &&
          plane.canAccelerate() == true )
      continue;

    if (  action == AiAction::Decelerate &&
          plane.canDecelerate() == true )
      continue;

    if (  action == AiAction::TurnLeft &&
          plane.canTurn() == true )
      continue;

    if (  action == AiAction::TurnRight &&
          plane.canTurn() == true )
      continue;

    if (  action == AiAction::Shoot &&
          plane.canShoot() == true )
      continue;

    if (  action == AiAction::Jump &&
          plane.canJump() == true )
      continue;


    actions.erase(actions.begin() + --i);
  }
}

Controls
AiController::actionToControls(
  const AiAction action ) const
{
  Controls controls {};

  switch (action)
  {
    case AiAction::Idle:
      break;

    case AiAction::Accelerate:
    {
      controls.throttle = THROTTLE_INCREASE;
      break;
    }

    case AiAction::Decelerate:
    {
      controls.throttle = THROTTLE_DECREASE;
      break;
    }

    case AiAction::TurnLeft:
    {
      controls.pitch = PITCH_LEFT;
      break;
    }

    case AiAction::TurnRight:
    {
      controls.pitch = PITCH_RIGHT;
      break;
    }

    case AiAction::Shoot:
    {
      controls.shoot = true;
      break;
    }

    case AiAction::Jump:
    {
      controls.jump = true;
      break;
    }

    default:
    {
      log_message("ERROR: Unknown AiAction: "
        + std::to_string(static_cast <size_t> (action)), "\n");
      break;
    }
  }

  return controls;
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

  blueData.state.update(planeBlue, planeRed);
  redData.state.update(planeRed, planeBlue);


  if ( hasRoundFinished() == false )
    return;


  evaluateWinner();

  if (  blueData.state.winCount >= mWinCountRequirement &&
        redData.state.winCount >= mWinCountRequirement )
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
