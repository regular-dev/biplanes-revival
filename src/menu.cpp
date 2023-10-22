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

#include <include/menu.h>
#include <include/sdl.h>
#include <include/time.hpp>
#include <include/timer.hpp>
#include <include/game_state.hpp>
#include <include/network_state.hpp>
#include <include/render.h>
#include <include/biplanes.hpp>
#include <include/matchmake.hpp>
#include <include/init_vars.h>
#include <include/controls.h>
#include <include/plane.h>
#include <include/sizes.hpp>
#include <include/textures.hpp>
#include <include/variables.h>
#include <include/utility.h>


Menu::Menu()
{
  mButtons =
  {
    {ROOMS::MENU_COPYRIGHT, 0},
    {ROOMS::MENU_SPLASH, 0},
    {ROOMS::MENU_MAIN, MENU_MAIN::EXIT},
    {ROOMS::MENU_SETTINGS_CONTROLS, MENU_SETTINGS_CONTROLS::BACK},
    {ROOMS::MENU_HELP, 0},

    {ROOMS::MENU_SP, MENU_SP::BACK},
    {ROOMS::MENU_SP_SETUP, MENU_SP_SETUP::BACK},

    {ROOMS::MENU_MP, MENU_MP::BACK},
    {ROOMS::MENU_MP_HELP_PAGE1, 0},
    {ROOMS::MENU_MP_HELP_PAGE2, 0},
    {ROOMS::MENU_MP_HELP_PAGE3, 0},
    {ROOMS::MENU_MP_HELP_PAGE4, 0},
    {ROOMS::MENU_MP_HELP_PAGE5, 0},
    {ROOMS::MENU_MP_HELP_PAGE6, 0},
    {ROOMS::MENU_MP_HELP_PAGE7, 0},
    {ROOMS::MENU_MP_HELP_PAGE8, 0},

    {ROOMS::MENU_MP_MMAKE, MENU_MP_MMAKE::BACK},
    {ROOMS::MENU_MP_MMAKE_FIND_GAME, MENU_MP_MMAKE::BACK},
//    {ROOMS::MENU_MP_MMAKE_FIND_GAME, MENU_MP_MMAKE_FIND_GAME::BACK},

    {ROOMS::MENU_MP_DC, MENU_MP_DC::BACK},
    {ROOMS::MENU_MP_DC_HOST, MENU_MP_DC_HOST::BACK},
    {ROOMS::MENU_MP_DC_JOIN, MENU_MP_DC_JOIN::BACK},
    {ROOMS::MENU_MP_DC_HELP, 0},

    {ROOMS::MENU_PAUSE, MENU_PAUSE::DISCONNECT},
  };

  mInputIp = SERVER_IP;
  mInputPortHost = std::to_string(LOCAL_PORT);
  mInputPortClient = std::to_string(REMOTE_PORT);
  mInputPassword = MMAKE_PASSWORD;
  mInputScoreToWin = std::to_string(gameState().winScore);

  mConnectedMessageTimer = {3.0f};
}

void
Menu::ResizeWindow()
{
  if ( windowEvent.type != SDL_WINDOWEVENT )
    return;

  if (  windowEvent.window.event != SDL_WINDOWEVENT_RESIZED &&
        windowEvent.window.event != SDL_WINDOWEVENT_SIZE_CHANGED &&
        windowEvent.window.event != SDL_WINDOWEVENT_MOVED )
    return;


  if ( mCurrentRoom != ROOMS::GAME && gameState().isPaused == false )
    SDL_GetRendererOutputSize(
      gRenderer,
      &sizes.screen_width_new,
      &sizes.screen_height_new );

  if (  sizes.screen_height_new != sizes.screen_height ||
        sizes.screen_width_new != sizes.screen_width ||
        SDL_GetWindowDisplayIndex(gWindow) != DISPLAY_INDEX )
    window_resize();
}

void
Menu::setMessage(
  const MESSAGE_TYPE message_type )
{
  mCurrentMessage = message_type;

  if ( mCurrentMessage == MESSAGE_TYPE::SUCCESSFULL_CONNECTION )
    mConnectedMessageTimer.Start();
}

