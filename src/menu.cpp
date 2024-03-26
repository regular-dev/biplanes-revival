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

#include <include/menu.hpp>
#include <include/sdl.hpp>
#include <include/time.hpp>
#include <include/timer.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/network_state.hpp>
#include <include/render.hpp>
#include <include/biplanes.hpp>
#include <include/matchmake.hpp>
#include <include/controls.hpp>
#include <include/plane.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>
#include <include/utility.hpp>


Menu::Menu()
{
  mButtons =
  {
    {ROOMS::MENU_COPYRIGHT, 0},
    {ROOMS::MENU_SPLASH, 0},
    {ROOMS::MENU_MAIN, MENU_MAIN::EXIT},
    {ROOMS::MENU_SETTINGS, MENU_SETTINGS::BACK},
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
  mInputAudioVolume = std::to_string(percentageToInteger(gameState().audioVolume));
  mInputStereoDepth = std::to_string(percentageToInteger(gameState().stereoDepth));

  mConnectedMessageTimer = {constants::menu::connectedMessageTimeout};
  mIntroAutoSkipTimer = {constants::menu::introAutoSkipTimeout};

  mIntroAutoSkipTimer.Start();
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
  namespace button = constants::menu::button;


  mButtonX += mButtonDir * button::speed * deltaTime;

  if ( mButtonX >= 1.0f )
  {
    mButtonX = 2.0f - mButtonX;
    mButtonDir = MENU_BUTTON_DIR::LEFT;
  }
  else if ( mButtonX <= 0.0f )
  {
    mButtonX = -mButtonX;
    mButtonDir = MENU_BUTTON_DIR::RIGHT;
  }
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

ROOMS
Menu::currentRoom() const
{
  return mCurrentRoom;
}

MESSAGE_TYPE
Menu::currentMessage() const
{
  return mCurrentMessage;
}


void
Menu::DrawMenu()
{
  AnimateButton();


  const auto& game = gameState();

  if ( game.isPaused == false )
    mConnectedMessageTimer.Update();


  switch (menu.mCurrentRoom)
  {
    case ROOMS::MENU_COPYRIGHT:
    {
      if ( game.autoSkipIntro == true )
      {
        ChangeRoom(ROOMS::MENU_MAIN);
        break;
      }

      mIntroAutoSkipTimer.Update();

      if ( mIntroAutoSkipTimer.isReady() == true )
      {
        mIntroAutoSkipTimer.Start();
        ChangeRoom(ROOMS::MENU_SPLASH);
        break;
      }

      screen_copyright();
      break;
    }

    case ROOMS::MENU_SPLASH:
    {
      mIntroAutoSkipTimer.Update();

      if ( mIntroAutoSkipTimer.isReady() == true )
      {
        ChangeRoom(ROOMS::MENU_MAIN);
        break;
      }

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
          log_message( "\nLOG: Failed to initialize multiplayer game!\n\n" );
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

    case ROOMS::MENU_SETTINGS:
    {
      screen_settings();
      break;
    }

    case ROOMS::MENU_SETTINGS_CONTROLS:
    {
      screen_controls();
      break;
    }

    case ROOMS::MENU_HELP:
    {
      screen_help();
      break;
    }

    case ROOMS::MENU_RECENT_STATS_PAGE1:
    {
      screen_stats_recent_page1();
      break;
    }

    case ROOMS::MENU_RECENT_STATS_PAGE2:
    {
      screen_stats_recent_page2();
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

    case MESSAGE_TYPE::CONNECTION_UNSTABLE:
    {
      draw_text( "Connection is unstable...", 0, 0 );
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
        const float offsetInLetters =
          playerPlane.type() == PLANE_TYPE::BLUE
          ? 1.0f : 7.0f;

        draw_text( "It's You!",
          playerPlane.x() - offsetInLetters * constants::text::sizeX,
          playerPlane.y() - 0.1f ); // TODO: move to constants
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

    case MESSAGE_TYPE::EVERY_SIDE_WON:
    {
      draw_text( "          Everyone won          ", 0, 0 );
      break;
    }

    case MESSAGE_TYPE::ROUND_DRAW:
    {
      draw_text( "           Nobody won           ", 0, 0 );
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
}

void
Menu::DrawMenuRect()
{
  namespace menu = constants::menu;
  namespace header = menu::header;
  namespace border = menu::border;
  namespace button = menu::button;
  namespace colors = constants::colors;


  const auto buttonCount = mButtons[mCurrentRoom] + 1;

  const auto buttonSectionSizeY =
    buttonCount * button::height / constants::baseHeight;

  const auto menuSizeY =
    header::sizeY + buttonSectionSizeY + border::thicknessY;


  const SDL_FRect boxRect
  {
    toWindowSpaceX(menu::originX),
    toWindowSpaceY(menu::originY),
    scaleToScreenX(menu::sizeX),
    scaleToScreenY(menuSizeY),
  };

  const SDL_FRect headerRect
  {
    toWindowSpaceX(menu::originX),
    toWindowSpaceY(menu::originY),
    scaleToScreenX(header::sizeX),
    scaleToScreenY(header::sizeY),
  };

  setRenderColor(colors::menuBox);
  SDL_RenderFillRectF(
    gRenderer,
    &boxRect );

  setRenderColor(colors::menuHeader);
  SDL_RenderFillRectF(
    gRenderer,
    &headerRect );


  setRenderColor(colors::menuBorder);


//  top
  SDL_FRect borderRect
  {
    toWindowSpaceX(menu::originX),
    toWindowSpaceY(menu::originY),
    scaleToScreenX(menu::sizeX),
    scaleToScreenY(border::thicknessY),
  };

  SDL_RenderFillRectF(
    gRenderer,
    &borderRect );


//  right
  borderRect =
  {
    toWindowSpaceX(menu::originX + menu::sizeX - border::thicknessX),
    toWindowSpaceY(menu::originY),
    scaleToScreenX(border::thicknessX),
    scaleToScreenY(menuSizeY),
  };

  SDL_RenderFillRectF(
    gRenderer,
    &borderRect );


//  bottom
  borderRect =
  {
    toWindowSpaceX(menu::originX),
    toWindowSpaceY(menu::originY + header::sizeY + buttonSectionSizeY),
    scaleToScreenX(menu::sizeX),
    scaleToScreenY(border::thicknessY),
  };

  SDL_RenderFillRectF(
    gRenderer,
    &borderRect );


//  left
  borderRect =
  {
    toWindowSpaceX(menu::originX),
    toWindowSpaceY(menu::originY),
    scaleToScreenX(border::thicknessX),
    scaleToScreenY(menuSizeY),
  };

  SDL_RenderFillRectF(
    gRenderer,
    &borderRect );
}

void
Menu::DrawButton()
{
  namespace button = constants::menu::button;

  const SDL_Rect srcRect
  {
    mButtonX * 0.5f * button::width,
    0,
    0.5f * button::width,
    button::height,
  };

  const SDL_FRect buttonRect
  {
    toWindowSpaceX(button::originX),
    toWindowSpaceY(button::originY + mSelectedItem * button::sizeY),
    scaleToScreenX(button::sizeX),
    scaleToScreenY(button::sizeY),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.menu_moving_button,
    &srcRect,
    &buttonRect );
}


void
Menu::screen_main()
{
  namespace button = constants::menu::button;

  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  draw_background();
  draw_barn();

  DrawMenuRect();
  DrawButton();


  draw_text( "BIPLANES REVIVAL", 0.250f, 0.2855f );
  draw_text( "One Player Game ", 0.255f, 0.2855f + 0.0721f );
  draw_text( "Two Player Game ", 0.255f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Settings        ", 0.255f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "Help            ", 0.255f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( "Quit            ", 0.255f, 0.2855f + 0.0721f + button::sizeY * 4.f );

  draw_text( "Navigate menu using arrows/WASD ", 0.005f, 0.65f );
  draw_text( " Press[RETURN] to enter submenu ", 0.005f, 0.70f );
  draw_text( "  Press [F1] to see your stats  ", 0.005f, 0.75f );
}

void
Menu::screen_settings()
{
  namespace button = constants::menu::button;


  if ( gameState().isPaused == false )
  {
    setRenderColor(constants::colors::background);
    SDL_RenderClear(gRenderer);

    draw_background();
    draw_barn();
  }

  DrawMenuRect();
  DrawButton();


  draw_text( "SETTINGS       ",       0.025f, 0.2855f );
  draw_text( "Controls       ",       0.040f, 0.2855f + 0.0721f );
  draw_text( "Audio volume:  ",       0.040f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( mInputAudioVolume + "%", 0.500f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Audio panning: ",       0.040f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( mInputStereoDepth + "%", 0.500f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "Reset stats    ",       0.040f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( "Back           ",       0.040f, 0.2855f + 0.0721f + button::sizeY * 4.f );


  if ( isSpecifyingVar(MENU_SPECIFY::AUDIO_VOLUME) == true )
  {
    draw_text( "Press [RETURN] to finish", 0.250f, 0.600f );
    draw_text( "specifying audio volume ", 0.250f, 0.650f );

    return;
  }

  if ( isSpecifyingVar(MENU_SPECIFY::STEREO_DEPTH) == true )
  {
    draw_text( "Press [RETURN] to finish", 0.250f, 0.600f );
    draw_text( "specifying stereo depth ", 0.250f, 0.650f );

    return;
  }


  switch (mSelectedItem)
  {
    case MENU_SETTINGS::AUDIO_VOLUME:
    {
      draw_text( "Press [RETURN] to specify      ", 0.005f, 0.700f );
      draw_text( "                   audio volume", 0.005f, 0.750f );
      break;
    }

    case MENU_SETTINGS::STEREO_DEPTH:
    {
      draw_text( "Press [RETURN] to specify      ", 0.005f, 0.700f );
      draw_text( "                   stereo depth", 0.005f, 0.750f );
      break;
    }

    case MENU_SETTINGS::STATS_RESET:
    {
      draw_text( "Press [RETURN] to reset        ", 0.005f, 0.700f );
      draw_text( "                    your stats ", 0.005f, 0.750f );
      break;
    }

    default:
      break;
  }
}

void
Menu::screen_controls()
{
  namespace button = constants::menu::button;


  if ( gameState().isPaused == false )
  {
    setRenderColor(constants::colors::background);
    SDL_RenderClear(gRenderer);

    draw_background();
    draw_barn();
  }

  DrawMenuRect();
  DrawButton();


  draw_text( "CONTROLS            ",        0.025f, 0.2855f );
  draw_text( "Accelerate          ",        0.025f, 0.2855f + 0.0721f );
  draw_text( SDL_GetScancodeName(THROTTLE_UP),   0.700f, 0.2855f + 0.0721f );
  draw_text( "Decelerate          ",        0.025f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( SDL_GetScancodeName(THROTTLE_DOWN), 0.700f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Turn Anti-Clockwise ",        0.025f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( SDL_GetScancodeName(TURN_LEFT),     0.700f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "Turn Clockwise      ",        0.025f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( SDL_GetScancodeName(TURN_RIGHT),    0.700f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( "Fire                ",        0.025f, 0.2855f + 0.0721f + button::sizeY * 4.f );
  draw_text( SDL_GetScancodeName(FIRE),          0.700f, 0.2855f + 0.0721f + button::sizeY * 4.f );
  draw_text( "Eject               ",        0.025f, 0.2855f + 0.0721f + button::sizeY * 5.f );
  draw_text( SDL_GetScancodeName(JUMP),          0.700f, 0.2855f + 0.0721f + button::sizeY * 5.f );
  draw_text( "Back                ",        0.025f, 0.2855f + 0.0721f + button::sizeY * 6.f );

  if ( menu.isDefiningKey() == true )
  {
    draw_text( "     Press the key you wish    ", 0.025f, 0.05f );
    draw_text( "  to assign to this function.  ", 0.025f, 0.05f * 2.0f );
    draw_text( "     Press [ESC] to cancel.    ", 0.025f, 0.05f * 3.5f );
  }
  else
  {
    draw_text( "Press[RETURN] to remap selected", 0.025f, 0.025f );
    draw_text( "          key binding.         ", 0.025f, 0.075f );
    draw_text( "  Press [DELETE]  to reset it  ", 0.025f, 0.05f * 3.f );
    draw_text( "       to default value.       ", 0.025f, 0.05f * 4.f );
  }
}

void
Menu::screen_pause()
{
  namespace button = constants::menu::button;


  DrawMenuRect();
  DrawButton();

  draw_text( "GAME PAUSED       ", 0.250f, 0.2855f );
  draw_text( "On With the Show! ", 0.255f, 0.2855f + 0.0721f );
  draw_text( "Settings          ", 0.255f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Help              ", 0.255f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "Bail Out!         ", 0.255f, 0.2855f + 0.0721f + button::sizeY * 3.f );
}

void
Menu::screen_copyright()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  draw_text( "      'Bluetooth Biplanes'      ", 0, 0.350f );
  draw_text( "      @ Morpheme Ltd. 2004      ", 0, 0.400f );
  draw_text( "       All Rights Reserved      ", 0, 0.450f );
  draw_text( "       www.morpheme.co.uk       ", 0, 0.500f );

  draw_text( "  Brought to PC on 01.04.20 by  ", 0, 0.650f );
  draw_text( "       casqade & xion  at       ", 0, 0.700f );
  draw_text( "     github.com/regular-dev     ", 0, 0.750f );
  draw_text( "         regular-dev.org        ", 0, 0.800f );
}

void
Menu::screen_splash()
{
  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  const SDL_FRect logoRect
  {
    toWindowSpaceX(0.5f - 0.5f / constants::aspectRatio),
    toWindowSpaceY(0.0f),
    scaleToScreenX(1.0f / constants::aspectRatio),
    scaleToScreenY(1.0f),
  };

  SDL_RenderCopyF(
    gRenderer,
    textures.menu_logo,
    nullptr,
    &logoRect );
}
