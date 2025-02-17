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

#include <include/biplanes.hpp>
#include <include/sdl.hpp>
#include <include/time.hpp>
#include <include/constants.hpp>
#include <include/resources.hpp>
#include <include/game_state.hpp>
#include <include/network.hpp>
#include <include/network_data.hpp>
#include <include/network_state.hpp>
#include <include/menu.hpp>
#include <include/render.hpp>
#include <include/plane.hpp>
#include <include/bullet.hpp>
#include <include/cloud.hpp>
#include <include/zeppelin.hpp>
#include <include/controls.hpp>
#include <include/matchmake.hpp>
#include <include/effects.hpp>
#include <include/canvas.hpp>
#include <include/sounds.hpp>
#include <include/stats.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>
#include <include/ai_stuff.hpp>

#include <lib/Net.h>
#include <lib/picojson.h>
#include <TimeUtils/Duration.hpp>

using TimeUtils::Duration;


const uint16_t DEFAULT_LOCAL_PORT   = 55555;
const uint16_t DEFAULT_REMOTE_PORT  = 55555;
const std::string DEFAULT_SERVER_IP = "127.0.0.1";

//  Incompatible game versions must have different prefix
const std::string MMAKE_PASSWORD_PREFIX   = "biplanes1.1_";

uint16_t LOCAL_PORT   = DEFAULT_LOCAL_PORT;
uint16_t REMOTE_PORT  = DEFAULT_REMOTE_PORT;
std::string SERVER_IP = DEFAULT_SERVER_IP;
std::string MMAKE_PASSWORD {};

static Duration packetSendTime {};

const static int32_t ProtocolId {0x11223344};
const static float ConnectionTimeout {10.0f};


Canvas canvas {};
Textures textures {};
Sounds sounds {};

Menu menu {};

std::map <PLANE_TYPE, Plane> planes
{
  {PLANE_TYPE::BLUE, {PLANE_TYPE::BLUE}},
  {PLANE_TYPE::RED, {PLANE_TYPE::RED}},
};


Effects effects {};
BulletSpawner bullets {};
std::vector <Cloud> clouds {};
Zeppelin zeppelin {};

AiController aiController {};


GameState&
gameState()
{
  static GameState state {};
  return state;
}

NetworkState&
networkState()
{
  static NetworkState state {};
  return state;
}


int
main(
  int argc,
  char* args[] )
{
  logVersionAndReadSettings();

  auto& game = gameState();

  if ( SDL_init(game.isVSyncEnabled, game.isAudioEnabled) != 0 )
  {
    log_message( "\n\nSDL Startup: SDL startup failed!\n" );

    return 1;
  }

  for ( auto& [planeType, plane] : planes )
  {
    plane.input.setPlane(&plane);
    plane.pilot.setPlane(&plane);
  }

  auto& network = networkState();

  network.connection = new net::ReliableConnection(
    ProtocolId, ConnectionTimeout );

  network.flowControl = new net::FlowControl();
  network.matchmaker = new MatchMaker();

  textures_load();
  sounds_load();


  const auto tickInterval = 1.0 / constants::tickRate;

  auto timePrevious = TimeUtils::Now();
  auto tickPrevious = timePrevious + tickInterval;

  const auto connection = network.connection;

  log_message( "\nLOG: Reached main menu loop!\n\n" );


  while ( game.isExiting == false )
  {
    TimeUtils::SleepUntil(tickPrevious + tickInterval);

    const auto currentTime = TimeUtils::Now();

    deltaTime = static_cast <double> (currentTime - timePrevious);

    timePrevious = currentTime;


    if ( connection->IsRunning() == true )
      connection->Update(deltaTime);

    network.matchmaker->Update();


    while ( SDL_PollEvent(&windowEvent) != 0 )
    {
      if ( windowEvent.type == SDL_QUIT )
      {
        game.isExiting = true;
        break;
      }

      queryWindowSize();
      readKeyboardInput();
      menu.UpdateControls();
    }


    uint32_t ticks = 0;

    for ( ; currentTime >= tickPrevious + tickInterval;
            tickPrevious += tickInterval )
      ++ticks;

//    Fixed time step
    deltaTime = ticks * tickInterval;


//    TODO: independent render frequency
    if ( ticks == 0 )
      continue;


    if ( gameState().isRoundRunning == true )
    {
      if ( game.gameMode == GAME_MODE::HUMAN_VS_HUMAN )
        game_loop_mp();
      else
        game_loop_sp();

//      this prevents sticky keys when next event poll returns nothing
      readKeyboardInput();

      draw_game();
    }

    menu.DrawMenu();
    draw_window_letterbox();

    display_update();
  }

  log_message("EXIT: Exit sequence initiated\n");

  if ( connection->IsConnected() == true )
  {
    sendDisconnectMessage();
    connection->Stop();
  }

  delete network.matchmaker;
  delete network.flowControl;
  delete network.connection;

  net::ShutdownSockets();

  if ( gameState().output.stats == true )
    stats_write();

  stopSound(-1);
  sounds_unload();
  textures_unload();

  SDL_close();

  return 0;
}

