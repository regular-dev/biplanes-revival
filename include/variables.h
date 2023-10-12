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

// platform detection

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
  #define PLATFORM PLATFORM_WINDOWS

#elif defined(__APPLE__)
  #define PLATFORM PLATFORM_MAC

#else
  #define PLATFORM PLATFORM_UNIX

#endif

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
  #include <arpa/inet.h>
  #include <netdb.h>
#else
    #error unknown platform!

#endif


#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
  extern double StartingTime, EndingTime, Frequency;

#elif PLATFORM == PLATFORM_WINDOWS
  extern LARGE_INTEGER StartingTime, EndingTime, Frequency;

#endif

extern double deltaTime;


#define PI 3.1416f

extern bool consoleOutput;
extern bool logFileOutput;
extern bool statsOutput;
extern bool show_hitboxes;


enum class SRV_CLI
{
  SERVER,
  CLIENT
};

extern SRV_CLI srv_or_cli;
extern bool game_exit;
extern bool game_pause;
extern bool sound_initialized;
extern bool game_finished;


#include <include/server.h>
extern Packet opponent_data;
extern Packet local_data;
extern bool opponent_connected;


#include <include/sdl.h>
extern SDL_Event event;
extern const SDL_Keycode DEFAULT_THROTTLE_UP;
extern const SDL_Keycode DEFAULT_THROTTLE_DOWN;
extern const SDL_Keycode DEFAULT_TURN_LEFT;
extern const SDL_Keycode DEFAULT_TURN_RIGHT;
extern const SDL_Keycode DEFAULT_FIRE;
extern const SDL_Keycode DEFAULT_JUMP;

extern SDL_Keycode THROTTLE_UP;
extern SDL_Keycode THROTTLE_DOWN;
extern SDL_Keycode TURN_LEFT;
extern SDL_Keycode TURN_RIGHT;
extern SDL_Keycode FIRE;
extern SDL_Keycode JUMP;

extern const unsigned short DEFAULT_HOST_PORT;
extern const std::string DEFAULT_SERVER_IP;
extern const unsigned short DEFAULT_SERVER_PORT;
extern const std::string DEFAULT_MMAKE_PASSWORD;
extern const std::string MMAKE_PASSWORD_PREFIX;

extern const bool DEFAULT_HARDCORE_MODE;

extern unsigned short HOST_PORT;
extern std::string SERVER_IP;
extern unsigned short SERVER_PORT;
extern std::string MMAKE_PASSWORD;

extern bool HARDCORE_MODE;

extern double TICK_RATE;
extern double TICK_TIME;
extern double ticktime;


extern const int PacketSize;

namespace net
{
  class ReliableConnection;
}

extern net::ReliableConnection *connection;

extern const int ProtocolId;
extern const float TimeOut;

#include <include/structures.h>
extern Sizes sizes;
extern Textures textures;
extern Sounds sounds;
extern Controls controls_local, controls_opponent;
extern Statistics stats_recent, stats_total;


#include <include/plane.h>
#include <include/bullet.h>
#include <include/cloud.h>
#include <include/zeppelin.h>
#include <include/menu.h>

extern class Plane plane_blue;
extern class Plane plane_red;
extern class BulletSpawner bullets;
extern std::vector <Cloud> clouds;
extern class Zeppelin zeppelin;
extern class Menu menu;

