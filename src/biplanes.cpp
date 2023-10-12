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

#include <lib/Net.h>
#include <lib/picojson.h>

#include <include/init_vars.h>
#include <include/controls.h>
#include <include/render.h>
#include <include/matchmake.hpp>

#include <fstream>
#include <sstream>


bool consoleOutput  = false;
bool logFileOutput  = true;
bool statsOutput    = true;
bool show_hitboxes  = false;

bool game_exit          = false;
bool game_pause         = false;
bool game_finished      = false;
bool sound_initialized  = false;

SRV_CLI srv_or_cli = SRV_CLI::SERVER;

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
  double StartingTime = 0.0, EndingTime = 0.0, Frequency = 0.0;
#elif PLATFORM == PLATFORM_WINDOWS
  LARGE_INTEGER StartingTime, EndingTime, Frequency;
#endif

double deltaTime = 0.0;

Packet opponent_data, opponent_data_prev;
Packet local_data;
bool opponent_connected = false;
bool connection_changed = false;

const unsigned short DEFAULT_HOST_PORT    = 55555;
const std::string DEFAULT_SERVER_IP       = "127.0.0.1";
const unsigned short DEFAULT_SERVER_PORT  = 55555;
const std::string DEFAULT_MMAKE_PASSWORD  = "";
const std::string MMAKE_PASSWORD_PREFIX   = "biplanes1.0_";

const bool DEFAULT_HARDCORE_MODE = false;

unsigned short HOST_PORT    = DEFAULT_HOST_PORT;
std::string SERVER_IP       = DEFAULT_SERVER_IP;
unsigned short SERVER_PORT  = DEFAULT_SERVER_PORT;
std::string MMAKE_PASSWORD  = DEFAULT_MMAKE_PASSWORD;

bool HARDCORE_MODE = DEFAULT_HARDCORE_MODE;

const int ProtocolId  = 0x11223344;
const float TimeOut   = 10.0f;

const unsigned int FRAMERATE = 240; // Actually game ticks per second

double TICK_RATE  = 64.0;
double TICK_TIME  = 1.0 / TICK_RATE;
double ticktime   = 0.0;
net::ReliableConnection *connection = nullptr;
net::FlowControl *flowControl       = nullptr;
const int PacketSize                = sizeof( Packet ) + 12;

double send_coords_time = 0.0;

SDL_Event event;
const SDL_Keycode DEFAULT_THROTTLE_UP   = SDLK_UP;
const SDL_Keycode DEFAULT_THROTTLE_DOWN = SDLK_DOWN;
const SDL_Keycode DEFAULT_TURN_LEFT     = SDLK_LEFT;
const SDL_Keycode DEFAULT_TURN_RIGHT    = SDLK_RIGHT;
const SDL_Keycode DEFAULT_FIRE          = SDLK_SPACE;
const SDL_Keycode DEFAULT_JUMP          = SDLK_LCTRL;

SDL_Keycode THROTTLE_UP   = SDLK_UP;
SDL_Keycode THROTTLE_DOWN = SDLK_DOWN;
SDL_Keycode TURN_LEFT     = SDLK_LEFT;
SDL_Keycode TURN_RIGHT    = SDLK_RIGHT;
SDL_Keycode FIRE          = SDLK_SPACE;
SDL_Keycode JUMP          = SDLK_LCTRL;

Sizes sizes;
Textures textures;
Sounds sounds;

Menu menu;
Plane plane_blue( PLANE_TYPE::BLUE );
Plane plane_red( PLANE_TYPE::RED );
Controls controls_local, controls_opponent;
Statistics stats_recent, stats_total;

BulletSpawner bullets;
std::vector <Cloud> clouds;
Zeppelin zeppelin;


bool sentGameParams = false;
bool eventNewIterationLoc = true;
std::deque <unsigned char> eventsLocal( 32, 'n' );

unsigned char eventCounterLoc = 0;
unsigned char eventCounterOpp = 0;

void DrawGame();