void
game_reset()
{
  if ( clouds.empty() == true )
  {
    clouds.resize(8);

    for ( uint8_t i = 0; i < clouds.size(); i++ )
      clouds[i] = {static_cast <bool> (i % 2), i};
  }

  eventsReset();

  for ( auto& [planeType, plane] : planes )
  {
    plane.Respawn();
    plane.ResetScore();
    plane.ResetStats();
  }

  for ( auto& [planeType, plane] : planes )
    plane.ResetSpawnProtection();

  zeppelin.Respawn();
  bullets.Clear();

  for ( auto& cloud : clouds )
    cloud.Respawn();

  effects.Clear();

  Mix_HaltChannel(-1);
}

bool
game_init_sp()
{
  auto& network = networkState();

  network.nodeType = SRV_CLI::SERVER;

  gameState().isRoundFinished = false;
  network.connectionChanged = false;
  network.isOpponentConnected = true;
  game_reset();

  aiController = {};
  aiController.init();

  log_message( "\nLOG: Singleplayer game initialized successfully!\n\n" );

  gameState().isRoundRunning = true;
  menu.setMessage(MESSAGE_TYPE::SUCCESSFULL_CONNECTION);

  return 0;
}

bool
game_init_mp()
{
  auto& network = networkState();
  const auto connection = network.connection;


  net::Address address {};
  const uint16_t port =
    network.nodeType == SRV_CLI::CLIENT
    ? REMOTE_PORT : LOCAL_PORT;


  if ( network.nodeType == SRV_CLI::CLIENT )
    address = net::Address::FromString(
      SERVER_IP, std::to_string(port) );


  if ( net::InitializeSockets() != 0 )
  {
    log_message( "NETWORK: Failed to initialize sockets!\n" );
    menu.setMessage(MESSAGE_TYPE::SOCKET_INIT_FAILED);

    return 1;
  }

  if ( network.nodeType == SRV_CLI::CLIENT )
  {
    if ( connection->Start(port - 1) == false )
    {
      log_message( "NETWORK: Failed to initialize multiplayer session", "\n" );
      menu.setMessage(MESSAGE_TYPE::CANT_START_CONNECTION);
      return 1;
    }

    connection->Connect(address);
    menu.setMessage(MESSAGE_TYPE::CLIENT_CONNECTING);
  }
  else
  {
    if ( connection->Start(port) == false )
    {
      log_message( "NETWORK: Failed to initialize multiplayer session", "\n" );
      menu.setMessage(MESSAGE_TYPE::CANT_START_CONNECTION);
      return 1;
    }

    connection->Listen();
    menu.setMessage(MESSAGE_TYPE::HOST_LISTENING);
  }

  gameState().winScore = constants::defaultWinScore;
  gameState().isRoundFinished = false;
  network.connectionChanged = false;
  network.isOpponentConnected = false;

  packetSendTime = {};
  game_reset();

  if ( gameState().debug.ai == true )
  {
    aiController = {};
    aiController.init();
  }

  log_message( "\nLOG: Multiplayer game initialized successfully!\n\n" );

  gameState().isRoundRunning = true;

  return 0;
}