void
Menu::AnimateButton()
{
  if ( sizes.button_dir == MENU_BUTTON_DIR::RIGHT )
  {
    if ( sizes.button_x < 127 - deltaTime * sizes.screen_width * 0.075 )
      sizes.button_x += deltaTime * sizes.screen_width * 0.075;
    else
      sizes.button_dir = MENU_BUTTON_DIR::LEFT;

    return;
  }

  if ( sizes.button_x > deltaTime * sizes.screen_width * 0.075 )
    sizes.button_x -= deltaTime * sizes.screen_width * 0.075;
  else
    sizes.button_dir = MENU_BUTTON_DIR::RIGHT;
}

bool
Menu::isSpecifyingVar(
  const MENU_SPECIFY var_type ) const
{
  return mSpecifyingVarState[var_type];
}

bool
Menu::isDefiningKey() const
{
  return mIsDefiningKey;
}


void
Menu::DrawMenu()
{
  const auto& game = gameState();


  if ( game.isPaused == true )
  {
    if ( game.gameMode == GAME_MODE::HUMAN_VS_HUMAN )
      game_loop_mp();

    else
    {
      deltaTime = 0.0;
      game_loop_sp();
    }
  }
  else
    mConnectedMessageTimer.Update();

  switch (menu.mCurrentRoom)
  {
    case ROOMS::MENU_COPYRIGHT:
    {
      screen_copyright();
      break;
    }

    case ROOMS::MENU_SPLASH:
    {
      screen_splash();
      break;
    }

    case ROOMS::MENU_MAIN:
    {
      screen_main();
      break;
    }

    case ROOMS::MENU_SP:
    {
      screen_sp();
      break;
    }

    case ROOMS::MENU_SP_SETUP:
    {
      screen_sp_setup();
      break;
    }

    case ROOMS::MENU_MP:
    {
      screen_mp();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE1:
    {
      screen_mp_help_page1();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE2:
    {
      screen_mp_help_page2();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE3:
    {
      screen_mp_help_page3();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE4:
    {
      screen_mp_help_page4();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE5:
    {
      screen_mp_help_page5();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE6:
    {
      screen_mp_help_page6();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE7:
    {
      screen_mp_help_page7();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE8:
    {
      screen_mp_help_page8();
      break;
    }

    case ROOMS::MENU_MP_MMAKE:
    {
      screen_mp_mmake();
      break;
    }

    case ROOMS::MENU_MP_MMAKE_FIND_GAME:
    {
      screen_mp_mmake();

      auto& network = networkState();

      const auto mmakeState = network.matchmaker->state();

      if ( mmakeState == MatchMakerState::MATCH_READY )
      {
        auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
        auto& planeRed = planes.at(PLANE_TYPE::RED);

        planeBlue.setBot(false);
        planeRed.setBot(false);

        if ( network.matchmaker->clientNodeType() == SRV_CLI::CLIENT )
        {
          const auto remoteAddress = network.matchmaker->opponentAddress();

          network.nodeType = SRV_CLI::CLIENT;

          planeBlue.setLocal(false);
          planeRed.setLocal(true);

          SERVER_IP =
              remoteAddress.GetA() + "." +
              remoteAddress.GetB() + "." +
              remoteAddress.GetC() + "." +
              remoteAddress.GetD();

          REMOTE_PORT = remoteAddress.GetPort();
        }
        else
        {
          network.nodeType = SRV_CLI::SERVER;

          planeBlue.setLocal(true);
          planeRed.setLocal(false);
        }


        if ( game_init_mp() != 0 )
        {
          log_message( "\nLOG: Failed to initialize game!\n\n" );
          network.connection->Stop();

          ReturnToMainMenu();
        }
        else
        {
          network.matchmaker->Reset();
          ChangeRoom(ROOMS::GAME);
        }

        break;
      }

      if ( mmakeState == MatchMakerState::MATCH_TIMEOUT )
      {
        networkState().matchmaker->Reset();
        ChangeRoom(ROOMS::MENU_MP_MMAKE);
        log_message( "NETWORK: Peer-to-Peer connection timeout!\n" );
        setMessage(MESSAGE_TYPE::MMAKE_PTP_TIMEOUT);

        break;
      }

      break;
    }

    case ROOMS::MENU_MP_DC:
    {
      screen_mp_dc();
      break;
    }

    case ROOMS::MENU_MP_DC_HOST:
    {
      screen_mp_dc_host();
      break;
    }

    case ROOMS::MENU_MP_DC_JOIN:
    {
      screen_mp_dc_join();
      break;
    }

    case ROOMS::MENU_MP_DC_HELP:
    {
      screen_mp_dc_help();
      break;
    }

    case ROOMS::MENU_SETTINGS_CONTROLS:
    {
      screen_settings();
      break;
    }

    case ROOMS::MENU_HELP:
    {
      screen_help();
      break;
    }

    case ROOMS::MENU_RECENT_STATS:
    {
      screen_stats_recent();
      break;
    }

    case ROOMS::MENU_TOTAL_STATS_PAGE1:
    {
      screen_stats_total_page1();
      break;
    }

    case ROOMS::MENU_TOTAL_STATS_PAGE2:
    {
      screen_stats_total_page2();
      break;
    }

    case ROOMS::MENU_PAUSE:
    {
      screen_pause();
      break;
    }

    case ROOMS::GAME:
    {
      if ( game.gameMode == GAME_MODE::HUMAN_VS_HUMAN )
        game_loop_mp();
      else
        game_loop_sp();

      break;
    }

    default:
      break;
  }


  switch (mCurrentMessage)
  {
    case MESSAGE_TYPE::SOCKET_INIT_FAILED:
    {
      draw_text( "Failed to init sockets!", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::CANT_START_CONNECTION:
    {
      draw_text( "Failed to start connection!", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::CLIENT_CONNECTING:
    {
      char textbuf[40];

      sprintf( textbuf, "Connecting to %s:%d",
        SERVER_IP.c_str(), REMOTE_PORT );

      draw_text( textbuf, 0, 0 );
      break;
    }

    case MESSAGE_TYPE::CONNECTION_FAILED:
    {
      draw_text( "Connection failed!", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::CONNECTION_TIMED_OUT:
    {
      draw_text( "Connection timed out!", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::HOST_CEASED_CONNECTION:
    {
      draw_text( "Server ceased connection!", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::HOST_LISTENING:
    {
      draw_text( "Waiting for incoming connections", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::CLIENT_DISCONNECTED:
    {
      draw_text( "Client disconnected!", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::SUCCESSFULL_CONNECTION:
    {
      if ( mConnectedMessageTimer.isReady() == true )
      {
        setMessage(MESSAGE_TYPE::NONE);
        break;
      }

      if ( game.isPaused == true )
        break;


      const auto& planeRed = planes.at(PLANE_TYPE::RED);
      const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);

      if ( game.gameMode != GAME_MODE::BOT_VS_BOT )
      {
        const auto& playerPlane =
          planeRed.isBot() == false && planeRed.isLocal() == true
          ? planeRed : planeBlue;

//        TODO: move away magic number
        const float textSizeFactor =
          playerPlane.type() == PLANE_TYPE::BLUE
          ? 1.0f : 7.0f;

        draw_text( "It's You!",
          playerPlane.x() - sizes.text_sizex * textSizeFactor,
          playerPlane.y() - sizes.screen_height * 0.1 );
      }

      if ( game.gameMode != GAME_MODE::HUMAN_VS_HUMAN )
        break;

      if ( networkState().nodeType == SRV_CLI::CLIENT )
        draw_text( "Successfully connected to server", 0, 0 );
      else
        draw_text( "New client connected!", 0, 0 );

      break;
    }

    case MESSAGE_TYPE::GAME_WON:
    {
      draw_text( "            You won!            ", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::GAME_LOST:
    {
      draw_text( "           You  lost!           ", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::BLUE_SIDE_WON:
    {
      draw_text( "         Blue side won!         ", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::RED_SIDE_WON:
    {
      draw_text( "         Red  side won!         ", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::MMAKE_CONNECTING_TO_SERVER:
    {
      draw_text( "Connecting to matchmake server...", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::MMAKE_BAD_SERVER_REPLY:
    {
      draw_text( "Bad matchmake server reply!", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::MMAKE_SEARCHING_OPPONENT:
    {
      draw_text( "Searching for opponent...", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::P2P_ESTABLISHING:
    {
      draw_text( "Establishing p2p connection...", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::P2P_WAIT_ANSWER:
    {
      draw_text( "Waiting for opponent answer...", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::MMAKE_PTP_TIMEOUT:
    {
      draw_text( "Peer-to-peer connection timeout!", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::NONE:
      break;

    default:
      break;
  }

  AnimateButton();
}

void
Menu::DrawButton()
{
  const SDL_Rect srcRect
  {
    sizes.button_x, 0,
    127, 12,
  };

  const SDL_Rect buttonRect
  {
    sizes.screen_width * 0.008,
    sizes.screen_height * 0.34755 + mSelectedButton * sizes.button_sizey,
    sizes.button_sizex,
    sizes.button_sizey,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_moving_button,
    &srcRect,
    &buttonRect );
}


void
Menu::screen_main()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear(gRenderer);

  draw_background();
  draw_barn();


  const SDL_Rect menuRect
  {
    0,
    sizes.screen_height * 0.3,
    sizes.screen_width,
    sizes.screen_height * 0.288
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_box,
    nullptr,
    &menuRect );


  DrawButton();


  draw_text( "BIPLANES REVIVAL", sizes.screen_width * 0.250, sizes.screen_height * 0.2855 );
  draw_text( "One Player Game ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Two Player Game ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Controls        ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Help            ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
  draw_text( "Quit            ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 4.0 );

  draw_text( "Navigate menu using arrow keys  ", sizes.screen_width * 0.005, sizes.screen_height * 0.65 );
  draw_text( " Press[RETURN] to enter submenu ", sizes.screen_width * 0.005, sizes.screen_height * 0.70 );
  draw_text( "  Press [F1] to see your stats  ", sizes.screen_width * 0.005, sizes.screen_height * 0.75 );
}

void
Menu::screen_settings()
{
  if ( gameState().isPaused == false )
  {
    SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
    SDL_RenderClear(gRenderer);

    draw_background();
    draw_barn();
  }


  const SDL_Rect settingsRect
  {
    0,
    sizes.screen_height * 0.3,
    sizes.screen_width,
    sizes.screen_height * 0.288 + sizes.screen_height * 0.05775 * 3.0
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_settings_controls_box,
    nullptr,
    &settingsRect);


  DrawButton();


  draw_text( "Controls            ",          sizes.screen_width * 0.250, sizes.screen_height * 0.2855 );
  draw_text( "Accelerate          ",          sizes.screen_width * 0.025, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( SDL_GetKeyName( THROTTLE_UP ),   sizes.screen_width * 0.700, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Decelerate          ",          sizes.screen_width * 0.025, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( SDL_GetKeyName( THROTTLE_DOWN ), sizes.screen_width * 0.700, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Turn Anti-Clockwise ",          sizes.screen_width * 0.025, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( SDL_GetKeyName( TURN_LEFT ),     sizes.screen_width * 0.700, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Turn Clockwise      ",          sizes.screen_width * 0.025, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
  draw_text( SDL_GetKeyName( TURN_RIGHT ),    sizes.screen_width * 0.700, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
  draw_text( "Fire                ",          sizes.screen_width * 0.025, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 4.0 );
  draw_text( SDL_GetKeyName( FIRE ),          sizes.screen_width * 0.700, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 4.0 );
  draw_text( "Eject               ",          sizes.screen_width * 0.025, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 5.0 );
  draw_text( SDL_GetKeyName( JUMP ),          sizes.screen_width * 0.700, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 5.0 );
  draw_text( "Back                ",          sizes.screen_width * 0.025, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 6.0 );

  if ( menu.isDefiningKey() == true )
  {
    draw_text( "     Press the key you wish    ", sizes.screen_width * 0.025, sizes.screen_height * 0.05 );
    draw_text( "  to assign to this function.  ", sizes.screen_width * 0.025, sizes.screen_height * 0.05 * 2.0 );
    draw_text( "     Press [ESC] to cancel.    ", sizes.screen_width * 0.025, sizes.screen_height * 0.05 * 3.5 );
  }
  else
  {
    draw_text( "Press[RETURN] to remap selected", sizes.screen_width * 0.025, sizes.screen_height * 0.025 );
    draw_text( "          key binding.         ", sizes.screen_width * 0.025, sizes.screen_height * 0.075 );
    draw_text( "  Press [DELETE]  to reset it  ", sizes.screen_width * 0.025, sizes.screen_height * 0.05 * 3.0 );
    draw_text( "       to default value.       ", sizes.screen_width * 0.025, sizes.screen_height * 0.05 * 4.0 );
  }
}

void
Menu::screen_pause()
{
  const SDL_Rect menuRect
  {
    0,
    sizes.screen_height * 0.3,
    sizes.screen_width,
    sizes.screen_height * 0.288
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_box,
    nullptr,
    &menuRect );


  DrawButton();


  draw_text( "GAME PAUSED       ", sizes.screen_width * 0.250, sizes.screen_height * 0.2855 );
  draw_text( "On With the Show! ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Controls          ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Help              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Bail Out!         ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
}

void
Menu::screen_copyright()
{
  SDL_SetRenderDrawColor(gRenderer, 0, 154, 239, 255);
  SDL_RenderClear(gRenderer);

  draw_text( "      'Bluetooth Biplanes'      ", 0, sizes.screen_height * 0.350 );
  draw_text( "      @ Morpheme Ltd. 2004      ", 0, sizes.screen_height * 0.400 );
  draw_text( "       All Rights Reserved      ", 0, sizes.screen_height * 0.450 );
  draw_text( "       www.morpheme.co.uk       ", 0, sizes.screen_height * 0.500 );

  draw_text( "  Brought to PC on 01.04.20 by  ", 0, sizes.screen_height * 0.650 );
  draw_text( "       casqade & xion  at       ", 0, sizes.screen_height * 0.700 );
  draw_text( "     github.com/regular-dev     ", 0, sizes.screen_height * 0.750 );
  draw_text( "         regular-dev.org        ", 0, sizes.screen_height * 0.800 );

  draw_text( "  Press [SPACE],[ESC]or[RETURN] ", 0, sizes.screen_height * 0.900 );
}

void
Menu::screen_splash()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear(gRenderer);


  const SDL_Rect logoRect
  {
    sizes.screen_width * 0.5f - sizes.screen_height * 0.5f,
    0,
    sizes.screen_height,
    sizes.screen_height
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_logo,
    nullptr,
    &logoRect );

  draw_text( "  Press [SPACE],[ESC]or[RETURN] ", 0, sizes.screen_height * 0.900f );
}

void
window_resize()
{
  if ( SDL_GetWindowDisplayIndex(gWindow) != DISPLAY_INDEX )
  {
    DISPLAY_INDEX = SDL_GetWindowDisplayIndex(gWindow);

    SDL_DisplayMode dm;

    SDL_GetDesktopDisplayMode(
      SDL_GetWindowDisplayIndex(gWindow),
      &dm );

    SCREEN_WIDTH = dm.w;
    SCREEN_HEIGHT = dm.h;

    SDL_SetWindowMinimumSize(
      gWindow,
      SCREEN_HEIGHT * 1.23 * 0.2,
      SCREEN_HEIGHT * 0.2 );

    SDL_SetWindowMaximumSize(
      gWindow,
      SCREEN_HEIGHT * 1.23,
      SCREEN_HEIGHT );
  }

  sizes.screen_height = sizes.screen_height_new;
  sizes.screen_width = sizes.screen_height * 1.23f;
  sizes.screen_width_new = sizes.screen_width;

  SDL_SetWindowSize(
    gWindow,
    sizes.screen_width,
    sizes.screen_height );

  init_sizes();
  SDL_RenderClear(gRenderer);
}
