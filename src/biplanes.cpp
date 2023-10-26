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

#include <include/biplanes.hpp>
#include <include/init_vars.hpp>
#include <include/sdl.hpp>
#include <include/time.hpp>
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
#include <include/sizes.hpp>
#include <include/sounds.hpp>
#include <include/stats.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>

#include <lib/Net.h>
#include <lib/picojson.h>
#include <TimeUtils/Duration.hpp>

using TimeUtils::Duration;


Packet localData {};
Packet opponentData {};
Packet opponentDataPrev {};

const uint16_t DEFAULT_LOCAL_PORT   = 55555;
const uint16_t DEFAULT_REMOTE_PORT  = 55555;
const std::string DEFAULT_SERVER_IP = "127.0.0.1";

//  Incompatible game versions must have different prefix
const std::string MMAKE_PASSWORD_PREFIX   = "biplanes1.1_";

uint16_t LOCAL_PORT   = DEFAULT_LOCAL_PORT;
uint16_t REMOTE_PORT  = DEFAULT_REMOTE_PORT;
std::string SERVER_IP = DEFAULT_SERVER_IP;
std::string MMAKE_PASSWORD {};

const uint8_t DEFAULT_WIN_SCORE = 10;


const static uint32_t TICK_RATE {240};
const static uint32_t PACKET_RATE {64};
static Duration packetSendTime {};

const static int32_t ProtocolId {0x11223344};
const static float ConnectionTimeout {10.0f};


Sizes sizes {};
Textures textures {};
Sounds sounds {};

Menu menu {};

std::map <PLANE_TYPE, Plane> planes
{
  {PLANE_TYPE::BLUE, {PLANE_TYPE::BLUE}},
  {PLANE_TYPE::RED, {PLANE_TYPE::RED}},
};


Controls controls_local {};
Controls controls_opponent {};

Effects effects {};
BulletSpawner bullets {};
std::vector <Cloud> clouds {};
Zeppelin zeppelin {};




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


  if ( SDL_init() != 0 )
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

  network.matchmaker = new MatchMaker();

  textures_load();
  sounds_load();
  init_sizes();


  const auto tickInterval = 1.0 / TICK_RATE;

  auto timePrevious = TimeUtils::Now();
  auto tickPrevious = timePrevious + tickInterval;

  const auto& game = gameState();

  log_message( "\nLOG: Reached main menu loop!\n\n" );


  while ( game.isExiting == false )
  {
    TimeUtils::SleepUntil(tickPrevious + tickInterval);

    const auto currentTime = TimeUtils::Now();
    deltaTime = static_cast <double> (currentTime - timePrevious);
    timePrevious = currentTime;

    network.matchmaker->Update();

    while ( SDL_PollEvent(&windowEvent) != 0 )
    {
      menu.ResizeWindow();
      menu.UpdateControls();
    }

    uint32_t ticks = 0;

    for ( ; currentTime >= tickPrevious + tickInterval;
            tickPrevious += tickInterval )
      ++ticks;

//    Fixed time step
    deltaTime = ticks * tickInterval;


//    TODO: split logic & rendering
    if ( ticks == 0 )
      continue;


    if (  menu.currentRoom() == ROOMS::GAME ||
          menu.currentRoom() == ROOMS::MENU_PAUSE )
    {
      if ( game.gameMode == GAME_MODE::HUMAN_VS_HUMAN )
        game_loop_mp();
      else
        game_loop_sp();
    }


    game_draw();
    menu.DrawMenu();

    display_update();
  }

  const auto connection = network.connection;

  if ( connection->IsConnected() == true )
  {
    sendDisconnectMessage();
    connection->Stop();
  }

  if ( gameState().output.stats == true )
    stats_write();

  SDL_close();

  return 0;
}

void
game_reset()
{
  init_sizes();

  erasePacket(localData);
  erasePacket(opponentData);
  erasePacket(opponentDataPrev);

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
}

bool
game_init_sp()
{
  auto& network = networkState();

  SDL_SetWindowResizable( gWindow, SDL_FALSE );

  network.nodeType = SRV_CLI::SERVER;

  gameState().isRoundFinished = false;
  network.connectionChanged = false;
  network.isOpponentConnected = true;
  game_reset();

  log_message( "\nLOG: Singleplayer game initialized successfully!\n\n" );

  menu.setMessage(MESSAGE_TYPE::SUCCESSFULL_CONNECTION);

  return 0;
}

