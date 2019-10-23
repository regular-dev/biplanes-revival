#include <fstream>
#include <sstream>

#include "../include/init_vars.h"
#include "../include/Net.h"
#include "../include/controls.h"
#include "../include/render.h"

double deltaTime = 0.0;

bool game_exit = false;
bool game_pause = false;
bool sound_enable = false;


SRV_CLI srv_or_cli = SRV_CLI::SERVER;

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
double StartingTime = 0.0, EndingTime = 0.0, Frequency = 0.0;
#elif PLATFORM == PLATFORM_WINDOWS
LARGE_INTEGER StartingTime, EndingTime, Frequency;
#endif

Packet opponent_data;
Packet local_data;
bool opponent_connected = false;
bool connection_changed = false;

const unsigned short DEFAULT_HOST_PORT = 55555;
const std::string DEFAULT_SERVER_IP = "127.0.0.1";
const unsigned short DEFAULT_CLIENT_PORT = 55555;
const std::string DEFAULT_MMAKE_PASSWORD = "";

const bool DEFAULT_PLANE_COLLISIONS = false;
const bool DEFAULT_CLOUDS_OPAQUE = true;

unsigned short HOST_PORT = DEFAULT_HOST_PORT;
std::string SERVER_IP = DEFAULT_SERVER_IP;
unsigned short CLIENT_PORT = DEFAULT_CLIENT_PORT;
std::string MMAKE_PASSWORD = DEFAULT_MMAKE_PASSWORD;

bool PLANE_COLLISIONS = DEFAULT_PLANE_COLLISIONS;
bool CLOUDS_OPAQUE = DEFAULT_CLOUDS_OPAQUE;


const int ProtocolId = 0x11223344;
const float TimeOut = 10.0f;

double TICK_RATE = 64.0;
double TICK_TIME = 1.0 / TICK_RATE;
double ticktime = 0.0;
net::ReliableConnection *connection = nullptr;
net::FlowControl *flowControl = nullptr;
const int PacketSize = sizeof( Packet ) + 12;

double send_coords_time = 0.0;


SDL_Event event;
const SDL_Keycode DEFAULT_THROTTLE_UP = SDLK_UP;
const SDL_Keycode DEFAULT_THROTTLE_DOWN = SDLK_DOWN;
const SDL_Keycode DEFAULT_TURN_LEFT = SDLK_LEFT;
const SDL_Keycode DEFAULT_TURN_RIGHT = SDLK_RIGHT;
const SDL_Keycode DEFAULT_FIRE = SDLK_SPACE;
const SDL_Keycode DEFAULT_JUMP = SDLK_LCTRL;

SDL_Keycode THROTTLE_UP = SDLK_UP;
SDL_Keycode THROTTLE_DOWN = SDLK_DOWN;
SDL_Keycode TURN_LEFT = SDLK_LEFT;
SDL_Keycode TURN_RIGHT = SDLK_RIGHT;
SDL_Keycode FIRE = SDLK_SPACE;
SDL_Keycode JUMP = SDLK_LCTRL;

Sizes sizes;
Textures textures;
Sounds sounds;

Menu menu;
Plane plane_blue( PLANE_TYPE::BLUE );
Plane plane_red( PLANE_TYPE::RED );
Controls controls_local, controls_opponent;
BulletSpawner bullets;
std::vector <Cloud> clouds;
Zeppelin zeppelin;

void DrawGame();
void prepare_local_data();
void transform_opponent_data();
void prepare_plane_coords();
void events_reset();

int main( int argc, char *args[] )
{
  std::ofstream log( "log.log", std::ios::trunc );
  log.close();

  // Read user settings from file
  std::ifstream settings_read( "settings.ini" );
  if ( settings_read.is_open() )
  {
    settings_read >>
      THROTTLE_UP >>
      THROTTLE_DOWN >>
      TURN_LEFT >>
      TURN_RIGHT >>
      FIRE >>
      JUMP >>
      HOST_PORT >>
      SERVER_IP >>
      CLIENT_PORT >>
      PLANE_COLLISIONS >>
      CLOUDS_OPAQUE >>
      MMAKE_PASSWORD;
    settings_read.close();
    TICK_TIME = 1.0 / TICK_RATE;
  }
  else
  {
    log_message( "LOG: Can't find user settings file! Creating a new one with default settings...\n\n" );
    settings_write();
  }

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

  while ( !game_exit )
  {
    countDelta();

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

  SDL_close();
  return 0;
}

void game_reset()
{
  init_vars();

  local_data.disconnect = false;
  opponent_data.disconnect = false;

  plane_blue.Respawn();
  plane_red.Respawn();
  for ( Cloud &cloud : clouds )
    cloud.Respawn();
  zeppelin.Respawn();
}

bool game_init()
{
  SDL_SetWindowResizable( gWindow, SDL_FALSE );

  net::Address address;
  int port = srv_or_cli == SRV_CLI::CLIENT ? CLIENT_PORT : HOST_PORT;

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
  flowControl = new net::FlowControl();
  game_reset();

  log_message( "\nLOG: Game initialized successfully!\n\n" );
  return 0;
}

void game_loop()
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

        plane_blue.Respawn();
        plane_red.Respawn();

        plane_blue.ResetScore();
        plane_red.ResetScore();

        for ( Cloud &cloud : clouds )
          cloud.Respawn();

        bullets.Clear();
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
    if( srv_or_cli == SRV_CLI::CLIENT )
    {
      process_local_controls( plane_red, controls_local );
      process_local_controls( plane_blue, controls_opponent );
    }
    else
    {
      process_local_controls( plane_blue, controls_local );
      process_local_controls( plane_red, controls_opponent );
    }
    plane_blue.Update();
    plane_red.Update();
  }
  else
  {
    if( srv_or_cli == SRV_CLI::CLIENT )
    {
      process_local_controls( plane_red, controls_local );
      plane_red.Update();
    }
    else
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
    events_reset();
  }
  ticktime += deltaTime;

  if ( send_coords_time >= 0.25 )
    send_coords_time = 0.0;

  send_coords_time += deltaTime;

  // show packets that were acked this frame
