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


float
calcReward(
  const std::vector <float>& inputs,
  const AiAction output )
{
  namespace PlaneIndices = AiDatasetPlaneIndices;
  namespace BulletIndices = AiDatasetBulletIndices;

  const auto selfIndex = AiDatasetIndices::SelfState;
  const auto opponentIndex = AiDatasetIndices::OpponentState;


  const bool canAccelerate =
    inputs.at(selfIndex + PlaneIndices::CanAccelerate) > 0.5f;

  const bool canDecelerate =
    inputs.at(selfIndex + PlaneIndices::CanDecelerate) > 0.5f;

  const bool canTurn =
    inputs.at(selfIndex + PlaneIndices::CanTurn) > 0.5f;

  const bool canShoot =
    inputs.at(selfIndex + PlaneIndices::CanShoot) > 0.5f;

  const bool canJump =
    inputs.at(selfIndex + PlaneIndices::CanJump) > 0.5f;


  const bool isDead =
    inputs.at(selfIndex + PlaneIndices::IsDead) > 0.5f;

  const bool isOnGround =
    inputs.at(selfIndex + PlaneIndices::IsOnGround) > 0.5f;

  const auto damage =
    inputs.at(selfIndex + PlaneIndices::Damage);


  if ( isDead == true )
    return 0.0f;


  switch (output)
  {
    case AiAction::Idle:
    {
      if ( isOnGround == true )
        return 0.4f;

      break;
    }

    case AiAction::Accelerate:
    {
      if ( canAccelerate == false )
        return 0.45f;

      break;
    }

    case AiAction::Decelerate:
    {
      if ( canDecelerate == false )
        return 0.45f;

      break;
    }

    case AiAction::TurnLeft:
    case AiAction::TurnRight:
    {
      if ( canTurn == false )
        return 0.45f;

      break;
    }

    case AiAction::Shoot:
    {
      if ( canShoot == false )
        return 0.45f;

      break;
    }

    case AiAction::Jump:
    {
      if ( canJump == false )
        return 0.45f;

      if ( damage * constants::plane::maxHp < 1.0f )
        return 0.0f;

      break;
    }

    default:
      break;
  }

  const auto opponendIsDead =
    inputs.at(opponentIndex + PlaneIndices::IsDead);

  const auto opponentDamage =
    inputs.at(opponentIndex + PlaneIndices::Damage);

  if ( opponendIsDead < 0.5f && opponentDamage > 0.0f )
    return 0.5f + 0.5f * opponentDamage;


  float dodgeReward {};

  const auto selfX = inputs.at(selfIndex + PlaneIndices::PosX);
  const auto selfY = inputs.at(selfIndex + PlaneIndices::PosY);

  for ( size_t i = 0; i < BulletIndices::BulletCount; ++i )
  {
    const size_t bulletIndex =
      AiDatasetIndices::Bullets + i;

    const auto bulletX =
      inputs.at(bulletIndex + BulletIndices::PosX);

    const auto bulletY =
      inputs.at(bulletIndex + BulletIndices::PosY);

    if ( bulletX == 0.0f && bulletY == 0.0f )
      continue;


    const auto distance = sqrt(
      pow(bulletX - selfX, 2.f) +
      pow(bulletY - selfY, 2.f) );

    dodgeReward += distance / sqrt(2.0f);
  }

  dodgeReward /= BulletIndices::BulletCount;

  if ( dodgeReward > 0.0f )
    return 0.5f + 0.5f * dodgeReward;


  return 0.5f;
}

std::vector <float>
calcRewards(
  const std::vector <float>& inputs,
  const std::vector <AiAction>& actions )
{
  std::vector <float> rewards {};

  for ( size_t action = 0;
        action < static_cast <size_t> (AiAction::ActionCount);
        ++action )
  {
    const auto reward = calcReward(
      inputs, static_cast <AiAction> (action) );

    assert(reward >= 0.0f && reward <= 1.0f);

    rewards.push_back(reward);
  }

  return rewards;
}

