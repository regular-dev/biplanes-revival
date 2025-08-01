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

#pragma once

#include <cstdint>


enum class SRV_CLI : uint8_t
{
  SERVER,
  CLIENT,
};

enum MENU_BUTTON_DIR : int8_t
{
  LEFT = -1,
  RIGHT = 1,
};

enum GAME_MODE : uint8_t
{
  HUMAN_VS_HUMAN,
  HUMAN_VS_HUMAN_HOTSEAT,
  HUMAN_VS_BOT,
  BOT_VS_BOT,
};

enum PLANE_TYPE : uint8_t
{
  BLUE,
  RED,
};

enum PLANE_THROTTLE : uint8_t
{
  THROTTLE_IDLE,
  THROTTLE_DECREASE,
  THROTTLE_INCREASE,
};

enum PLANE_PITCH : uint8_t
{
  PITCH_IDLE,
  PITCH_LEFT,
  PITCH_RIGHT,
};

enum CHUTE_STATE : uint8_t
{
  CHUTE_IDLE,
  CHUTE_LEFT,
  CHUTE_RIGHT,
  CHUTE_DESTROYED,
  CHUTE_NONE,
};

enum class EVENTS : unsigned char
{
  NONE = 'n',

  NO_EXTRA_CLOUDS   = 'O',
  NO_ONESHOT_KILLS  = 'K',
  NO_ALT_HITBOXES   = 'H',

  SHOOT         = 's',
  EJECT         = 'e',

  HIT_PLANE     = 'P',
  HIT_CHUTE     = 'c',
  HIT_PILOT     = 'p',
  PLANE_DEATH   = 'D',
  PILOT_DEATH   = 'd',
  PLANE_RESPAWN = 'R',
  PILOT_RESPAWN = 'r',
  PILOT_LAND    = 'l',
};


enum class AiAction : uint8_t
{
  Idle,
  Accelerate,
  Decelerate,
  TurnLeft,
  TurnRight,
  Shoot,
  Jump,

  ActionCount,
};

namespace MENU_SPECIFY
{
  enum MENU_SPECIFY
  {
    IP,
    PORT,
    PASSWORD,
    WIN_SCORE,
    AUDIO_VOLUME,
    STEREO_DEPTH,
  };
}

namespace DIFFICULTY
{
  enum DIFFICULTY : uint8_t
  {
    EASY,
    MEDIUM,
    HARD,
    DEVELOPER,
    INSANE
  };
}

enum class MESSAGE_TYPE
{
  NONE,

  SOCKET_INIT_FAILED,
  CANT_START_CONNECTION,

  CLIENT_CONNECTING,
  CONNECTION_FAILED,
  CONNECTION_TIMED_OUT,
  CONNECTION_UNSTABLE,
  HOST_CEASED_CONNECTION,

  HOST_LISTENING,
  CLIENT_DISCONNECTED,

  SUCCESSFULL_CONNECTION,
  GAME_WON,
  GAME_LOST,

  BLUE_SIDE_WON,
  RED_SIDE_WON,
  EVERY_SIDE_WON,
  ROUND_DRAW,

  MMAKE_CONNECTING_TO_SERVER,
  MMAKE_BAD_SERVER_REPLY,
  MMAKE_SEARCHING_OPPONENT,

  P2P_ESTABLISHING,
  P2P_WAIT_ANSWER,
  MMAKE_PTP_TIMEOUT,
};

enum class ROOMS
{
  MENU_COPYRIGHT,
  MENU_SPLASH,
  MENU_MAIN,
  MENU_SP,
  MENU_SP_SETUP,
  MENU_MP,
  MENU_MP_HELP_PAGE1,
  MENU_MP_HELP_PAGE2,
  MENU_MP_HELP_PAGE3,
  MENU_MP_HELP_PAGE4,
  MENU_MP_HELP_PAGE5,
  MENU_MP_HELP_PAGE6,
  MENU_MP_HELP_PAGE7,
  MENU_MP_HELP_PAGE8,
  MENU_MP_MMAKE,
  MENU_MP_MMAKE_FIND_GAME,
  MENU_MP_DC,
  MENU_MP_DC_HOST,
  MENU_MP_DC_JOIN,
  MENU_MP_DC_HELP,
  MENU_MP_HOTSEAT,
  MENU_SETTINGS,
  MENU_SETTINGS_CONTROLS_PLAYER1,
  MENU_SETTINGS_CONTROLS_PLAYER2,
  MENU_HELP,
  MENU_RECENT_STATS_PAGE1,
  MENU_RECENT_STATS_PAGE2,
  MENU_TOTAL_STATS_PAGE1,
  MENU_TOTAL_STATS_PAGE2,
  MENU_PAUSE,
  GAME,
};

namespace MENU_MAIN
{
  enum MENU_MAIN
  {
    SINGLEPLAYER,
    MULTIPLAYER,
    SETTINGS,
    HELP,
    EXIT,
  };
}

namespace MENU_SP
{
  enum MENU_SP
  {
    SETUP_GAME,
    AI_MODE,
    AI_DIFFICULTY,
    BACK,
  };
}

namespace MENU_SP_SETUP
{
  enum MENU_SP_SETUP
  {
    START,
    WIN_SCORE,
    EXTRA_CLOUDS,
    ONESHOT_KILLS,
    ALT_HITBOXES,
    BACK,
  };
}

namespace MENU_MP
{
  enum MENU_MP
  {
    MMAKE,
    DC,
    HOTSEAT,
    HELP,
    BACK,
  };
}

namespace MENU_MP_MMAKE
{
  enum MENU_MP_MMAKE
  {
    FIND_GAME,
    SPECIFY_PASSWORD,
    EXTRA_CLOUDS,
    ONESHOT_KILLS,
    ALT_HITBOXES,
    BACK,
  };
}

namespace MENU_MP_MMAKE_FIND_GAME
{
  enum MENU_MP_MMAKE_FIND_GAME
  {
    BACK,
  };
}

namespace MENU_MP_DC
{
  enum MENU_MP_DC
  {
    HOST,
    JOIN,
    HELP,
    BACK,
  };
}

namespace MENU_MP_DC_HOST
{
  enum MENU_MP_DC_HOST
  {
    HOST_START,
    SPECIFY_PORT,
    EXTRA_CLOUDS,
    ONESHOT_KILLS,
    ALT_HITBOXES,
    BACK,
  };
}

namespace MENU_MP_DC_JOIN
{
  enum MENU_MP_DC_JOIN
  {
    JOIN,
    SPECIFY_IP,
    SPECIFY_PORT,
    BACK,
  };
}

using MENU_MP_HOTSEAT = MENU_SP_SETUP::MENU_SP_SETUP;

namespace MENU_SETTINGS
{
  enum MENU_SETTINGS
  {
    CONTROLS,
    AUDIO_VOLUME,
    STEREO_DEPTH,
    STATS_RESET,
    BACK,
  };
}

namespace MENU_SETTINGS_CONTROLS
{
  enum MENU_SETTINGS_CONTROLS
  {
    ACCELERATE,
    DECELERATE,
    LEFT,
    RIGHT,
    SHOOT,
    EJECT,
    TOGGLE_PLAYER_PAGE,
    BACK,
  };
}

namespace MENU_PAUSE
{
  enum MENU_PAUSE
  {
    CONTINUE,
    SETTINGS,
    HELP,
    DISCONNECT,
  };
}