int main( int argc, char *args[] )
{
  // Print game version and read user settings
  logVerReadSettings();


  // Initialize SDL
  if ( SDL_init() )
  {
    log_message( "\n\nSDL Startup: SDL startup failed!\n" );

    return 1;
  }
  connection = new net::ReliableConnection( ProtocolId, TimeOut );

  textures_load();
  sounds_load();
  init_vars();

  log_message( "\nLOG: Reached main menu loop!\n\n" );

  Timer fpsTimer( 1.0f / FRAMERATE );
  fpsTimer.Start();

  while ( !game_exit )
  {
    countDelta();
    fpsTimer.Update();
    if ( !fpsTimer.isReady() )
    {
      SDL_Delay( fpsTimer.remainderTime() * 1000 );
      fpsTimer.Start();
    }
    else
      fpsTimer.Start();

    MatchMaker::Inst().update();

    if ( SDL_PollEvent( &event ) )
    {
      menu.ResizeWindow();
      menu.UpdateControls();
    }

    menu.DrawMenu();
    display_update();
  }
  if ( connection->IsConnected() )
  {
    connection->SendDisconnectMessage();
    connection->Stop();
  }

  if ( statsOutput )
    stats_write();

  SDL_close();

  return 0;
}

void game_reset()
{
  init_vars();

  erasePacket( local_data );
  erasePacket( opponent_data );
  erasePacket( opponent_data_prev );

  events_reset();

  plane_blue.Respawn();
  plane_red.Respawn();
  plane_blue.ResetSpawnProtection();
  plane_red.ResetSpawnProtection();

  plane_blue.ResetScore();
  plane_red.ResetScore();

  zeppelin.Respawn();
  bullets.Clear();

  for ( Cloud& cloud : clouds )
    cloud.Respawn();
}

bool game_init_mp()
{
  SDL_SetWindowResizable( gWindow, SDL_FALSE );

  net::Address address;
  int port = srv_or_cli == SRV_CLI::CLIENT ? SERVER_PORT : HOST_PORT;

  if ( srv_or_cli == SRV_CLI::CLIENT )
  {
    std::stringstream s( SERVER_IP );
    int a, b, c, d;
    char ch;
    s >> a >> ch >> b >> ch >> c >> ch >> d;

    address = net::Address( a, b, c, d, port );
  }

  if ( net::InitializeSockets() )
  {
    log_message( "NETWORK: Failed to initialize sockets!\n" );
    menu.setMessage( MESSAGE_TYPE::SOCKET_INIT_FAILED );

    return 1;
  }

  if ( srv_or_cli == SRV_CLI::CLIENT )
  {
    if ( !connection->Start( port - 1 ) )
      return 1;

    connection->Connect( address );
    menu.setMessage( MESSAGE_TYPE::CLIENT_CONNECTING );
  }
  else
  {
    if ( !connection->Start( port ) )
      return 1;

    connection->Listen();
    menu.setMessage( MESSAGE_TYPE::HOST_LISTENING );
  }

  opponent_connected = false;
  connection_changed = false;
  game_finished = false;
  flowControl = new net::FlowControl();
  game_reset();
  stats_recent = Statistics();

  log_message( "\nLOG: Multiplayer game initialized successfully!\n\n" );

  return 0;
}

bool game_init_sp()
{
  SDL_SetWindowResizable( gWindow, SDL_FALSE );

  aiController.setActive( true );

  srv_or_cli = SRV_CLI::SERVER;

  opponent_connected = true;
  connection_changed = false;
  game_finished = false;
  game_reset();
  stats_recent = Statistics();

  log_message( "\nLOG: Singleplayer game initialized successfully!\n\n" );

  menu.setMessage( MESSAGE_TYPE::SUCCESSFULL_CONNECTION );

  return 0;
}