void
game_loop_sp()
{
  auto& game = gameState();

  if ( game.isPaused == true )
    return;

  if ( game.debug.stepByStepMode == true )
  {
    if ( game.debug.advanceOneTick == false )
      return;

    game.debug.advanceOneTick = false;
  }


  auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  auto& planeRed = planes.at(PLANE_TYPE::RED);

  switch (game.gameMode)
  {
    case GAME_MODE::HUMAN_VS_BOT:
    {
      const auto playerPlane =
        planeBlue.isBot() == false
        ? &planeBlue
        : planeRed.isBot() == false
          ? &planeRed
          : nullptr;


      if ( playerPlane != nullptr )
        processPlaneControls(
          *playerPlane, getLocalControls() );

      break;
    }

    case GAME_MODE::HUMAN_VS_HUMAN_HOTSEAT:
    {
      processPlaneControls(
        planeBlue, getLocalControls(bindings::player2) );

      processPlaneControls(
        planeRed, getLocalControls(bindings::player1) );

      break;
    }

    default:
      break;
  }

  aiController.update();


  for ( auto& cloud : clouds )
    cloud.Update();

  for ( auto& [planeType, plane] : planes )
    plane.Update();

  zeppelin.Update();
  bullets.Update();
  effects.Update();
}

void
game_loop_mp()
{
  auto& game = gameState();
  auto& network = networkState();
  const auto connection = network.connection;


  if ( connection->ConnectHasErrors() == true )
  {
    if ( connection->ConnectFailed() == true )
      menu.setMessage(MESSAGE_TYPE::CONNECTION_FAILED);

    else if ( connection->ConnectTimedOut() == true )
      menu.setMessage(MESSAGE_TYPE::CONNECTION_TIMED_OUT);

    if ( network.nodeType == SRV_CLI::CLIENT )
    {
      connection->Stop();
      menu.ReturnToMainMenu();

      return;
    }
  }


  if ( connection->IsConnected() == true )
  {
    network.flowControl->Update(
      connection->GetReliabilitySystem().GetRoundTripTime() * 1000.0f,
      deltaTime );

    if ( network.connectionChanged == true )
    {
      network.connectionChanged = false;

      if ( network.isOpponentConnected == true )
      {
        network.isOpponentConnected = false;
        network.flowControl->Reset();

        if ( network.nodeType == SRV_CLI::SERVER )
        {
          connection->Listen();
          menu.setMessage(MESSAGE_TYPE::CLIENT_DISCONNECTED);
          game_reset();
        }
        else
        {
          connection->Stop();
          menu.setMessage(MESSAGE_TYPE::HOST_CEASED_CONNECTION);
          menu.ReturnToMainMenu();

          return;
        }
      }
      else
      {
        network.isOpponentConnected = true;
        menu.setMessage(MESSAGE_TYPE::SUCCESSFULL_CONNECTION);

        game.isRoundFinished = false;
        game_reset();
      }
    }
    else
    {
      if ( network.flowControl->IsConnectionStable() == true )
      {
        if ( menu.currentMessage() == MESSAGE_TYPE::CONNECTION_UNSTABLE )
          menu.setMessage(MESSAGE_TYPE::NONE);
      }
      else if ( menu.currentMessage() == MESSAGE_TYPE::NONE )
        menu.setMessage(MESSAGE_TYPE::CONNECTION_UNSTABLE);
    }
  }


  network.sendCoordsTimer.Update();


//  GET PACKET
  static Packet opponentData {};
  uint8_t packet[sizeof(Packet)] {};

  while ( connection->ReceivePacket( packet, sizeof(packet) ) > 0 )
  {
    if (  network.connectionChanged == false &&
          network.isOpponentConnected == false )
      network.connectionChanged = true;

    const auto opponentDataPrev = opponentData;

    memcpy( &opponentData, &packet, sizeof(opponentData) );

    if ( opponentData.disconnect == false )
      processOpponentData(opponentData, opponentDataPrev);
  }


  if ( opponentData.disconnect == true )
  {
    network.connectionChanged = true;
    opponentData = {};
  }

  for ( auto& cloud : clouds )
    cloud.Update();


//  INPUT
  Controls controlsLocal {};

  if ( game.isPaused == false )
    controlsLocal = getLocalControls();

  auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  auto& planeRed = planes.at(PLANE_TYPE::RED);

  auto& localPlane =
    planeBlue.isLocal() == true
    ? planeBlue
    : planeRed;

  auto& remotePlane =
    planeBlue.isLocal() == false
    ? planeBlue
    : planeRed;

  processPlaneControls(localPlane, controlsLocal);
  localPlane.Update();

  if ( network.isOpponentConnected == true )
  {
    const Controls opponentControls
    {
      opponentData.pitch,
      opponentData.throttle,
    };

    processPlaneControls(remotePlane, opponentControls);
    remotePlane.Update();
  }

  if ( gameState().debug.ai == true )
    aiController.update();

  zeppelin.Update();
  bullets.Update();
  effects.Update();


//  SEND PACKET
  const Duration packetSendInterval = 1.0 / constants::packetSendRate;

  if ( packetSendTime >= packetSendInterval )
  {
    while ( packetSendTime >= packetSendInterval )
      packetSendTime -= packetSendInterval;

    Packet localData {};

    localData << controlsLocal;
    localData << localPlane.getNetworkData();

    eventsPack(localData);

    memcpy( packet, &localData, sizeof(packet) );

    connection->SendPacket( packet, sizeof(packet) );
    eventsNewTick();
  }

  packetSendTime += deltaTime;


  if ( network.sendCoordsTimer.isReady() == true )
    network.sendCoordsTimer.Start();
}


