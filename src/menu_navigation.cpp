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
#include <include/biplanes.hpp>
#include <include/game_state.hpp>
#include <include/network.hpp>
#include <include/network_state.hpp>
#include <include/matchmake.hpp>
#include <include/plane.hpp>
#include <include/variables.hpp>
#include <include/utility.hpp>


void
Menu::Select()
{
  if ( mSelectedItem == mButtons[mCurrentRoom] )
  {
    if ( mCurrentRoom == ROOMS::MENU_MAIN )
      gameState().isExiting = true;

    else if ( mCurrentRoom != ROOMS::MENU_PAUSE )
      return GoBack();
  }

  switch (mCurrentRoom)
  {
    case ROOMS::MENU_MAIN:
    {
      switch (mSelectedItem)
      {
        case MENU_MAIN::SINGLEPLAYER:
        {
          gameState().gameMode = GAME_MODE::HUMAN_VS_BOT;
          ChangeRoom(ROOMS::MENU_SP);
          break;
        }

        case MENU_MAIN::MULTIPLAYER:
        {
          gameState().gameMode = GAME_MODE::HUMAN_VS_HUMAN;
          ChangeRoom(ROOMS::MENU_MP);
          break;
        }

        case MENU_MAIN::SETTINGS:
        {
          ChangeRoom(ROOMS::MENU_SETTINGS_CONTROLS);
          break;
        }

        case MENU_MAIN::HELP:
        {
          ChangeRoom(ROOMS::MENU_HELP);
          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_HELP:
      break;

    case ROOMS::MENU_SP:
    {
      switch (mSelectedItem)
      {
        case MENU_SP::SETUP_GAME:
        {
//          TODO: if ai difficulty is evolve:
//                switch to room MENU_SP_SETUP_NN
          ChangeRoom(ROOMS::MENU_SP_SETUP);
          mInputScoreToWin = std::to_string(gameState().winScore);
          break;
        }

        case MENU_SP::AI_MODE:
        {
          auto& gameMode = gameState().gameMode;

          if ( gameMode != GAME_MODE::HUMAN_VS_BOT )
            gameMode = GAME_MODE::HUMAN_VS_BOT;
          else
            gameMode = GAME_MODE::BOT_VS_BOT;

          break;
        }

        case MENU_SP::AI_DIFFICULTY:
        {
          auto& difficulty = gameState().botDifficulty;

          if ( difficulty < DIFFICULTY::EVOLVE )
            difficulty = static_cast <DIFFICULTY> (static_cast <uint8_t> (difficulty) + 1);
          else
            difficulty = DIFFICULTY::EASY;

          break;
        }

        case MENU_SP::BACK:
        {
          GoBack();
          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_SP_SETUP:
    {
      switch (mSelectedItem)
      {
        case MENU_SP_SETUP::START:
        {
          auto& gameMode = gameState().gameMode;
          auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
          auto& planeRed = planes.at(PLANE_TYPE::RED);

          planeBlue.setLocal(true);
          planeRed.setLocal(true);

          if ( gameMode == GAME_MODE::HUMAN_VS_BOT )
          {
            planeBlue.setBot(false);
            planeRed.setBot(true);
          }
          else
          {
            planeBlue.setBot(true);
            planeRed.setBot(true);
          }

          game_init_sp();
          ChangeRoom(ROOMS::GAME);

          break;
        }

        case MENU_SP_SETUP::WIN_SCORE:
        {
          ToggleTyping(MENU_SPECIFY::WIN_SCORE);
          break;
        }

        case MENU_SP_SETUP::HARDCORE_MODE:
        {
//          TODO: change to HARDCORE_MODE_SP ?
          auto& isHardcoreEnabled = gameState().isHardcoreEnabled;
          isHardcoreEnabled = !isHardcoreEnabled;

          settingsWrite();
          break;
        }

        case MENU_SP_SETUP::BACK:
        {
          GoBack();
          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_MP:
    {
      switch (mSelectedItem)
      {
        case MENU_MP::MMAKE:
        {
          ChangeRoom(ROOMS::MENU_MP_MMAKE);
          mInputPassword = MMAKE_PASSWORD;

          break;
        }

        case MENU_MP::DC:
        {
          ChangeRoom(ROOMS::MENU_MP_DC);
          break;
        }

        case MENU_MP::HELP:
        {
          ChangeRoom(ROOMS::MENU_MP_HELP_PAGE1);
          break;
        }

        case MENU_MP::BACK:
        {
          GoBack();
          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE1:
    {
      GoBack();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE2:
    {
      GoBack();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE3:
    {
      GoBack();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE4:
    {
      GoBack();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE5:
    {
      GoBack();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE6:
    {
      GoBack();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE7:
    {
      GoBack();
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE8:
    {
      GoBack();
      break;
    }

    case ROOMS::MENU_MP_MMAKE:
    {
      switch (mSelectedItem)
      {
        case MENU_MP_MMAKE::FIND_GAME:
        {
          setMessage(MESSAGE_TYPE::MMAKE_SEARCHING_OPPONENT);

          const auto matchmaker = networkState().matchmaker;

          matchmaker->setPassword(
            MMAKE_PASSWORD_PREFIX + MMAKE_PASSWORD );

          if ( matchmaker->initNewSession() == true )
            ChangeRoom(ROOMS::MENU_MP_MMAKE_FIND_GAME);

          break;
        }

        case MENU_MP_MMAKE::SPECIFY_PASSWORD:
        {
          ToggleTyping(MENU_SPECIFY::PASSWORD);
          break;
        }

        case MENU_MP_MMAKE::HARDCORE_MODE:
        {
          auto& isHardcoreEnabled = gameState().isHardcoreEnabled;
          isHardcoreEnabled = !isHardcoreEnabled;

          settingsWrite();
          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_MP_MMAKE_FIND_GAME:
    {
//      THINK: Session Browser?
      break;
    }

    case ROOMS::MENU_MP_DC:
    {
      switch (mSelectedItem)
      {
        case MENU_MP_DC::HOST:
        {
          ChangeRoom(ROOMS::MENU_MP_DC_HOST);
          mInputPortHost = std::to_string(LOCAL_PORT);

          break;
        }

        case MENU_MP_DC::JOIN:
        {
          ChangeRoom(ROOMS::MENU_MP_DC_JOIN);
          mInputIp = SERVER_IP;
          mInputPortClient = std::to_string(REMOTE_PORT);

          break;
        }

        case MENU_MP_DC::HELP:
        {
          ChangeRoom(ROOMS::MENU_MP_DC_HELP);
          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_MP_DC_HOST:
    {
      switch (mSelectedItem)
      {
        case MENU_MP_DC_HOST::HOST_START:
        {
          auto& network = networkState();
          auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
          auto& planeRed = planes.at(PLANE_TYPE::RED);

          network.nodeType = SRV_CLI::SERVER;

          planeBlue.setLocal(true);
          planeRed.setLocal(false);

          planeBlue.setBot(false);
          planeRed.setBot(false);

          if ( game_init_mp() != 0 )
          {
            log_message( "\nLOG: Failed to initialize game!\n\n" );
            network.connection->Stop();

            ReturnToMainMenu();
          }
          else
            ChangeRoom(ROOMS::GAME);

          break;
        }

        case MENU_MP_DC_HOST::SPECIFY_PORT:
        {
          ToggleTyping(MENU_SPECIFY::PORT);
          break;
        }

        case MENU_MP_DC_HOST::HARDCORE_MODE:
        {
          auto& isHardcoreEnabled = gameState().isHardcoreEnabled;
          isHardcoreEnabled = !isHardcoreEnabled;

          settingsWrite();
          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_MP_DC_JOIN:
    {
      switch (mSelectedItem)
      {
        case MENU_MP_DC_JOIN::JOIN:
        {
          auto& network = networkState();
          auto& planeBlue = planes.at(PLANE_TYPE::BLUE);
          auto& planeRed = planes.at(PLANE_TYPE::RED);

          network.nodeType = SRV_CLI::CLIENT;

          planeBlue.setLocal(false);
          planeRed.setLocal(true);

          planeBlue.setBot(false);
          planeRed.setBot(false);

//          Automatically switches off if
//          the opponent has this option disabled
          gameState().isHardcoreEnabled = true;

          if ( game_init_mp() != 0 )
          {
            log_message( "\nLOG: Failed to initialize game!\n\n" );
            network.connection->Stop();

            ReturnToMainMenu();
          }
          else
            ChangeRoom(ROOMS::GAME);

          break;
        }

        case MENU_MP_DC_JOIN::SPECIFY_IP:
        {
          ToggleTyping(MENU_SPECIFY::IP);
          break;
        }

        case MENU_MP_DC_JOIN::SPECIFY_PORT:
        {
          ToggleTyping(MENU_SPECIFY::PORT);
          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_MP_DC_HELP:
    {
      GoBack();
      break;
    }

    case ROOMS::MENU_SETTINGS_CONTROLS:
    {
      switch (mSelectedItem)
      {
        case MENU_SETTINGS_CONTROLS::ACCELERATE:
        {
          ToggleDefiningKey(MENU_SETTINGS_CONTROLS::ACCELERATE);
          break;
        }

        case MENU_SETTINGS_CONTROLS::DECELERATE:
        {
          ToggleDefiningKey(MENU_SETTINGS_CONTROLS::DECELERATE);
          break;
        }

        case MENU_SETTINGS_CONTROLS::LEFT:
        {
          ToggleDefiningKey(MENU_SETTINGS_CONTROLS::LEFT);
          break;
        }

        case MENU_SETTINGS_CONTROLS::RIGHT:
        {
          ToggleDefiningKey(MENU_SETTINGS_CONTROLS::RIGHT);
          break;
        }

        case MENU_SETTINGS_CONTROLS::SHOOT:
        {
          ToggleDefiningKey(MENU_SETTINGS_CONTROLS::SHOOT);
          break;
        }

        case MENU_SETTINGS_CONTROLS::EJECT:
        {
          ToggleDefiningKey(MENU_SETTINGS_CONTROLS::EJECT);
          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_PAUSE:
    {
      switch (mSelectedItem)
      {
        case MENU_PAUSE::CONTINUE:
        {
          GoBack();
          gameState().isPaused = false;
          break;
        }

        case MENU_PAUSE::CONTROLS:
        {
          ChangeRoom(ROOMS::MENU_SETTINGS_CONTROLS);
          break;
        }

        case MENU_PAUSE::HELP:
        {
          ChangeRoom(ROOMS::MENU_HELP);
          break;
        }

        case MENU_PAUSE::DISCONNECT:
        {
          if ( gameState().gameMode == GAME_MODE::HUMAN_VS_HUMAN )
          {
            sendDisconnectMessage();
            networkState().connection->Stop();
          }

          ReturnToMainMenu();
          setMessage(MESSAGE_TYPE::NONE);

          break;
        }

        default:
          break;
      }

      break;
    }

    case ROOMS::MENU_COPYRIGHT:
    {
      mIntroAutoSkipTimer.Start();
      ChangeRoom(ROOMS::MENU_SPLASH);
      break;
    }

    case ROOMS::MENU_SPLASH:
    {
      ChangeRoom(ROOMS::MENU_MAIN);
      break;
    }

    default:
      break;
  }
}

void
Menu::GoBack()
{
  switch (mCurrentRoom)
  {
    case ROOMS::MENU_COPYRIGHT:
    {
      mIntroAutoSkipTimer.Start();
      ChangeRoom(ROOMS::MENU_SPLASH);
      break;
    }

    case ROOMS::MENU_SPLASH:
    {
      ChangeRoom(ROOMS::MENU_MAIN);
      break;
    }

    case ROOMS::MENU_MAIN:
    {
//      THINK: Exit confirmation?
//      gameState().isExiting = true; // exit using ESCAPE key
      break;
    }

    case ROOMS::MENU_SP:
    {
      ChangeRoom(ROOMS::MENU_MAIN);
      break;
    }

    case ROOMS::MENU_SP_SETUP:
    {
      ChangeRoom(ROOMS::MENU_SP);
      break;
    }

    case ROOMS::MENU_MP:
    {
      ChangeRoom(ROOMS::MENU_MAIN);
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE1:
    {
      ChangeRoom(ROOMS::MENU_MP_HELP_PAGE2);
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE2:
    {
      ChangeRoom(ROOMS::MENU_MP_HELP_PAGE3);
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE3:
    {
      ChangeRoom(ROOMS::MENU_MP_HELP_PAGE4);
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE4:
    {
      ChangeRoom(ROOMS::MENU_MP_HELP_PAGE5);
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE5:
    {
      ChangeRoom(ROOMS::MENU_MP_HELP_PAGE6);
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE6:
    {
      ChangeRoom(ROOMS::MENU_MP_HELP_PAGE7);
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE7:
    {
      ChangeRoom(ROOMS::MENU_MP_HELP_PAGE8);
      break;
    }

    case ROOMS::MENU_MP_HELP_PAGE8:
    {
      ChangeRoom(ROOMS::MENU_MP);
      break;
    }

    case ROOMS::MENU_MP_MMAKE:
    {
      ChangeRoom(ROOMS::MENU_MP);
      break;
    }

    case ROOMS::MENU_MP_MMAKE_FIND_GAME:
    {
      const auto matchmaker = networkState().matchmaker;

      matchmaker->Reset();
      matchmaker->sendStatus(
        MatchConnectStatus::GOODBYE,
        MatchMaker::GetServerAddress() );

      log_message( "NETWORK MMAKE: Sent goodbye message to matchmake server", "\n\n" );

      ChangeRoom(ROOMS::MENU_MP);
      setMessage(MESSAGE_TYPE::NONE);

      break;
    }

    case ROOMS::MENU_MP_DC:
    {
      ChangeRoom(ROOMS::MENU_MP);
      break;
    }

    case ROOMS::MENU_MP_DC_HOST:
    {
      ChangeRoom(ROOMS::MENU_MP_DC);
      break;
    }

    case ROOMS::MENU_MP_DC_JOIN:
    {
      ChangeRoom(ROOMS::MENU_MP_DC);
      break;
    }

    case ROOMS::MENU_MP_DC_HELP:
    {
      ChangeRoom(ROOMS::MENU_MP_DC);
      break;
    }

    case ROOMS::MENU_SETTINGS_CONTROLS:
    {
      if ( mIsDefiningKey ==  true )
        ToggleDefiningKey(mKeyToDefine);

      settingsWrite();

      if ( gameState().isPaused == true )
        ChangeRoom(ROOMS::MENU_PAUSE);
      else
        ChangeRoom(ROOMS::MENU_MAIN);

      break;
    }

    case ROOMS::MENU_HELP:
    {
      if ( gameState().isPaused == true )
        ChangeRoom(ROOMS::MENU_PAUSE);
      else
        ChangeRoom(ROOMS::MENU_MAIN);

      break;
    }

    case ROOMS::MENU_RECENT_STATS_PAGE1:
    {
      ChangeRoom(ROOMS::MENU_RECENT_STATS_PAGE2);
      break;
    }

    case ROOMS::MENU_RECENT_STATS_PAGE2:
    {
      ChangeRoom(ROOMS::MENU_TOTAL_STATS_PAGE1);
      break;
    }

    case ROOMS::MENU_TOTAL_STATS_PAGE1:
    {
      ChangeRoom(ROOMS::MENU_TOTAL_STATS_PAGE2);
      break;
    }

    case ROOMS::MENU_TOTAL_STATS_PAGE2:
    {
      ChangeRoom(ROOMS::MENU_MAIN);
      break;
    }

    case ROOMS::GAME:
    {
      ChangeRoom(ROOMS::MENU_PAUSE);
      gameState().isPaused = true;

      break;
    }

    case ROOMS::MENU_PAUSE:
    {
      ChangeRoom(ROOMS::GAME);
      gameState().isPaused = false;

      break;
    }

    default:
      break;
  }
}

void
Menu::MenuItemPrevious()
{
  if ( mSelectedItem > 0 )
    --mSelectedItem;
  else
    mSelectedItem = mButtons[mCurrentRoom];
}

void
Menu::MenuItemNext()
{
  if ( mSelectedItem < mButtons[mCurrentRoom] )
    ++mSelectedItem;
  else
    mSelectedItem = 0;
}

void
Menu::ReturnToMainMenu()
{
  gameState().isRoundFinished = false;
  gameState().isPaused = false;
  networkState().isOpponentConnected = false;

  ChangeRoom(ROOMS::MENU_MAIN);
}

void
Menu::ChangeRoom(
  const ROOMS room )
{
  mCurrentRoom = room;
  mSelectedItem = 0;
}