void game_loop_mp()
{
  connection->Update();

  if ( connection->ConnectFailed() && srv_or_cli == SRV_CLI::CLIENT )
  {
    connection->Stop();
    menu.ReturnToMainMenu();

    return;
  }

  if ( connection->IsConnected() )
  {
    flowControl->Update( connection->GetReliabilitySystem().GetRoundTripTime() * 1000.0f );
    if ( connection_changed )
    {
      connection_changed = false;

      if ( opponent_connected )
      {
        opponent_connected = false;

        if ( srv_or_cli == SRV_CLI::SERVER )
        {
          flowControl->Reset();
          menu.setMessage( MESSAGE_TYPE::CLIENT_DISCONNECTED );
          game_reset();
        }
        else
        {
          connection->Stop();
          menu.setMessage( MESSAGE_TYPE::HOST_CEASED_CONNECTION );
          menu.ReturnToMainMenu();
          return;
        }
      }
      else
      {
        opponent_connected = true;
        menu.setMessage( MESSAGE_TYPE::SUCCESSFULL_CONNECTION );

        game_finished = false;
        stats_update();

        game_reset();
      }
    }
  }


//  GET PACKET
  unsigned char packet_opponent[PacketSize];
  if ( connection->ReceivePacket( packet_opponent, sizeof( packet_opponent ) ) > 0 )
  {
    if ( !connection_changed && !opponent_connected )
      connection_changed = true;

    memcpy( &opponent_data, &packet_opponent, sizeof( opponent_data ) );
  }

  transform_opponent_data();


  if ( opponent_data.disconnect )
  {
    connection_changed = true;
    opponent_data.disconnect = false;
  }

  // Controls
  if ( !game_pause )
    collect_local_input();

  if ( opponent_connected )
  {
    collect_opponent_input();
    if ( srv_or_cli == SRV_CLI::CLIENT )  // red plane controls
    {
      process_local_controls( plane_blue, controls_opponent );
      process_local_controls( plane_red,  controls_local );
    }
    else  // blue plane controls
    {
      process_local_controls( plane_blue, controls_local );
      process_local_controls( plane_red,  controls_opponent );
    }
    controls_opponent.fire = false;

    plane_blue.Update();
    plane_red.Update();
  }
  else
  {
    if ( srv_or_cli == SRV_CLI::CLIENT )  // red plane controls
    {
      process_local_controls( plane_red, controls_local );
      plane_red.Update();
    }
    else  // blue plane controls
    {
      process_local_controls( plane_blue, controls_local );
      plane_blue.Update();
    }
  }

  bullets.UpdateBullets();


//  SEND PACKET
  prepare_local_data();
  if ( ticktime >= TICK_TIME )
  {
    ticktime -= TICK_TIME;

    unsigned char packet_local[PacketSize];
    memcpy( packet_local, &local_data, sizeof( local_data ) );

    connection->SendPacket( packet_local, sizeof( packet_local ) );
    eventNewIterationLoc = true;
  }
  ticktime += deltaTime;

  if ( send_coords_time >= 0.25 )
    send_coords_time = 0.0;

  send_coords_time += deltaTime;

  // Rendering
  DrawGame();
}


// Rendering
void DrawGame()
{
  draw_background();

  zeppelin.Update();
  bullets.Draw();

  if ( opponent_connected )
  {
    if ( srv_or_cli == SRV_CLI::CLIENT )
    {
      plane_red.AnimationsUpdate();
      plane_blue.AnimationsUpdate();
    }
    else
    {
      plane_blue.AnimationsUpdate();
      plane_red.AnimationsUpdate();
    }
  }
  else
  {
    if ( srv_or_cli == SRV_CLI::CLIENT )
      plane_red.AnimationsUpdate();
    else
      plane_blue.AnimationsUpdate();
  }

  draw_barn();
  for ( Cloud &cloud : clouds )
  {
    cloud.Update();
    cloud.Draw();
  }

  // Display score
  if (  opponent_connected &&
      ( plane_blue.isDead() ||
        plane_red.isDead() ) )
          draw_score();
}

// Pack local controls
void prepare_local_data()
{
  local_data.throttle = controls_local.throttle;
  local_data.pitch    = controls_local.pitch;

  prepare_plane_coords();

  if ( !sentGameParams )
  {
    if ( !HARDCORE_MODE )
      event_push( (unsigned char) EVENTS::NO_HARDCORE );
    sentGameParams = true;
  }

  events_pack();
}

// Pack local coordinates
void prepare_plane_coords()
{
  Plane_Data data;
  if ( srv_or_cli == SRV_CLI::CLIENT )
    data = plane_red.getData();
  else
    data = plane_blue.getData();

  local_data.x         = data.x / sizes.screen_width;
  local_data.y         = data.y / sizes.screen_height;
  local_data.dir       = data.dir;
  local_data.pilot_x   = data.pilot_x / sizes.screen_width;
  local_data.pilot_y   = data.pilot_y / sizes.screen_height;
}

// Add local event to queue
void event_push( unsigned char newEvent )
{
  if ( !opponent_connected )
    return;

  if ( eventNewIterationLoc )
  {
    eventsLocal.pop_front();
    eventsLocal.push_back( eventCounterLoc );
    eventNewIterationLoc = false;

    if ( ++eventCounterLoc > 63 )
      eventCounterLoc = 0;
  }
  eventsLocal.pop_front();
  eventsLocal.push_back( newEvent );
}

// Pack local events
void events_pack()
{
  for ( unsigned char i = 0; i < 32; i++ )
    local_data.events[i] = eventsLocal.at(i);
}

void events_reset()
{
  eventCounterLoc = 0;
  eventCounterOpp = 0;
  eventNewIterationLoc = true;
  sentGameParams = false;

  eventsLocal.clear();
  eventsLocal.resize( 32, 'n' );
}