// Rendering
void
draw_game()
{
  draw_background();

  zeppelin.Draw();
  bullets.Draw();


  const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  const auto& planeRed = planes.at(PLANE_TYPE::RED);

  const auto& playerPlane =
    planeBlue.isLocal() == true && planeBlue.isBot() == false
    ? planeBlue
    : planeRed;

  const auto& opponentPlane =
    &planeBlue == &playerPlane
    ? planeRed
    : planeBlue;


  if ( networkState().isOpponentConnected == true )
  {
    opponentPlane.Draw();
    opponentPlane.pilot.Draw();
  }

  playerPlane.Draw();
  playerPlane.pilot.Draw();


  draw_barn();

  effects.Draw();

  for ( auto& cloud : clouds )
  {
    cloud.setOpaque();

    if ( playerPlane.isInCloud(cloud) == true ||
         opponentPlane.isInCloud(cloud) == true )
      cloud.setTransparent();

    cloud.Draw();
  }


  if ( networkState().isOpponentConnected == true )
  {
    if (  planeBlue.isDead() == true ||
          planeRed.isDead() == true )
      draw_score();
  }


  if ( gameState().debug.collisions == true )
  {
    draw_ground_collision_layer();
    draw_barn_collision_layer();

    playerPlane.DrawCollisionLayer();
    playerPlane.pilot.DrawCollisionLayer();

    opponentPlane.DrawCollisionLayer();
    opponentPlane.pilot.DrawCollisionLayer();

    for ( auto& cloud : clouds )
      cloud.DrawCollisionLayer();
  }

  if ( gameState().debug.ai == true )
    aiController.drawDebugLayer();
}