bool
game_init_mp()
{
  auto& network = networkState();
  const auto connection = network.connection;


  SDL_SetWindowResizable( gWindow, SDL_FALSE );


  net::Address address {};
  const uint16_t port =
    network.nodeType == SRV_CLI::CLIENT
    ? REMOTE_PORT : LOCAL_PORT;


  if ( network.nodeType == SRV_CLI::CLIENT )
  {
    std::stringstream stream {SERVER_IP};
    int a, b, c, d;
    char ch;
    stream >> a >> ch >> b >> ch >> c >> ch >> d;

    address = net::Address {a, b, c, d, port};
  }


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

  gameState().winScore = DEFAULT_WIN_SCORE;
  gameState().isRoundFinished = false;
  network.connectionChanged = false;
  network.isOpponentConnected = false;

  network.flowControl = new net::FlowControl();
  packetSendTime = {};
  game_reset();

  log_message( "\nLOG: Multiplayer game initialized successfully!\n\n" );

  return 0;
}

void
game_loop_sp()
{
  if ( gameState().isPaused == true )
    return;


  auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
  auto& planeRed = planes.at(PLANE_TYPE::RED);

  const auto playerPlane =
    planeBlue.isBot() == false
    ? &planeBlue
    : planeRed.isBot() == false
      ? &planeRed
      : nullptr;


  if ( playerPlane != nullptr )
  {
    readLocalInput();
    processLocalControls(*playerPlane, controls_local);
  }

//  TODO: skip if game is paused
  for ( auto& [planeType, plane] : planes )
  {
    if ( plane.isBot() == true )
    {
      const auto inputs = plane.aiState();
//      TODO: train/process bot
    }
  }


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

  connection->Update(deltaTime);


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

        if ( network.nodeType == SRV_CLI::SERVER )
        {
          network.flowControl->Reset();
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
  }


  network.sendCoordsTimer.Update();


//  GET PACKET
  uint8_t packet[sizeof(Packet)] {};

  while ( connection->ReceivePacket( packet, sizeof(packet) ) > 0 )
  {
    if (  network.connectionChanged == false &&
          network.isOpponentConnected == false )
      network.connectionChanged = true;

//    drop JSON packets sent by MatchMaker p2p accept
    if ( packet[0] != '{' && packet[0] != '[' )
    {
      memcpy( &opponentData, &packet, sizeof(opponentData) );
      processOpponentData();
    }
//    TODO: remove these logs in release
    else
    {
      log_message("Discarding JSON packet: ");
      log_message("'", (const char*) packet, "'");
    }
  }


  if ( opponentData.disconnect == true )
  {
    network.connectionChanged = true;
    opponentData.disconnect = false;
  }


//  INPUT
  if ( game.isPaused == false )
    readLocalInput();

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

  processLocalControls(localPlane, controls_local);
  localPlane.Update();

  if ( network.isOpponentConnected == true )
  {
    readOpponentInput();

    processLocalControls(remotePlane, controls_opponent);

    remotePlane.Update();
  }

  zeppelin.Update();
  bullets.Update();
  effects.Update();


//  SEND PACKET
  packLocalData();

  const Duration packetSendInterval = 1.0 / PACKET_RATE;

  if ( packetSendTime >= packetSendInterval )
  {
    while ( packetSendTime >= packetSendInterval )
      packetSendTime -= packetSendInterval;

    memcpy( packet, &localData, sizeof(packet) );

    connection->SendPacket( packet, sizeof(packet) );
    eventsFinishIteration();
  }

  packetSendTime += deltaTime;


  if ( network.sendCoordsTimer.isReady() == true )
    network.sendCoordsTimer.Start();
}


// Rendering
void
game_draw()
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
    cloud.Update();
    cloud.Draw();
  }

  if ( networkState().isOpponentConnected == false )
    return;


  if (  planeBlue.isDead() == true ||
        planeRed.isDead() == true )
    draw_score();
}
