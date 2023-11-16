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


Controls
aiActionToControls(
  const AiAction action )
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
AiStateMonitor::printState() const
{
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
  const AiDatasetEntry& entry )
{
  mData.push_back(entry);
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
      case AiAction::ActionCount:
      break;
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


AiTrainingPhase::~AiTrainingPhase()
{
  assert(mInputProcessor != nullptr);
  delete mInputProcessor;
}

void
AiTrainingPhase::train(
  AI_Backend& backend,
  AiDataset& dataset )
{
  dataset.removeDuplicates();
  dataset.saveEveryNthEntry(3);
  dataset.shuffle();


  const size_t batchSize {4};
  const float minLoss {0.01f};

  const auto inputs = dataset.toBatch();
  const auto labels = dataset.toOneHotLabels();

  size_t epochs = backend.getTrainedEpochsCount();

  float currentLoss = backend.getLoss(inputs, labels);
  float prevLoss {};


  do
  {
    log_message("Epoch " + std::to_string(epochs++) + " loss: " + std::to_string(currentLoss) + "\n");

    prevLoss = currentLoss;

    backend.train(
      inputs, labels,
      batchSize, 1 );

    currentLoss = backend.getLoss(inputs, labels);
  }
  while ( currentLoss < prevLoss && currentLoss > minLoss );


  log_message("Loss after training: " + std::to_string(currentLoss) + "\n");

  gameState().deltaTimeResetRequested = true;
}

void
AiTrainingPhase::update()
{
  assert(mInputProcessor != nullptr);
}

Controls
AiTrainingPhase::getInput(
  const Plane& plane )
{
  return {};
}

void
AiTrainingPhase::init()
{
}

void
AiTrainingPhase::initNewRound()
{
}

void
AiTrainingPhase::save() const
{
}

void
AiTrainingPhase::load()
{
  init();
  initNewRound();
}

bool
AiTrainingPhase::hasRoundFinished() const
{
  return false;
}

bool
AiTrainingPhase::hasPhaseFinished() const
{
  return false;
}

bool
AiTrainingPhase::isReadyForTraining(
  const Plane& ) const
{
  return false;
}


AiDatasetEntry
AiInputFilter::filterInput(
  const std::vector <float>& inputs,
  const Plane& plane,
  AI_Backend& backend )
{
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

//  const auto output = data.backend->predictDistLabel(
//    {inputs.begin(), inputs.end()},
//    data.actionConstraint );

//  auto outputs = data.backend->predictDistLabels(
//    {inputs.begin(), inputs.end()} );

//  const auto output = data.backend->getRandomIndex(
//    outputs,
//    data.actionConstraint );

  auto outputs = backend.predictDistLabelsProb(
    {inputs.begin(), inputs.end()} );

  filterActions(plane, outputs);

  const auto output = backend.getIndexByProb(outputs);

  return {inputs, output};
}

void
AiInputFilter::filterActions(
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
AiInputFilter::filterActions(
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


class TakeoffTrainingPhase : public AiTrainingPhase
{
  struct RoundData
  {
    AiStateMonitor state {};
    AiDataset dataset {};
  };


  std::map <PLANE_TYPE, std::shared_ptr <AI_Backend>> mBackends
  {
    {PLANE_TYPE::BLUE, std::make_shared <AI_Backend> ()},
    {PLANE_TYPE::RED, std::make_shared <AI_Backend> ()},
  };

  std::map <PLANE_TYPE, std::vector <RoundData>> mRoundData
  {
    {PLANE_TYPE::BLUE, {}},
    {PLANE_TYPE::RED, {}},
  };

  Timer mRoundTimeout {1.0};


public:
  TakeoffTrainingPhase() = default;

  void update() override;
  Controls getInput( const Plane& ) override;

  void init() override;
  void initNewRound() override;

  void save() const override;
  void load() override;

  bool hasRoundFinished() const override;
  bool hasPhaseFinished() const override;

  bool isReadyForTraining( const Plane& ) const override;
};

void
TakeoffTrainingPhase::update()
{
  mRoundTimeout.Update();


  auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  auto& planeRed = planes.at(PLANE_TYPE::RED);

  auto& roundDataBlue = mRoundData[planeBlue.type()];
  auto& roundDataRed = mRoundData[planeRed.type()];

  roundDataBlue.back().state.update(
    planeBlue, planeRed );

  roundDataRed.back().state.update(
    planeRed, planeBlue );


  if ( hasRoundFinished() == false )
    return;


  for ( const auto& [planeType, plane] : planes )
  {
    auto& data = mRoundData[planeType];

    std::sort( data.begin(), data.end(),
    [] ( const RoundData& lhs, const RoundData& rhs )
    {
      return lhs.state.takeoffTime() > rhs.state.takeoffTime();
    });

//    const auto newEnd = std::unique( data.begin(), data.end(),
//    [] ( const RoundData& lhs, const RoundData& rhs )
//    {
//      return lhs.state.takeoffTime() == rhs.state.takeoffTime();
//    });

//    data.resize(std::distance(data.begin(), newEnd));

    if ( plane.isDead() == false && plane.isAirborne() == false )
    {
      log_message("Plane " + std::to_string(planeType) + " takes off too slow\n");
      mBackends.at(planeType)->initNet();
      data.clear();

      continue;
    }

    log_message("Plane " + std::to_string(planeType) + " state:\n");
    data.back().state.printState();

    log_message("\n");

    if ( isReadyForTraining(plane) == false )
      continue;


    log_message("Plane " + std::to_string(planeType) + " action stats:\n");
    data.back().dataset.printActionStats();

    log_message("\n");

    log_message("Training plane " + std::to_string(planeType) + "\n");
    train( *mBackends.at(planeType), data.back().dataset );

    data.front() = data.back();
    data.resize(1);

    log_message("\n");
  }

  initNewRound();
}

Controls
TakeoffTrainingPhase::getInput(
  const Plane& plane )
{
  const auto datasetEntry = mInputProcessor->filterInput(
    plane.aiState(),
    plane,
    *mBackends.at(plane.type()) );

  if ( plane.isAirborne() == false )
    mRoundData[plane.type()].back().dataset.push(datasetEntry);

  const auto action =
    static_cast <AiAction> (datasetEntry.output);

  return aiActionToControls(action);
}

void
TakeoffTrainingPhase::init()
{
  if ( mInputProcessor == nullptr )
    mInputProcessor = new AiInputFilter();

  for ( auto& [planeType, backend] : mBackends )
    backend->initNet();

  for ( auto& [planeType, data] : mRoundData )
    data.clear();

  initNewRound();
}

void
TakeoffTrainingPhase::initNewRound()
{
  mRoundTimeout.Start();

  for ( auto& [planeType, data] : mRoundData )
    data.push_back({});

  game_reset();
}

void
TakeoffTrainingPhase::save() const
{
  auto& blueBackend = mBackends.at(PLANE_TYPE::BLUE);
  auto& redBackend = mBackends.at(PLANE_TYPE::RED);

  log_message("Saving blue AI model " + std::to_string(blueBackend->getWeights().size()) + "\n");
  blueBackend->saveModel(AI_BLUE_PATH);

  log_message("Saving red AI model " + std::to_string(redBackend->getWeights().size()) + "\n");
  redBackend->saveModel(AI_RED_PATH);
}

void
TakeoffTrainingPhase::load()
{
  init();

  auto& blueBackend = mBackends.at(PLANE_TYPE::BLUE);
  auto& redBackend = mBackends.at(PLANE_TYPE::RED);

  log_message("Loading blue AI model\n");

  if ( blueBackend->loadModel(AI_BLUE_PATH) == false )
    log_message("ERROR: Failed to load blue AI model '" AI_BLUE_PATH "'\n");

  log_message("Blue AI model weights: " + std::to_string(blueBackend->getWeights().size()) + "\n");

  log_message("Loading red AI model\n");

  if ( redBackend->loadModel(AI_RED_PATH) == false )
    log_message("ERROR: Failed to load red AI model '" AI_RED_PATH "'\n");

  log_message("Red AI model weights: " + std::to_string(redBackend->getWeights().size()) + "\n");
}

bool
TakeoffTrainingPhase::hasRoundFinished() const
{
  return mRoundTimeout.isReady();
}

bool
TakeoffTrainingPhase::hasPhaseFinished() const
{
  return false;
}

bool
TakeoffTrainingPhase::isReadyForTraining(
  const Plane& plane ) const
{
  auto& data = mRoundData.at(plane.type());

  size_t records {};
  size_t takeoffTimePrev {};

  for ( const auto& round : data )
  {
    const auto takeoffTimeNew = round.state.takeoffTime();
    records += takeoffTimeNew < takeoffTimePrev;

    log_message("takeoffTimePrev " + std::to_string(takeoffTimePrev) + " ");
    log_message("takeoffTimeNew " + std::to_string(takeoffTimeNew) + "\n");

    takeoffTimePrev = takeoffTimeNew;
  }

  log_message("\n");

  return data.size() >= 10 || records > 3;
}


void
AiController::init()
{
  if ( mTrainingPhase == nullptr )
    mTrainingPhase = new TakeoffTrainingPhase();

  mTrainingPhase->init();
}

void
AiController::save()
{
  mTrainingPhase->save();
}

void
AiController::load()
{
  mTrainingPhase->load();
}

void
AiController::update()
{
  assert(mTrainingPhase != nullptr);

  mTrainingPhase->update();
}

void
AiController::processInput()
{
  assert(mTrainingPhase != nullptr);

  for ( auto& [planeType, plane] : planes )
  {
    if ( plane.isDead() == false && plane.isBot() == true )
      processPlaneControls(
        plane,
        mTrainingPhase->getInput(plane) );
  }
}