#ifdef SHOW_ACKS
  unsigned int *acks = NULL;
  int ack_count = 0;
  connection->GetReliabilitySystem().GetAcks( &acks, ack_count );
  if ( ack_count > 0 )
  {
    printf( "acks: %d", acks[0] );
    for ( int i = 1; i < ack_count; ++i )
      printf( ",%d", acks[i] );
    printf( "\n" );
  }
#endif


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
  local_data.fire     = controls_local.fire;
  local_data.jump     = controls_local.jump;

  prepare_plane_coords();
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

// Unpack opponent coordinates
void transform_opponent_data()
{
  Plane_Data data;
  data.dir        = opponent_data.dir;
  data.pilot_x    = opponent_data.pilot_x * sizes.screen_width;
  data.pilot_y    = opponent_data.pilot_y * sizes.screen_height;
  if ( srv_or_cli == SRV_CLI::CLIENT )
  {
    plane_blue.setDir( data.dir );
    plane_blue.pilot->setX( data.pilot_x );
    plane_blue.pilot->setY( data.pilot_y );
  }
  else
  {
    plane_red.setDir( data.dir );
    plane_red.pilot->setX( data.pilot_x );
    plane_red.pilot->setY( data.pilot_y );
  }

  if ( opponent_data.hit > HIT_STATE::HIT_NONE )
  {
    switch( opponent_data.hit )
    {
      case HIT_STATE::HIT_PLANE:
      {
        if ( srv_or_cli == SRV_CLI::SERVER )
          plane_red.Hit( (int) srv_or_cli );
        else
          plane_blue.Hit( (int) srv_or_cli );

        break;
      }
      case HIT_STATE::HIT_CHUTE:
      {
        if ( srv_or_cli == SRV_CLI::SERVER )
          plane_red.pilot->ChuteHit();
        else
          plane_blue.pilot->ChuteHit();

        break;
      }
      case HIT_STATE::HIT_PILOT:
      {
        if ( srv_or_cli == SRV_CLI::SERVER )
          plane_red.pilot->Kill( (int) srv_or_cli );
        else
          plane_blue.pilot->Kill( (int) srv_or_cli );

        break;
      }
    }
    opponent_data.hit = HIT_STATE::HIT_NONE;
  }

  if ( opponent_data.death > DEATH_STATE::DEATH_NONE )
  {
    switch( opponent_data.death )
    {
      case DEATH_STATE::PLANE_DEATH:
      {
        if ( srv_or_cli == SRV_CLI::SERVER )
          plane_red.Crash();
        else
          plane_blue.Crash();

        break;
      }
      case DEATH_STATE::PILOT_DEATH:
      {
        if ( srv_or_cli == SRV_CLI::SERVER )
        {
          plane_red.pilot->Death();
          plane_red.ScoreChange( -1 );
        }
        else
        {
          plane_blue.pilot->Death();
          plane_blue.ScoreChange( -1 );
        }
        break;
      }
      case DEATH_STATE::PILOT_RESP:
      {
        if ( srv_or_cli == SRV_CLI::SERVER )
          plane_red.pilot->Rescue();
        else
          plane_blue.pilot->Rescue();

        break;
      }
    }
    opponent_data.death = DEATH_STATE::DEATH_NONE;
  }

  if ( send_coords_time < 0.25 )
    return;


  data.x          = opponent_data.x * sizes.screen_width;
  data.y          = opponent_data.y * sizes.screen_height;


  if ( srv_or_cli == SRV_CLI::CLIENT )
    plane_blue.setCoords( data );
  else
    plane_red.setCoords( data );
}


void events_reset()
{
  if ( local_data.hit > HIT_STATE::HIT_NONE )
    local_data.hit = HIT_STATE::HIT_NONE;

  if ( local_data.death > DEATH_STATE::DEATH_NONE )
    local_data.death = DEATH_STATE::DEATH_NONE;
}