std::vector <float>
predictRewards(
  const std::vector <float>& inputs,
  const AI_Backend& backend )
{
  std::vector <float> rewards {};

  AiDatasetEntry stateEntry {inputs};

  for ( size_t action = 0;
        action < static_cast <size_t> (AiAction::ActionCount);
        ++action )
  {
    stateEntry.action = static_cast <AiAction> (action);

    const auto state = stateEntry.merge();

    const auto reward = backend.predictReward(
      {state.begin(), state.end()} );

    assert(reward >= 0.0f && reward <= 1.0f);

    rewards.push_back(reward);
  }

  return rewards;
}

std::vector <AiRewardedAction>
predictActionRewards(
  const std::vector <float>& inputs,
  const AI_Backend& backend )
{
  std::vector <AiRewardedAction> rewards {};

  AiDatasetEntry stateEntry {inputs};

  for ( size_t action = 0;
        action < static_cast <size_t> (AiAction::ActionCount);
        ++action )
  {
    stateEntry.action = static_cast <AiAction> (action);

    const auto state = stateEntry.merge();

    const auto reward = backend.predictReward(
      {state.begin(), state.end()} );

    rewards.push_back({stateEntry.action, reward});
  }

  return rewards;
}


std::vector <float>
AiDatasetEntry::merge() const
{
  auto state = inputs;

  const auto stateSize =
    inputs.size() + static_cast <size_t> (AiAction::ActionCount);

  state.resize(stateSize);

  state[inputs.size() + static_cast <size_t> (action)] = 1.0f;

  return state;
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
  const AiAction action )
{
  mData.push_back({inputs, action});
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
      assert(inputRhs[i] >= 0.0f && inputRhs[i] <= 1.0f);

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

      if ( mData[iLhs].action != mData[iLhs + 1].action )
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
  if ( mData.empty() == true || mData.size() < n )
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

AiRewardDataset
AiDataset::toRewardDataset(
  const AI_Backend& backend ) const
{
  if ( mData.size() < 2 )
    return {};


  const float discountFactor = 0.6f;

  AiRewardDataset result {};

  for ( size_t i = 0; i < mData.size() - 1; ++i )
  {
    auto& currentEntry = mData[i];
    auto& nextEntry = mData[i + 1];

    const auto state = currentEntry.merge();

    const std::vector <AiAction> actions
    {
      AiAction::Idle,
      AiAction::Accelerate,
      AiAction::Decelerate,
      AiAction::TurnLeft,
      AiAction::TurnRight,
      AiAction::Shoot,
      AiAction::Jump,
    };

    const auto nextRewards = predictRewards(
      nextEntry.inputs, backend );

    const auto nextReward = *std::max_element(
      nextRewards.begin(), nextRewards.end() );

    const auto reward =
      (1.0f - discountFactor) * calcReward(currentEntry.inputs, currentEntry.action) +
      discountFactor * nextReward;

    assert(reward >= 0.0f && reward <= 1.0f);

    result.states.push_back({state.cbegin(), state.cend()});
    result.rewards.push_back({reward});
  }

  assert(result.states.size() == result.rewards.size());

  return result;
}

void
AiDataset::printActionStats() const
{
  std::map <AiAction, size_t> actionStatsMap {};

  for ( const auto& entry : mData )
    actionStatsMap[entry.action]++;


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

  const auto rewardDataset = dataset.toRewardDataset(backend);

  size_t epochs = backend.getTrainedEpochsCount();

  float currentLoss = backend.getLoss(
    rewardDataset.states,
    rewardDataset.rewards );

  float prevLoss {};


  do
  {
    log_message("Epoch " + std::to_string(epochs++) + " loss: " + std::to_string(currentLoss) + "\n");

    prevLoss = currentLoss;

    backend.train(
      rewardDataset.states,
      rewardDataset.rewards,
      std::min(batchSize, rewardDataset.rewards.size()),
      1 );

    currentLoss = backend.getLoss(
      rewardDataset.states,
      rewardDataset.rewards );
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

//  if ( plane.isDead() == true )
//    return {{}, AiAction::Idle};


  auto rewards = predictActionRewards(
    inputs, backend );

  std::sort( rewards.begin(), rewards.end(),
  [] ( const AiRewardedAction& lhs, const AiRewardedAction& rhs )
  {
    return lhs.reward > rhs.reward;
  });

  std::vector <size_t> outputs {};

  for ( const auto& rewardedAction : rewards )
    outputs.push_back(
      static_cast <size_t> (rewardedAction.action) );

//  filterActions(plane, outputs);

  const auto action =
    static_cast <AiAction> (outputs.front());

  return {inputs, action};
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


class QLearningPhase : public AiTrainingPhase
{
  std::map <PLANE_TYPE, std::shared_ptr <AI_Backend>> mBackends
  {
    {PLANE_TYPE::BLUE, std::make_shared <AI_Backend> ()},
    {PLANE_TYPE::RED, std::make_shared <AI_Backend> ()},
  };

  std::map <PLANE_TYPE, AiDataset> mData
  {
    {PLANE_TYPE::BLUE, {}},
    {PLANE_TYPE::RED, {}},
  };

  std::map <PLANE_TYPE, float> mRewards
  {
    {PLANE_TYPE::BLUE, {}},
    {PLANE_TYPE::RED, {}},
  };

  Timer mRoundTimeout {3.0};

  size_t mCurrentRound {};
  const size_t mRoundsPerGeneration {3};


public:
  QLearningPhase() = default;

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
QLearningPhase::update()
{
  mRoundTimeout.Update();


  if ( hasRoundFinished() == false )
    return;


  ++mCurrentRound;
  bool newGeneration {};

  for ( const auto& [planeType, plane] : planes )
  {
    auto& dataset = mData[planeType];

    if ( isReadyForTraining(plane) == false )
      continue;


    log_message("Plane " + std::to_string(planeType) + " action stats:\n");
    dataset.printActionStats();

    log_message("\n");

    log_message("Training plane " + std::to_string(planeType) + "\n");
    train( *mBackends.at(planeType), dataset );

    dataset = {};
    newGeneration = true;

    log_message("\n");
  }

  if ( newGeneration == true )
    mCurrentRound = 0;

  initNewRound();
}

Controls
QLearningPhase::getInput(
  const Plane& plane )
{
  const auto datasetEntry = mInputProcessor->filterInput(
    plane.aiState(),
    plane,
    *mBackends.at(plane.type()) );

  mData[plane.type()].push(datasetEntry);

  const auto reward = calcReward(
    datasetEntry.inputs,
    datasetEntry.action );

  mRewards[plane.type()] += (-1.0f + reward * 2.0f);
  log_message(
    "plane " + std::to_string(plane.type()) +
    ": " + std::to_string((size_t) (datasetEntry.action)) +
    ": " + std::to_string(reward) +
    "/" + std::to_string(mRewards[plane.type()]) + "\n");

  return aiActionToControls(datasetEntry.action);
}

void
QLearningPhase::init()
{
  if ( mInputProcessor == nullptr )
    mInputProcessor = new AiInputFilter();

  for ( auto& [planeType, backend] : mBackends )
    backend->initNet();

  for ( auto& [planeType, data] : mData )
    data = {};

  initNewRound();
  mCurrentRound = 0;
}

void
QLearningPhase::initNewRound()
{
  mRoundTimeout.Start();

  mRewards[PLANE_TYPE::BLUE] = 0.0f;
  mRewards[PLANE_TYPE::RED] = 0.0f;

  game_reset();

  for ( auto& [planeType, plane] : planes )
    plane.randomizeState();
}

void
QLearningPhase::save() const
{
  auto& blueBackend = mBackends.at(PLANE_TYPE::BLUE);
  auto& redBackend = mBackends.at(PLANE_TYPE::RED);

  log_message("Saving blue AI model " + std::to_string(blueBackend->getWeights().size()) + "\n");
  blueBackend->saveModel(AI_BLUE_PATH);

  log_message("Saving red AI model " + std::to_string(redBackend->getWeights().size()) + "\n");
  redBackend->saveModel(AI_RED_PATH);
}

void
QLearningPhase::load()
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
QLearningPhase::hasRoundFinished() const
{
  return mRoundTimeout.isReady();
}

bool
QLearningPhase::hasPhaseFinished() const
{
  return false;
}

bool
QLearningPhase::isReadyForTraining(
  const Plane& plane ) const
{
  return mCurrentRound >= mRoundsPerGeneration;
}


void
AiController::init()
{
  if ( mTrainingPhase == nullptr )
    mTrainingPhase = new QLearningPhase();

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
    if ( plane.isBot() == true )
      processPlaneControls(
        plane,
        mTrainingPhase->getInput(plane) );
  }
}