// Unpack opponent data
void transform_opponent_data()
{
  Plane_Data data;
  data.dir        = opponent_data.dir;
  data.pilot_x    = opponent_data.pilot_x * sizes.screen_width;
  data.pilot_y    = opponent_data.pilot_y * sizes.screen_height;

  if ( srv_or_cli == SRV_CLI::CLIENT )
  {
    plane_blue.setDir( data.dir );
    plane_blue.pilot.setX( data.pilot_x );
    plane_blue.pilot.setY( data.pilot_y );
  }
  else
  {
    plane_red.setDir( data.dir );
    plane_red.pilot.setX( data.pilot_x );
    plane_red.pilot.setY( data.pilot_y );
  }

  if ( !std::equal( std::begin( opponent_data.events ),
                    std::end(   opponent_data.events ),
                    std::begin( opponent_data_prev.events ) ) )
  {
    bool eventNewIterationOpp = true;
    for ( unsigned char i = 0; i < sizeof( opponent_data.events ); i++ )
    {
      if ( eventNewIterationOpp )
      {
        if ( opponent_data.events[i] == eventCounterOpp )
          eventNewIterationOpp = false;

        continue;
      }

      switch ( opponent_data.events[i] )
      {
        case (unsigned int) EVENTS::NONE:
          continue;

        case (unsigned int) EVENTS::NO_HARDCORE:
        {
          HARDCORE_MODE = false;

          continue;
        }
        case (unsigned int) EVENTS::SHOOT:
        {
          controls_opponent.fire = true;

          continue;
        }
        case (unsigned int) EVENTS::EJECT:
        {
          if ( srv_or_cli == SRV_CLI::SERVER )
            plane_red.input.Jump();
          else
            plane_blue.input.Jump();

          continue;
        }
        case (unsigned int) EVENTS::HIT_PLANE:
        {
          if ( srv_or_cli == SRV_CLI::SERVER )
            plane_red.Hit( (int) srv_or_cli );
          else
            plane_blue.Hit( (int) srv_or_cli );

          continue;
        }
        case (unsigned int) EVENTS::HIT_CHUTE:
        {
          if ( srv_or_cli == SRV_CLI::SERVER )
            plane_red.pilot.ChuteHit();
          else
            plane_blue.pilot.ChuteHit();

          continue;
        }
        case (unsigned int) EVENTS::HIT_PILOT:
        {
          if ( srv_or_cli == SRV_CLI::SERVER )
            plane_red.pilot.Kill( (int) srv_or_cli );
          else
            plane_blue.pilot.Kill( (int) srv_or_cli );

          continue;
        }
        case (unsigned int) EVENTS::PLANE_DEATH:
        {
          if ( srv_or_cli == SRV_CLI::SERVER )
            plane_red.Crash();
          else
            plane_blue.Crash();

          continue;
        }
        case (unsigned int) EVENTS::PILOT_DEATH:
        {
          if ( srv_or_cli == SRV_CLI::SERVER )
          {
            plane_red.pilot.Death();
            plane_red.ScoreChange( -1 );
          }
          else
          {
            plane_blue.pilot.Death();
            plane_blue.ScoreChange( -1 );
          }

          continue;
        }
        case (unsigned int) EVENTS::PLANE_RESP:
        {
          if ( srv_or_cli == SRV_CLI::SERVER )
          {
            plane_red.Respawn();
            plane_red.ResetSpawnProtection();
          }
          else
          {
            plane_blue.Respawn();
            plane_blue.ResetSpawnProtection();
          }

          continue;
        }
        case (unsigned int) EVENTS::PILOT_LAND:
        {
          if ( srv_or_cli == SRV_CLI::SERVER )
            plane_red.pilot.FallSurvive();
          else
            plane_blue.pilot.FallSurvive();

          continue;
        }
        default:
        {
          if ( eventCounterOpp < 63 )
            eventCounterOpp++;
          else
            eventCounterOpp = 0;

          break;
        }
      }
    }
    if ( eventCounterOpp < 63 )
      eventCounterOpp++;
    else
      eventCounterOpp = 0;
  }
  opponent_data_prev = opponent_data;


  if ( send_coords_time < 0.25 )
    return;


  data.x = opponent_data.x * sizes.screen_width;
  data.y = opponent_data.y * sizes.screen_height;


  if ( srv_or_cli == SRV_CLI::CLIENT )
    plane_blue.setCoords( data );
  else
    plane_red.setCoords( data );
}
