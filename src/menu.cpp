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
#include <include/render.h>
#include <include/init_vars.h>
#include <include/matchmake.hpp>
#include <include/variables.h>

#include <cmath>


Menu::Menu()
{
  buttons =
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
//  {ROOMS::MENU_MP_MMAKE_FIND_GAME, MENU_MP_MMAKE_FIND_GAME::BACK},

  {ROOMS::MENU_MP_DC, MENU_MP_DC::BACK},
  {ROOMS::MENU_MP_DC_HOST, MENU_MP_DC_HOST::BACK},
  {ROOMS::MENU_MP_DC_JOIN, MENU_MP_DC_JOIN::BACK},
  {ROOMS::MENU_MP_DC_HELP, 0},

  {ROOMS::MENU_PAUSE, MENU_PAUSE::DISCONNECT},
  };

  inputIp = SERVER_IP;
  inputPortHost = std::to_string( HOST_PORT );
  inputPortClient = std::to_string( SERVER_PORT );
  inputPass = MMAKE_PASSWORD;
}

void Menu::ResizeWindow()
{
  if ( event.type == SDL_WINDOWEVENT )
  {
    if (  event.window.event == SDL_WINDOWEVENT_RESIZED ||
          event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
          event.window.event == SDL_WINDOWEVENT_MOVED )
    {
      if ( current_room != ROOMS::GAME && !game_pause )
        SDL_GetRendererOutputSize( gRenderer, &sizes.screen_width_new, &sizes.screen_height_new );

      if (  sizes.screen_height_new != sizes.screen_height ||
            sizes.screen_width_new != sizes.screen_width )
        window_resize();
      else if ( SDL_GetWindowDisplayIndex( gWindow ) != DISPLAY_INDEX )
        window_resize();
    }
  }
}

void Menu::DrawMenu()
{
  if ( game_pause )
    game_loop();
  else
    connected_message_timer->Update();

  switch ( menu.current_room )
  {
  case ROOMS::MENU_COPYRIGHT:
  {
    menu_copyright();
    break;
  }
  case ROOMS::MENU_SPLASH:
  {
    menu_splash();
    break;
  }
  case ROOMS::MENU_MAIN:
  {
    menu_main();
    break;
  }
  case ROOMS::MENU_SP:
  {
    menu_sp();
    break;
  }
  case ROOMS::MENU_SP_SETUP:
  {
    menu_sp_setup();
    break;
  }
  case ROOMS::MENU_MP:
  {
    menu_mp();
    break;
  }
  case ROOMS::MENU_MP_HELP_PAGE1:
  {
    menu_mp_help_page1();
    break;
  }
  case ROOMS::MENU_MP_HELP_PAGE2:
  {
    menu_mp_help_page2();
    break;
  }
  case ROOMS::MENU_MP_HELP_PAGE3:
  {
    menu_mp_help_page3();
    break;
  }
  case ROOMS::MENU_MP_HELP_PAGE4:
  {
    menu_mp_help_page4();
    break;
  }
  case ROOMS::MENU_MP_HELP_PAGE5:
  {
    menu_mp_help_page5();
    break;
  }
  case ROOMS::MENU_MP_HELP_PAGE6:
  {
    menu_mp_help_page6();
    break;
  }
  case ROOMS::MENU_MP_HELP_PAGE7:
  {
    menu_mp_help_page7();
    break;
  }
  case ROOMS::MENU_MP_HELP_PAGE8:
  {
    menu_mp_help_page8();
    break;
  }
  case ROOMS::MENU_MP_MMAKE:
  {
    menu_mp_mmake();
    break;
  }
  case ROOMS::MENU_MP_MMAKE_FIND_GAME:
  {
    menu_mp_mmake();

    MatchMakerState mmakeState = MatchMaker::Inst().state();

    if ( mmakeState == MatchMakerState::MATCH_READY )
    {
      if ( MatchMaker::Inst().srv_or_cli() )
      {
        net::Address opp = MatchMaker::Inst().opponentAddress();
        srv_or_cli = SRV_CLI::CLIENT;
        SERVER_IP =
            opp.GetA() + "." +
            opp.GetB() + "." +
            opp.GetC() + "." +
            opp.GetD();

        SERVER_PORT = opp.GetPort();
      }
      else
        srv_or_cli = SRV_CLI::SERVER;

      if ( game_init_mp() )
      {
        log_message( "\nLOG: Failed to initialize game!\n\n" );
        connection->Stop();

        ReturnToMainMenu();
      }
      else
      {
        MatchMaker::Inst().Reset();
        ChangeRoom( ROOMS::GAME );
      }
      break;
    }
    else if ( mmakeState == MatchMakerState::MATCH_TIMEOUT )
    {
      MatchMaker::Inst().Reset();
      ChangeRoom( ROOMS::MENU_MP_MMAKE );
      log_message( "NETWORK: Peer-to-Peer connection timeout!\n" );
      setMessage( MESSAGE_TYPE::MMAKE_PTP_TIMEOUT );
      break;
    }
    break;
  }
  case ROOMS::MENU_MP_DC:
  {
    menu_mp_dc();
    break;
  }
  case ROOMS::MENU_MP_DC_HOST:
  {
    menu_mp_dc_host();
    break;
  }
  case ROOMS::MENU_MP_DC_JOIN:
  {
    menu_mp_dc_join();
    break;
  }
  case ROOMS::MENU_MP_DC_HELP:
  {
    menu_mp_dc_help();
    break;
  }
  case ROOMS::MENU_SETTINGS_CONTROLS:
  {
    menu_settings_controls();
    break;
  }
  case ROOMS::MENU_HELP:
  {
    menu_help();
    break;
  }
  case ROOMS::MENU_RECENT_STATS:
  {
    menu_recent_stats();
    break;
  }
  case ROOMS::MENU_TOTAL_STATS_PAGE1:
  {
    menu_total_stats_page1();
    break;
  }
  case ROOMS::MENU_TOTAL_STATS_PAGE2:
  {
    menu_total_stats_page2();
    break;
  }
  case ROOMS::MENU_PAUSE:
  {
    menu_pause();
    break;
  }
  case ROOMS::GAME:
  {
      game_loop_mp();
    break;
  }
  default:
    break;
  }


  switch ( message )
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
    sprintf( textbuf, "Connecting to %s:%d", SERVER_IP.c_str(), SERVER_PORT );
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
    if ( !connected_message_timer->isReady() )
    {
      if ( game_pause )
        break;

      if ( srv_or_cli == SRV_CLI::CLIENT )
      {
        draw_text( "Successfully connected to server", 0, 0 );
        draw_text( "It's You!", plane_red.getX() - sizes.text_sizex * 7, plane_red.getY() - sizes.screen_height * 0.1 );
      }
      else
      {
        draw_text( "New client connected!", 0, 0 );
        draw_text( "It's You!", plane_blue.getX() - sizes.text_sizex, plane_blue.getY() - sizes.screen_height * 0.1 );
      }
    }
    else
      setMessage( MESSAGE_TYPE::NONE );

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
  case MESSAGE_TYPE::MMAKE_SEARCHING_OPP:
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
  {
    break;
  }
  default:
    break;
  }
  AnimateButton();
}

void Menu::setMessage( MESSAGE_TYPE message_type )
{
  message = message_type;
  if ( message == MESSAGE_TYPE::SUCCESSFULL_CONNECTION )
    connected_message_timer->Start();
}

void Menu::ChangeRoom( ROOMS new_room )
{
  current_room = new_room;
  button_selected = 0;
  button_pressed = true;
}

void Menu::UpdateControls()
{
  const Uint8 *keyboard_state = SDL_GetKeyboardState( NULL );
  if  ( !keyboard_state[SDL_SCANCODE_RETURN] &&
        !keyboard_state[SDL_SCANCODE_DOWN] &&
        !keyboard_state[SDL_SCANCODE_UP] &&
        !keyboard_state[SDL_SCANCODE_SPACE] &&
        !keyboard_state[SDL_SCANCODE_ESCAPE] &&
        !keyboard_state[SDL_SCANCODE_F1] && button_pressed )
    button_pressed = false;

  if ( current_room != ROOMS::GAME )
  {
    if ( typing )
      UpdateTyping();
    else if ( defining_key )
      UpdateDefiningKey();
    else
    {
      if ( keyboard_state[SDL_SCANCODE_DOWN] && !button_pressed )
        ButtonDown();
      else if ( keyboard_state[SDL_SCANCODE_UP] && !button_pressed )
        ButtonUp();
      else if ( keyboard_state[SDL_SCANCODE_ESCAPE] && !button_pressed )
        GoBack();
      else if ( keyboard_state[SDL_SCANCODE_DELETE] && !button_pressed )
        ResetKey();
      else if ( keyboard_state[SDL_SCANCODE_SPACE] && !button_pressed )
        GoBack();
      else if ( keyboard_state[SDL_SCANCODE_F1] && !button_pressed )
        if ( current_room == ROOMS::MENU_MAIN )
        {
          setMessage( MESSAGE_TYPE::NONE );
          ChangeRoom( ROOMS::MENU_RECENT_STATS );
        }

      if ( event.type == SDL_QUIT && !game_pause )
        game_exit = true;
    }

    if ( keyboard_state[SDL_SCANCODE_RETURN] && !button_pressed )
      Select();
  }
  else
  {
    if ( keyboard_state[SDL_SCANCODE_ESCAPE] && !button_pressed )
      GoBack();
    else if ( !keyboard_state[SDL_SCANCODE_ESCAPE] && button_pressed )
      button_pressed = false;
  }
}

void Menu::AnimateButton()
{
  if ( sizes.button_dir == BUTTON_DIR::RIGHT )
  {
    if ( sizes.button_x < 127 - deltaTime * sizes.screen_width * 0.075 )
      sizes.button_x += deltaTime * sizes.screen_width * 0.075;
    else
      sizes.button_dir = BUTTON_DIR::LEFT;
  }
  else
  {
    if ( sizes.button_x > deltaTime * sizes.screen_width * 0.075 )
      sizes.button_x -= deltaTime * sizes.screen_width * 0.075;
    else
      sizes.button_dir = BUTTON_DIR::RIGHT;
  }
}

void Menu::ButtonUp()
{
  button_pressed = true;
  if ( button_selected > 0 )
    button_selected--;
  else
    button_selected = buttons[current_room];
}

void Menu::ButtonDown()
{
  button_pressed = true;
  if ( button_selected < buttons[current_room] )
    button_selected++;
  else
    button_selected = 0;
}

void Menu::Select()
{
  button_pressed = true;
  if ( button_selected == buttons[current_room] )
  {
    if ( current_room == ROOMS::MENU_MAIN )
      game_exit = true;
    else if ( current_room != ROOMS::MENU_PAUSE )
    {
      GoBack();
      return;
    }
  }

  switch ( current_room )
  {
    case ROOMS::MENU_MAIN:
    {
      switch ( button_selected )
      {
        case MENU_MAIN::SINGLEPLAYER:
        {
          ChangeRoom( ROOMS::MENU_SP );
          break;
        }
        case MENU_MAIN::MULTIPLAYER:
        {
          ChangeRoom( ROOMS::MENU_MP );
          break;
        }
        case MENU_MAIN::SETTINGS:
        {
          ChangeRoom( ROOMS::MENU_SETTINGS_CONTROLS );
          break;
        }
        case MENU_MAIN::HELP:
        {
          ChangeRoom( ROOMS::MENU_HELP );
//          ChangeRoom( ROOMS::MENU_STATS );
          break;
        }
        default:
          break;
      }
      break;
    }
    case ROOMS::MENU_HELP:
    {
      break;
    }
    case ROOMS::MENU_SP:
    {
      switch ( button_selected )
      {
        case MENU_SP::SETUP_GAME:
        {
          //  if ai difficulty is evolve:
          //  switch to room MENU_SP_SETUP_NN
          ChangeRoom( ROOMS::MENU_SP_SETUP );
          break;
        }
        case MENU_SP::AI_SIDE:
        {
          break;
        }
        case MENU_SP::AI_DIFFICULTY:
        {
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
      switch ( button_selected )
      {
        case MENU_SP_SETUP::START:
        {
          if ( game_init_sp() )
          {
            log_message( "\nLOG: Failed to initialize SP game!\n\n" );

            ReturnToMainMenu();
          }
          else
            ChangeRoom( ROOMS::GAME );

          break;
        }
        case MENU_SP_SETUP::GAME_LENGTH:
        {
          // specify game length [ 0(inf) - 99 ]
          break;
        }
        case MENU_SP_SETUP::HARDCORE_MODE:
        {
          HARDCORE_MODE = !HARDCORE_MODE; // todo: change to HARDCORE_MODE_SP
          settings_write();
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
      switch ( button_selected )
      {
        case MENU_MP::MMAKE:
        {
          ChangeRoom( ROOMS::MENU_MP_MMAKE );
          inputPass = MMAKE_PASSWORD;
          break;
        }
        case MENU_MP::DC:
        {
          ChangeRoom( ROOMS::MENU_MP_DC );
          break;
        }
        case MENU_MP::HELP:
        {
          ChangeRoom( ROOMS::MENU_MP_HELP_PAGE1 );
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
      switch ( button_selected )
      {
        case MENU_MP_MMAKE::FIND_GAME:
        {
          setMessage( MESSAGE_TYPE::MMAKE_SEARCHING_OPP );
          //  Password = prefix + userPassword
          std::string prefixedPassword = MMAKE_PASSWORD_PREFIX + MMAKE_PASSWORD;
          MatchMaker::Inst().setPassword( prefixedPassword );
          MatchMaker::Inst().matchInitForOpponent();

          ChangeRoom( ROOMS::MENU_MP_MMAKE_FIND_GAME );
          break;
        }
        case MENU_MP_MMAKE::SPECIFY_PASSWORD:
        {
          ToggleTyping( SPECIFY::PASSWORD );
          break;
        }
        case MENU_MP_MMAKE::HARDCORE_MODE:
        {
          HARDCORE_MODE = !HARDCORE_MODE;
          settings_write();
          break;
        }
        default:
          break;
      }
      break;
    }
    case ROOMS::MENU_MP_MMAKE_FIND_GAME:
    {

      //      Server Browser?
      break;
    }
    case ROOMS::MENU_MP_DC:
    {
      switch ( button_selected )
      {
        case MENU_MP_DC::HOST:
        {
          ChangeRoom( ROOMS::MENU_MP_DC_HOST );
          inputPortHost = std::to_string( HOST_PORT );
          break;
        }
        case MENU_MP_DC::JOIN:
        {
          ChangeRoom( ROOMS::MENU_MP_DC_JOIN );
          inputIp = SERVER_IP;
          inputPortClient = std::to_string( SERVER_PORT );
          break;
        }
        case MENU_MP_DC::HELP:
        {
          ChangeRoom( ROOMS::MENU_MP_DC_HELP );
          break;
        }
        default:
          break;
      }
      break;
    }
    case ROOMS::MENU_MP_DC_HOST:
    {
      switch ( button_selected )
      {
        case MENU_MP_DC_HOST::HOST_START:
        {
          srv_or_cli = SRV_CLI::SERVER;
          if ( game_init_mp() )
          {
            log_message( "\nLOG: Failed to initialize game!\n\n" );
            connection->Stop();

            ReturnToMainMenu();
          }
          else
            ChangeRoom( ROOMS::GAME );
          break;
        }
        case MENU_MP_DC_HOST::SPECIFY_PORT:
        {
          ToggleTyping( SPECIFY::PORT );
          break;
        }
        case MENU_MP_DC_HOST::HARDCORE_MODE:
        {
          HARDCORE_MODE = !HARDCORE_MODE;
          settings_write();
          break;
        }
        default:
          break;
      }
      break;
    }
    case ROOMS::MENU_MP_DC_JOIN:
    {
      switch ( button_selected )
      {
        case MENU_MP_DC_JOIN::JOIN:
        {
          srv_or_cli = SRV_CLI::CLIENT;
          HARDCORE_MODE = true;

          if ( game_init_mp() )
          {
            log_message( "\nLOG: Failed to initialize game!\n\n" );
            connection->Stop();

            ReturnToMainMenu();
          }
          else
            ChangeRoom( ROOMS::GAME );
          break;
        }
        case MENU_MP_DC_JOIN::SPECIFY_IP:
        {
          ToggleTyping( SPECIFY::IP );
          break;
        }
        case MENU_MP_DC_JOIN::SPECIFY_PORT:
        {
          ToggleTyping( SPECIFY::PORT );
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
      switch ( button_selected )
      {
        case MENU_SETTINGS_CONTROLS::ACCELERATE:
        {
          ToggleDefiningKey( MENU_SETTINGS_CONTROLS::ACCELERATE );
          break;
        }
        case MENU_SETTINGS_CONTROLS::DECELERATE:
        {
          ToggleDefiningKey( MENU_SETTINGS_CONTROLS::DECELERATE );
          break;
        }
        case MENU_SETTINGS_CONTROLS::LEFT:
        {
          ToggleDefiningKey( MENU_SETTINGS_CONTROLS::LEFT );
          break;
        }
        case MENU_SETTINGS_CONTROLS::RIGHT:
        {
          ToggleDefiningKey( MENU_SETTINGS_CONTROLS::RIGHT );
          break;
        }
        case MENU_SETTINGS_CONTROLS::SHOOT:
        {
          ToggleDefiningKey( MENU_SETTINGS_CONTROLS::SHOOT );
          break;
        }
        case MENU_SETTINGS_CONTROLS::EJECT:
        {
          ToggleDefiningKey( MENU_SETTINGS_CONTROLS::EJECT );
          break;
        }
        default:
          break;
      }
      break;
    }
    case ROOMS::MENU_PAUSE:
    {
      switch ( button_selected )
      {
        case MENU_PAUSE::CONTINUE:
        {
          GoBack();
          game_pause = false;
          break;
        }
        case MENU_PAUSE::CONTROLS:
        {
          ChangeRoom( ROOMS::MENU_SETTINGS_CONTROLS );
          break;
        }
        case MENU_PAUSE::HELP:
        {
          ChangeRoom( ROOMS::MENU_HELP );
          break;
        }
        case MENU_PAUSE::DISCONNECT:
        {
          connection->SendDisconnectMessage();
          connection->Stop();

          setMessage( MESSAGE_TYPE::NONE );
          ReturnToMainMenu();
          break;
        }
        default:
          break;
      }
      break;
    }
    case ROOMS::MENU_COPYRIGHT:
    {
      ChangeRoom( ROOMS::MENU_SPLASH );
      break;
    }
    case ROOMS::MENU_SPLASH:
    {
      ChangeRoom( ROOMS::MENU_MAIN );
      break;
    }
    default:
      break;
  }
}

void Menu::GoBack()
{
  button_pressed = true;
  switch ( current_room )
  {
    case ROOMS::MENU_COPYRIGHT:
    {
      ChangeRoom( ROOMS::MENU_SPLASH );
      break;
    }
    case ROOMS::MENU_SPLASH:
    {
      ChangeRoom( ROOMS::MENU_MAIN );
      break;
    }
    case ROOMS::MENU_MAIN:
    {
  //    Exit confirmation?
//      game_exit = true; // exit with ESCAPE key
      break;
    }
    case ROOMS::MENU_SP:
    {
      ChangeRoom( ROOMS::MENU_MAIN );
      break;
    }
    case ROOMS::MENU_SP_SETUP:
    {
      ChangeRoom( ROOMS::MENU_SP );
      break;
    }
    case ROOMS::MENU_MP:
    {
      ChangeRoom( ROOMS::MENU_MAIN );
      break;
    }
    case ROOMS::MENU_MP_HELP_PAGE1:
    {
      ChangeRoom( ROOMS::MENU_MP_HELP_PAGE2 );
      break;
    }
    case ROOMS::MENU_MP_HELP_PAGE2:
    {
      ChangeRoom( ROOMS::MENU_MP_HELP_PAGE3 );
      break;
    }
    case ROOMS::MENU_MP_HELP_PAGE3:
    {
      ChangeRoom( ROOMS::MENU_MP_HELP_PAGE4 );
      break;
    }
    case ROOMS::MENU_MP_HELP_PAGE4:
    {
      ChangeRoom( ROOMS::MENU_MP_HELP_PAGE5 );
      break;
    }
    case ROOMS::MENU_MP_HELP_PAGE5:
    {
      ChangeRoom( ROOMS::MENU_MP_HELP_PAGE6 );
      break;
    }
    case ROOMS::MENU_MP_HELP_PAGE6:
    {
      ChangeRoom( ROOMS::MENU_MP_HELP_PAGE7 );
      break;
    }
    case ROOMS::MENU_MP_HELP_PAGE7:
    {
      ChangeRoom( ROOMS::MENU_MP_HELP_PAGE8 );
      break;
    }
    case ROOMS::MENU_MP_HELP_PAGE8:
    {
      ChangeRoom( ROOMS::MENU_MP );
      break;
    }
    case ROOMS::MENU_MP_MMAKE:
    {
      ChangeRoom( ROOMS::MENU_MP );
      break;
    }
    case ROOMS::MENU_MP_MMAKE_FIND_GAME:
    {
      MatchMaker::Inst().Reset();
      MatchMaker::Inst().matchSendStatus( MatchConnectStatus::GOODBYE, toAddress( MATCHMAKE_SRV_IP, std::to_string( MATCHMAKE_SRV_PORT ) ) );
      log_message( "NETWORK MMAKE: Sent goodbye message to matchmake server", "\n" );

      ChangeRoom( ROOMS::MENU_MP );
      setMessage( MESSAGE_TYPE::NONE );
      break;
    }
    case ROOMS::MENU_MP_DC:
    {
      ChangeRoom( ROOMS::MENU_MP );
      break;
    }
    case ROOMS::MENU_MP_DC_HOST:
    {
      ChangeRoom( ROOMS::MENU_MP_DC );
      break;
    }
    case ROOMS::MENU_MP_DC_JOIN:
    {
      ChangeRoom( ROOMS::MENU_MP_DC );
      break;
    }
    case ROOMS::MENU_MP_DC_HELP:
    {
      ChangeRoom( ROOMS::MENU_MP_DC );
      break;
    }
    case ROOMS::MENU_SETTINGS_CONTROLS:
    {
      if ( defining_key )
        ToggleDefiningKey( define_key );
      settings_write();

      if ( game_pause )
        ChangeRoom( ROOMS::MENU_PAUSE );
      else
        ChangeRoom( ROOMS::MENU_MAIN );
      break;
    }
    case ROOMS::MENU_HELP:
    {
      if ( game_pause )
        ChangeRoom( ROOMS::MENU_PAUSE );
      else
        ChangeRoom( ROOMS::MENU_MAIN );
      break;
    }
    case ROOMS::MENU_RECENT_STATS:
    {
      ChangeRoom( ROOMS::MENU_TOTAL_STATS_PAGE1 );
      break;
    }
    case ROOMS::MENU_TOTAL_STATS_PAGE1:
    {
      ChangeRoom( ROOMS::MENU_TOTAL_STATS_PAGE2 );
      break;
    }
    case ROOMS::MENU_TOTAL_STATS_PAGE2:
    {
      ChangeRoom( ROOMS::MENU_MAIN );
      break;
    }
    case ROOMS::GAME:
    {
      ChangeRoom( ROOMS::MENU_PAUSE );
      game_pause = true;
      break;
    }
    case ROOMS::MENU_PAUSE:
    {
      ChangeRoom( ROOMS::GAME );
      game_pause = false;
      break;
    }
    default:
      break;
  }
}

void Menu::ToggleTyping( unsigned char var_to_specify )
{
  if ( typing )
    EndTyping( var_to_specify );
  else
  {
    typing = true;
    specifying_var[var_to_specify] = true;
    SDL_StartTextInput();
  }
}

void Menu::EndTyping( unsigned char var_to_specify )
{
  typing = false;
  specifying_var[var_to_specify] = false;
  SDL_StopTextInput();

  switch ( var_to_specify )
  {
    case SPECIFY::IP:
    {
      SERVER_IP = checkIp( inputIp );
      if ( SERVER_IP == "" )
        SERVER_IP = DEFAULT_SERVER_IP;
      inputIp = SERVER_IP;
      break;
    }
    case SPECIFY::PORT:
    {
      if ( current_room == ROOMS::MENU_MP_DC_HOST )
      {
        if ( checkPort( inputPortHost ) )
          HOST_PORT = stoi( inputPortHost );
        else
        {
          HOST_PORT = DEFAULT_HOST_PORT;
          inputPortHost = std::to_string( HOST_PORT );
        }
      }
      else
      {
        if ( checkPort( inputPortClient ) )
          SERVER_PORT = stoi( inputPortClient );
        else
        {
          SERVER_PORT = DEFAULT_SERVER_PORT;
          inputPortClient = std::to_string( SERVER_PORT );
        }
      }
      break;
    }
    case SPECIFY::PASSWORD:
    {
      if ( checkPass( inputPass ) )
        MMAKE_PASSWORD = inputPass;
      else
        MMAKE_PASSWORD = DEFAULT_MMAKE_PASSWORD;
      break;
    }
    default:
      break;
  }
  settings_write();
}

void Menu::UpdateTyping()
{
  SDL_StartTextInput();
  if ( specifying_var[SPECIFY::IP] )
  {
    if ( event.type == SDL_KEYDOWN )
    {
      if ( event.key.keysym.sym == SDLK_BACKSPACE && inputIp.length() > 0 )
        inputIp.pop_back();
      else if ( event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
        SDL_SetClipboardText( inputIp.c_str() );
      else if ( event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
        if ( !checkIp( SDL_GetClipboardText() ).empty() )
          inputIp = SDL_GetClipboardText();
    }
    if ( event.type == SDL_TEXTINPUT )
    {
      if ( !( ( event.text.text[0] == 'c' ||
                event.text.text[0] == 'C') &&
              ( event.text.text[0] == 'v' ||
                event.text.text[0] == 'V' ) &&
                SDL_GetModState() & KMOD_CTRL ) )
      {
        if ( inputIp.length() < 15 )
          inputIp += event.text.text;
      }
      SDL_StopTextInput();
    }
  }
  else if ( specifying_var[SPECIFY::PORT] )
  {
    std::string inputPort = "";
    if ( current_room == ROOMS::MENU_MP_DC_HOST )
      inputPort = inputPortHost;
    else
      inputPort = inputPortClient;

    if ( event.type == SDL_KEYDOWN )
    {
      if ( event.key.keysym.sym == SDLK_BACKSPACE && inputPort.length() > 0 )
        inputPort.pop_back();
      else if ( event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
        SDL_SetClipboardText(inputPort.c_str());
      else if ( event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
        if ( checkPort( SDL_GetClipboardText() ) )
          inputPort = SDL_GetClipboardText();
    }
    else if ( event.type == SDL_TEXTINPUT )
    {
      if ( !( ( event.text.text[0] == 'c' ||
                event.text.text[0] == 'C') &&
              ( event.text.text[0] == 'v' ||
                event.text.text[0] == 'V' ) &&
                SDL_GetModState() & KMOD_CTRL ) )
      {
        if ( inputPort.length() < 5 )
          inputPort += event.text.text;
      }
    }
    if ( current_room == ROOMS::MENU_MP_DC_HOST )
      inputPortHost = inputPort;
    else
      inputPortClient = inputPort;
  }
  else if ( specifying_var[SPECIFY::PASSWORD] )
  {
    if ( event.type == SDL_KEYDOWN )
    {
      if ( event.key.keysym.sym == SDLK_BACKSPACE && inputPass.length() > 0 )
        inputPass.pop_back();
      else if ( event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
        SDL_SetClipboardText(inputPass.c_str());
      else if ( event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
        if ( checkPass( SDL_GetClipboardText() ) )
          inputPass = SDL_GetClipboardText();
    }
    else if ( event.type == SDL_TEXTINPUT )
    {
      if ( !( ( event.text.text[0] == 'c' ||
                event.text.text[0] == 'C') &&
              ( event.text.text[0] == 'v' ||
                event.text.text[0] == 'V' ) &&
                SDL_GetModState() & KMOD_CTRL ) )
      {
        if ( inputPass.length() < 15 )
          if ( checkPass( std::string( event.text.text ) ) )
            inputPass += event.text.text;
      }
    }
  }
}

void Menu::ToggleDefiningKey( unsigned char action_to_define )
{
  if ( defining_key )
    defining_key = false;
  else
  {
    defining_key = true;
    define_key = action_to_define;
  }
}

void Menu::UpdateDefiningKey()
{
  const Uint8 *keyboard_state = SDL_GetKeyboardState( NULL );
  if (  keyboard_state[SDL_SCANCODE_ESCAPE] ||
      ( keyboard_state[SDL_SCANCODE_RETURN] &&
        !button_pressed ) )
  {
    button_pressed = true;
    ToggleDefiningKey( define_key );
    return;
  }

  if ( event.type == SDL_KEYDOWN && !button_pressed )
  {
    switch ( define_key )
    {
      case ( MENU_SETTINGS_CONTROLS::ACCELERATE ):
      {
        THROTTLE_UP = event.key.keysym.sym;
        break;
      }
      case ( MENU_SETTINGS_CONTROLS::DECELERATE ):
      {
        THROTTLE_DOWN = event.key.keysym.sym;
        break;
      }
      case ( MENU_SETTINGS_CONTROLS::LEFT ):
      {
        TURN_LEFT = event.key.keysym.sym;
        break;
      }
      case ( MENU_SETTINGS_CONTROLS::RIGHT ):
      {
        TURN_RIGHT = event.key.keysym.sym;
        break;
      }
      case ( MENU_SETTINGS_CONTROLS::SHOOT ):
      {
        FIRE = event.key.keysym.sym;
        break;
      }
      case ( MENU_SETTINGS_CONTROLS::EJECT ):
      {
        JUMP = event.key.keysym.sym;
        break;
      }
      default:
        break;
    }
    defining_key = false;
    button_pressed = true;
    SDL_FlushEvent( SDL_KEYDOWN );
  }
  else if ( !keyboard_state[SDL_SCANCODE_RETURN] &&
            !keyboard_state[SDL_SCANCODE_ESCAPE] &&
            button_pressed )
    button_pressed = false;
}

void Menu::ResetKey()
{
  button_pressed = true;
  if ( current_room != ROOMS::MENU_SETTINGS_CONTROLS )
    return;

  switch ( button_selected )
  {
    case ( MENU_SETTINGS_CONTROLS::ACCELERATE ):
    {
      THROTTLE_UP = DEFAULT_THROTTLE_UP;
      break;
    }
    case ( MENU_SETTINGS_CONTROLS::DECELERATE ):
    {
      THROTTLE_DOWN = DEFAULT_THROTTLE_DOWN;
      break;
    }
    case ( MENU_SETTINGS_CONTROLS::LEFT ):
    {
      TURN_LEFT = DEFAULT_TURN_LEFT;
      break;
    }
    case ( MENU_SETTINGS_CONTROLS::RIGHT ):
    {
      TURN_RIGHT = DEFAULT_TURN_RIGHT;
      break;
    }
    case ( MENU_SETTINGS_CONTROLS::SHOOT ):
    {
      FIRE = DEFAULT_FIRE;
      break;
    }
    case ( MENU_SETTINGS_CONTROLS::EJECT ):
    {
      JUMP = DEFAULT_JUMP;
      break;
    }
    default:
      break;
  }
}

void Menu::ReturnToMainMenu()
{
  if ( !game_finished )
    stats_update();

  game_finished = false;
  game_pause = false;
//  opponent_connected = false;

  SDL_SetWindowResizable( gWindow, SDL_TRUE );
  ChangeRoom( ROOMS::MENU_MAIN );

  button_pressed = false;
}

unsigned char Menu::getSelectedButton()
{
  return button_selected;
}

bool Menu::getSpecifyingVar( unsigned char var_type )
{
  return specifying_var[var_type];
}

bool Menu::getDefiningKey()
{
  return defining_key;
}

std::string Menu::getInputIp()
{
  return inputIp;
}

std::string Menu::getInputPortHost()
{
  return inputPortHost;
}

std::string Menu::getInputPortClient()
{
  return inputPortClient;
}

std::string Menu::getInputPass()
{
  return inputPass;
}

MESSAGE_TYPE Menu::getMessage()
{
  return message;
}

// MAIN MENU
void menu_main()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();


  // Menu box
  textures.destrect = { 0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288 };
  SDL_RenderCopy( gRenderer, textures.menu_box, NULL, &textures.destrect );


  // Menu button
  draw_menu_button();


  // Menu text
  draw_text( "BLUETOOTH BIPLANES", sizes.screen_width * 0.250, sizes.screen_height * 0.2855 );
  draw_text( "One Player Game   ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Two Player Game   ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Controls          ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Help              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
  draw_text( "Quit              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 4.0 );

  draw_text( "Navigate menu with arrow keys   ", sizes.screen_width * 0.005, sizes.screen_height * 0.65 );
  draw_text( "Press[RETURN] to enter submenu  ", sizes.screen_width * 0.005, sizes.screen_height * 0.70 );
  draw_text( "  Press [F1] to see your stats  ", sizes.screen_width * 0.005, sizes.screen_height * 0.75 );
}

// SINGLEPLAYER CHOOSE OPPONENT
void menu_sp()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();


  // Menu box
  textures.destrect = { 0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288 };
  SDL_RenderCopy( gRenderer, textures.menu_box, NULL, &textures.destrect );


  // Menu button
  draw_menu_button();

  std::string aiDifficulty = "";

  switch ( aiController.getDifficulty() )
  {
    case DIFFICULTY::EASY:
    {
      aiDifficulty = "First time, huh?";
      break;
    }
    case DIFFICULTY::MEDIUM:
    {
      aiDifficulty = "Rookie";
      break;
    }
    case DIFFICULTY::HARD:
    {
      aiDifficulty = "Developer";
      break;
    }
    case DIFFICULTY::EVOLVE:
    {
      aiDifficulty = "Evolve";
      break;
    }
  }

  // Menu text
  draw_text( "CHOOSE OPPONENT   ",        sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Setup Game        ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "AI side:          ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "AI difficulty:    ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( aiDifficulty.c_str(),        sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
}

// SINGLEPLAYER SETUP GAME
void menu_sp_setup()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();

  std::string hardcore = HARDCORE_MODE == true ? "On" : "Off";

  // Menu box
  textures.destrect = { 0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288 };
  SDL_RenderCopy( gRenderer, textures.menu_box, NULL, &textures.destrect );


  // Menu button
  draw_menu_button();


  // Menu text
  draw_text( "SETUP GAME        ",        sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Start Game        ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Game length:      ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( menu.getInputPass().c_str(), sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Hardcore mode:    ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( hardcore.c_str(),            sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
}

// MULTIPLAYER SELECT
void menu_mp()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();


  // Menu box
  textures.destrect = { 0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288 };
  SDL_RenderCopy( gRenderer, textures.menu_box, NULL, &textures.destrect );


  // Menu button
  draw_menu_button();


  // Menu text
  draw_text( "TWO PLAYER GAME   ", sizes.screen_width * 0.250, sizes.screen_height * 0.2855 );
  draw_text( "Matchmaking       ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Direct Connect    ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Help              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
}

// MULTIPLAYER MMAKE
void menu_mp_mmake()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();

  std::string hardcore = HARDCORE_MODE == true ? "On" : "Off";

  // Menu box
  textures.destrect = { 0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288 };
  SDL_RenderCopy( gRenderer, textures.menu_box, NULL, &textures.destrect );


  // Menu button
  draw_menu_button();


  // Menu text
  draw_text( "MATCHMAKING       ",        sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Find Game         ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Password:         ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( menu.getInputPass().c_str(), sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Hardcore mode:    ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( hardcore.c_str(),            sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ",        sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );

  if ( menu.getSpecifyingVar( SPECIFY::PASSWORD ) )
  {
    draw_text( "Press [RETURN] to finish", sizes.screen_width * 0.25, sizes.screen_height * 0.6 );
    draw_text( " specifying password... ", sizes.screen_width * 0.25, sizes.screen_height * 0.65 );
  }
  else
  {
    switch ( menu.getSelectedButton() )
    {
      case MENU_MP_MMAKE::FIND_GAME:
      {
        draw_text( "Search for opponents", sizes.screen_width * 0.005, sizes.screen_height * 0.65 );
        break;
      }
      case MENU_MP_MMAKE::SPECIFY_PASSWORD:
      {
        draw_text( "Press[RETURN]to specify password", sizes.screen_width * 0.005, sizes.screen_height * 0.65 );
        break;
      }
      case MENU_MP_MMAKE::HARDCORE_MODE:
      {
        draw_text( "Enable one-shot kills           ", sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
        draw_text( "Both opponents  should have this", sizes.screen_width * 0.005, sizes.screen_height * 0.700 );
        draw_text( "           turned  on           ", sizes.screen_width * 0.005, sizes.screen_height * 0.750 );
        break;
      }
      default:
        break;
    }
  }
}

// MULTIPLAYER DC
void menu_mp_dc()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();


  // Menu box
  textures.destrect = { 0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288 };
  SDL_RenderCopy( gRenderer, textures.menu_box, NULL, &textures.destrect );


  // Menu button
  draw_menu_button();


  // Menu text
  draw_text( "DIRECT CONNECT    ", sizes.screen_width * 0.250, sizes.screen_height * 0.2855 );
  draw_text( "Start Network Game", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Join Network Game ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Help              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
}

// MULTIPLAYER DC HOST
void menu_mp_dc_host()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  std::string hardcore = HARDCORE_MODE == true ? "On" : "Off";

  draw_background();
  draw_barn();


  // Menu box
  textures.destrect = { 0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288 };
  SDL_RenderCopy( gRenderer, textures.menu_box, NULL, &textures.destrect );


  // Menu button
  draw_menu_button();


  // Menu text
  draw_text( "HOST TWO PLAYER GAME  ",          sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Start Two Player Game ",          sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Host Port:            ",          sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( menu.getInputPortHost().c_str(),   sizes.screen_width * 0.825, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Hardcore mode:        ",          sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( hardcore.c_str(),                  sizes.screen_width * 0.825, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back                  ",          sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );

  if ( menu.getSpecifyingVar( SPECIFY::PORT ) )
  {
    draw_text( "Press [RETURN] to finish",  sizes.screen_width * 0.250, sizes.screen_height * 0.60 );
    draw_text( "     specifying port...     ",  sizes.screen_width * 0.250, sizes.screen_height * 0.65 );
  }
  else
  {
    switch ( menu.getSelectedButton() )
    {
      case MENU_MP_DC_HOST::HOST_START:
      {
        char textbuf[30];
        sprintf( textbuf, "Start server at port %d", HOST_PORT );
        draw_text( textbuf, sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
        break;
      }
      case MENU_MP_DC_HOST::SPECIFY_PORT:
      {
        draw_text( "Press [RETURN] to specify port", sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
        break;
      }
      case MENU_MP_DC_HOST::HARDCORE_MODE:
      {
        draw_text( "Enable one-shot kills         ", sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
        break;
      }
      default:
        break;
    }
  }
}

// MULTIPLAYER DC JOIN
void menu_mp_dc_join()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();

  // Menu box
  textures.destrect = {0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288};
  SDL_RenderCopy(gRenderer, textures.menu_box, NULL, &textures.destrect);


  // Menu button
  draw_menu_button();


  // Menu text
  draw_text( "JOIN TWO PLAYER GAME",            sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Connect             ",            sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Server IP:          ",            sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( menu.getInputIp().c_str(),         sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Server Port:        ",            sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( menu.getInputPortClient().c_str(), sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back                ",            sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );

  if ( menu.getSpecifyingVar( SPECIFY::IP ) )
  {
    draw_text( "Press [RETURN] to finish",       sizes.screen_width * 0.250, sizes.screen_height * 0.600 );
    draw_text( "specifying server IP... ",       sizes.screen_width * 0.250, sizes.screen_height * 0.650 );
  }
  else if ( menu.getSpecifyingVar( SPECIFY::PORT ) )
  {
    draw_text( "Press [RETURN] to finish",       sizes.screen_width * 0.250, sizes.screen_height * 0.600 );
    draw_text( "specifying server port  ",       sizes.screen_width * 0.250, sizes.screen_height * 0.650 );
  }
  else
  {
    switch ( menu.getSelectedButton() )
    {
      case MENU_MP_DC_JOIN::JOIN:
      {
        char textbuf[33];
        sprintf( textbuf, "Connect to %s:%d", (char*) SERVER_IP.data(), SERVER_PORT );
        draw_text( textbuf,                         sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
        break;
      }
      case MENU_MP_DC_JOIN::SPECIFY_IP:
      {
        draw_text( "Press [RETURN] to specify IP",   sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
        break;
      }
      case MENU_MP_DC_JOIN::SPECIFY_PORT:
      {
        draw_text( "Press [RETURN] to specify port", sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
        break;
      }
      default:
        break;
    }
  }
}

// MULTIPLAYER DC HELP
void menu_mp_dc_help()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  char textbuf[30];
  draw_text(          "      Direct connect help:       ", 0, sizes.screen_height * 0.100 );
  draw_text(          "           For CLIENT:           ", 0, sizes.screen_height * 0.175 );
  draw_text(          "   Make sure your anti-virus     ", 0, sizes.screen_height * 0.250 );
  draw_text(          " or firewall don't block access  ", 0, sizes.screen_height * 0.300 );
  draw_text(          " to the Internet for this game.  ", 0, sizes.screen_height * 0.350 );

  draw_text(          "            For HOST:            ", 0, sizes.screen_height * 0.475 );
  draw_text(          "  Make sure your firewall and    ", 0, sizes.screen_height * 0.550 );
  draw_text(          "  router don't block incoming    ", 0, sizes.screen_height * 0.600 );

  sprintf( textbuf,   "  connections at port   %d", HOST_PORT );
  draw_text( textbuf,                                      0, sizes.screen_height * 0.650 );
  draw_text(          "  and your opponent is provided  ", 0, sizes.screen_height * 0.700 );
  draw_text(          "  with your public IP address.   ", 0, sizes.screen_height * 0.750 );
  draw_text(          "       Or you can use any        ", 0, sizes.screen_height * 0.800 );
  draw_text(          "Virtual Private Network software ", 0, sizes.screen_height * 0.850 );
  draw_text(          " (LogMeIn Hamachi, Tunngle, etc) ", 0, sizes.screen_height * 0.900 );
}

// MULTIPLAYER HELP PAGE 1
void menu_mp_help_page1()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "       Multiplayer  HELP:       ", 0, sizes.screen_height * 0.175 );
  draw_text(        "   Make sure your  anti-virus   ", 0, sizes.screen_height * 0.250 );
  draw_text(        "and firewall  don't block access", 0, sizes.screen_height * 0.300 );
  draw_text(        " to the Internet for this game. ", 0, sizes.screen_height * 0.350 );

  draw_text(        "          Matchmaking:          ", 0, sizes.screen_height * 0.475 );
  draw_text(        "  Use empty password  field to  ", 0, sizes.screen_height * 0.550 );
  draw_text(        "  automatically find an opponent", 0, sizes.screen_height * 0.600 );
  draw_text(        "     and start peer-to-peer     ", 0, sizes.screen_height * 0.650 );
  draw_text(        "         game  with him         ", 0, sizes.screen_height * 0.700 );
  draw_text(        "  If you want to play with your ", 0, sizes.screen_height * 0.750 );
  draw_text(        "     friend you can specify     ", 0, sizes.screen_height * 0.800 );
  draw_text(        "a password. The session  will be", 0, sizes.screen_height * 0.850 );
  draw_text(        " established  between first two ", 0, sizes.screen_height * 0.900 );
  draw_text(        " opponents with same  passwords ", 0, sizes.screen_height * 0.950 );
}

// MULTIPLAYER HELP PAGE 2
void menu_mp_help_page2()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "  Possible matchmaking issues:  ", 0, sizes.screen_height * 0.175 );
  draw_text(        "  Successful matchmaking isn't  ", 0, sizes.screen_height * 0.250 );
  draw_text(        "  guaranteed if  your Internet  ", 0, sizes.screen_height * 0.300 );
  draw_text(        "  provider uses  Symmetric NAT  ", 0, sizes.screen_height * 0.350 );
  draw_text(        " or unpredictable port mapping. ", 0, sizes.screen_height * 0.400 );
  draw_text(        " It's very likely that you fall ", 0, sizes.screen_height * 0.450 );
  draw_text(        "  into these conditions if you  ", 0, sizes.screen_height * 0.500 );
  draw_text(        "    access Internet through     ", 0, sizes.screen_height * 0.550 );
  draw_text(        "         mobile network.        ", 0, sizes.screen_height * 0.600 );
  draw_text(        " P2P connection timeout message ", 0, sizes.screen_height * 0.650 );
  draw_text(        " means that you tried to connect", 0, sizes.screen_height * 0.700 );
  draw_text(        " to found opponent  but failed. ", 0, sizes.screen_height * 0.750 );
  draw_text(        "    If it happens every time -  ", 0, sizes.screen_height * 0.800 );
  draw_text(        "unfortunately you  won't be able", 0, sizes.screen_height * 0.850 );
  draw_text(        "   to use matchmaking feature   ", 0, sizes.screen_height * 0.900 );
}

// MULTIPLAYER HELP PAGE 3
void menu_mp_help_page3()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Connecting to matchmake server: ", 0, sizes.screen_height * 0.250 );
  draw_text(        " Game tries to connect to our   ", 0, sizes.screen_height * 0.300 );
  draw_text(        " matchmaking server.            ", 0, sizes.screen_height * 0.350 );
  draw_text(        " Seeing this message longer than", 0, sizes.screen_height * 0.400 );
  draw_text(        " 5-10 seconds means that our    ", 0, sizes.screen_height * 0.450 );
  draw_text(        " matchmaking server is down for ", 0, sizes.screen_height * 0.500 );
  draw_text(        " some reason.                   ", 0, sizes.screen_height * 0.550 );
  draw_text(        " If the issue persists more than", 0, sizes.screen_height * 0.600 );
  draw_text(        " a day please contact us to     ", 0, sizes.screen_height * 0.650 );
  draw_text(        " resolve it.                    ", 0, sizes.screen_height * 0.700 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.750 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.800 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.850 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.900 );
}

// MULTIPLAYER HELP PAGE 4
void menu_mp_help_page4()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Bad matchmake server reply!:    ", 0, sizes.screen_height * 0.250 );
  draw_text(        " It's almost impossible to get  ", 0, sizes.screen_height * 0.300 );
  draw_text(        " this error but if you do -     ", 0, sizes.screen_height * 0.350 );
  draw_text(        " please let us know the exact   ", 0, sizes.screen_height * 0.400 );
  draw_text(        " time when you got this error   ", 0, sizes.screen_height * 0.450 );
  draw_text(        " and we will check our logs to  ", 0, sizes.screen_height * 0.500 );
  draw_text(        " see what caused it.            ", 0, sizes.screen_height * 0.550 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.600 );
  draw_text(        "Searching for opponent:         ", 0, sizes.screen_height * 0.650 );
  draw_text(        " You received an echo reply from", 0, sizes.screen_height * 0.700 );
  draw_text(        " matchmaking server and now     ", 0, sizes.screen_height * 0.750 );
  draw_text(        " waiting for opponent.          ", 0, sizes.screen_height * 0.800 );
  draw_text(        " If there are no opponents the  ", 0, sizes.screen_height * 0.850 );
  draw_text(        " game will not start obviously. ", 0, sizes.screen_height * 0.900 );
}

// MULTIPLAYER HELP PAGE 5
void menu_mp_help_page5()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Establishing p2p connection:    ", 0, sizes.screen_height * 0.250 );
  draw_text(        " You found an opponent and now  ", 0, sizes.screen_height * 0.300 );
  draw_text(        " you're trying to connect to him", 0, sizes.screen_height * 0.350 );
  draw_text(        "                                ", 0, sizes.screen_height * 0.400 );
  draw_text(        "Waiting for opponent answer:    ", 0, sizes.screen_height * 0.450 );
  draw_text(        " You performed all required     ", 0, sizes.screen_height * 0.500 );
  draw_text(        " actions and wait for opponent  ", 0, sizes.screen_height * 0.550 );
  draw_text(        " to do the same. If the game    ", 0, sizes.screen_height * 0.600 );
  draw_text(        " doesn't start in 10 seconds    ", 0, sizes.screen_height * 0.650 );
  draw_text(        " you will get following error   ", 0, sizes.screen_height * 0.700 );
  draw_text(        " described in the next page:    ", 0, sizes.screen_height * 0.750 );
  draw_text(        "Peer-to-peer connection timeout!", 0, sizes.screen_height * 0.800 );
}

// MULTIPLAYER HELP PAGE 6
void menu_mp_help_page6()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Peer-to-peer connection timeout:", 0, sizes.screen_height * 0.250 );
  draw_text(        " Something went wrong and       ", 0, sizes.screen_height * 0.300 );
  draw_text(        " connection wasn't established. ", 0, sizes.screen_height * 0.350 );
  draw_text(        " If you get this error every    ", 0, sizes.screen_height * 0.400 );
  draw_text(        " time - unfortunately that means", 0, sizes.screen_height * 0.450 );
  draw_text(        " matchmaking is not possible    ", 0, sizes.screen_height * 0.500 );
  draw_text(        " with your current Internet     ", 0, sizes.screen_height * 0.550 );
  draw_text(        " provider.                      ", 0, sizes.screen_height * 0.600 );
  draw_text(        " You can still use Direct       ", 0, sizes.screen_height * 0.650 );
  draw_text(        " connect if you or your friend  ", 0, sizes.screen_height * 0.700 );
  draw_text(        " can setup port forwarding or   ", 0, sizes.screen_height * 0.750 );
  draw_text(        " just use Virtual Private       ", 0, sizes.screen_height * 0.800 );
  draw_text(        " Network software to play with  ", 0, sizes.screen_height * 0.850 );
  draw_text(        " each other.                    ", 0, sizes.screen_height * 0.900 );
}

// MULTIPLAYER HELP PAGE 7
void menu_mp_help_page7()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Connection failed:              ", 0, sizes.screen_height * 0.250 );
  draw_text(        " In Direct Connect mode you can ", 0, sizes.screen_height * 0.300 );
  draw_text(        " see this message if your       ", 0, sizes.screen_height * 0.350 );
  draw_text(        " opponent stopped hosting or he ", 0, sizes.screen_height * 0.400 );
  draw_text(        " is unreacheable (closed port or", 0, sizes.screen_height * 0.450 );
  draw_text(        " private IP address probably)   ", 0, sizes.screen_height * 0.500 );

  draw_text(        " In Matchmaking mode this       ", 0, sizes.screen_height * 0.600 );
  draw_text(        " message appears only if your   ", 0, sizes.screen_height * 0.650 );
  draw_text(        " opponent aborted the game      ", 0, sizes.screen_height * 0.700 );
  draw_text(        " before you managed to connect. ", 0, sizes.screen_height * 0.750 );
}

// MULTIPLAYER HELP PAGE 8
void menu_mp_help_page8()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_text(        "        Status messages:        ", 0, sizes.screen_height * 0.175 );
  draw_text(        "Connection timed out:           ", 0, sizes.screen_height * 0.250 );
  draw_text(        " This message may appear if you ", 0, sizes.screen_height * 0.300 );
  draw_text(        " didn't receive packets from    ", 0, sizes.screen_height * 0.350 );
  draw_text(        " your opponent for a long time. ", 0, sizes.screen_height * 0.400 );
  draw_text(        " This may occur if his game     ", 0, sizes.screen_height * 0.450 );
  draw_text(        " suddenly crashed or he lost    ", 0, sizes.screen_height * 0.500 );
  draw_text(        " Internet connection.           ", 0, sizes.screen_height * 0.550 );
  draw_text(        "Server ceased connection or     ", 0, sizes.screen_height * 0.600 );
  draw_text(        "Client disconnected:            ", 0, sizes.screen_height * 0.650 );
  draw_text(        " Obviously your opponent just   ", 0, sizes.screen_height * 0.700 );
  draw_text(        " exited game session.           ", 0, sizes.screen_height * 0.750 );
  draw_text(        "All other messages may indicate ", 0, sizes.screen_height * 0.800 );
  draw_text(        " problems with initializing     ", 0, sizes.screen_height * 0.850 );
  draw_text(        " network systems.               ", 0, sizes.screen_height * 0.900 );
}

// HELP
void menu_help()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  textures.destrect = { 0, 0, sizes.screen_width, sizes.screen_height };
  SDL_RenderCopy( gRenderer, textures.menu_help, NULL, &textures.destrect );

  textures.destrect = { sizes.screen_width * 0.5 - sizes.plane_sizex * 0.5, sizes.screen_height * 0.2 - sizes.plane_sizey * 0.5, sizes.plane_sizex, sizes.plane_sizey };
  SDL_RenderCopyEx( gRenderer, textures.texture_biplane_b, NULL, &textures.destrect, 337.5, NULL, SDL_FLIP_NONE );


  // Accelerate
  draw_text( "Accelerate",        sizes.screen_width * 0.360, sizes.screen_height * 0.05 );
  char textbuf[30];
  sprintf( textbuf, "[%s]", SDL_GetKeyName( THROTTLE_UP ) );
  draw_text( textbuf,             sizes.screen_width * 0.400, sizes.screen_height * 0.1 );


  // Turn left
  draw_text( "Turn Anti       ",  0,                          sizes.screen_height * 0.100 );
  draw_text( "Clockwise       ",  0,                          sizes.screen_height * 0.150 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( TURN_LEFT ) );
  draw_text( textbuf,             0,                          sizes.screen_height * 0.200 );


  // Turn right
  draw_text( "Turn            ",  sizes.screen_width * 0.600, sizes.screen_height * 0.150 );
  draw_text( "Clockwise       ",  sizes.screen_width * 0.600, sizes.screen_height * 0.200 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( TURN_RIGHT ) );
  draw_text( textbuf,             sizes.screen_width * 0.600, sizes.screen_height * 0.250 );


  // Decelerate
  draw_text( "Decelerate      ",  sizes.screen_width * 0.200, sizes.screen_height * 0.250 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( THROTTLE_DOWN ) );
  draw_text( textbuf,             sizes.screen_width * 0.200, sizes.screen_height * 0.300 );


  // Fire
  draw_text( " Fire           ",  sizes.screen_width * 0.125, sizes.screen_height * 0.350 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( FIRE ) );
  draw_text( textbuf,             sizes.screen_width * 0.275, sizes.screen_height * 0.350 );


  // Jump
  draw_text( "Eject           ",  sizes.screen_width * 0.125, sizes.screen_height * 0.400 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( JUMP ) );
  draw_text( textbuf,             sizes.screen_width * 0.275, sizes.screen_height * 0.400 );


  // Move left
  draw_text( "Move            ",  sizes.screen_width * 0.025, sizes.screen_height * 0.650 );
  draw_text( "Left            ",  sizes.screen_width * 0.025, sizes.screen_height * 0.700 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( TURN_LEFT ) );
  draw_text( textbuf,             sizes.screen_width * 0.025, sizes.screen_height * 0.750 );


  // Move right
  draw_text( " Move           ",  sizes.screen_width * 0.325, sizes.screen_height * 0.650 );
  draw_text( "Right           ",  sizes.screen_width * 0.325, sizes.screen_height * 0.700 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( TURN_RIGHT ) );
  draw_text( textbuf,             sizes.screen_width * 0.325, sizes.screen_height * 0.600 );


  // Run into the barn!
  draw_text( "Run into        ",  sizes.screen_width * 0.025, sizes.screen_height * 0.850 );
  draw_text( "      the barn! ",  sizes.screen_width * 0.025, sizes.screen_height * 0.900 );


  // Fly left
  draw_text( "Move            ",  sizes.screen_width * 0.525, sizes.screen_height * 0.650 );
  draw_text( "Left            ",  sizes.screen_width * 0.525, sizes.screen_height * 0.700 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( TURN_LEFT ) );
  draw_text( textbuf,             sizes.screen_width * 0.525, sizes.screen_height * 0.750 );


  // Fly right
  draw_text( " Move           ",  sizes.screen_width * 0.825, sizes.screen_height * 0.650 );
  draw_text( "Right           ",  sizes.screen_width * 0.825, sizes.screen_height * 0.700 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( TURN_RIGHT ) );
  draw_text( textbuf,             sizes.screen_width * 0.750, sizes.screen_height * 0.600 );


  // Open Parachute
  draw_text( "Open Parachute  ",  sizes.screen_width * 0.525, sizes.screen_height * 0.850 );
  sprintf( textbuf, "[%s]", SDL_GetKeyName( JUMP ) );
  draw_text( textbuf,             sizes.screen_width * 0.525, sizes.screen_height * 0.900 );
}

// RECENT STATS
void menu_recent_stats()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  const float total_hits        = stats_recent.plane_hits + stats_recent.chute_hits + stats_recent.pilot_hits;
  const int   misses            = stats_recent.shots - total_hits;
  const float accuracy          = std::isnan( total_hits / stats_recent.shots ) ? 0.0f : total_hits * 100.0f / stats_recent.shots;

  const float suicides          = stats_recent.crashes + stats_recent.falls;
        float selfPreservation  = std::isnan( stats_recent.rescues / ( suicides + stats_recent.rescues ) ) ? 0.0f : stats_recent.rescues * 100.0f / ( suicides + stats_recent.rescues );

  const float totalKills        = stats_recent.plane_kills + stats_recent.pilot_hits;
  const float killsDeaths       = std::isnan( totalKills / ( suicides + stats_recent.deaths ) ) ? 0.0f : totalKills / ( suicides + stats_recent.deaths );
        float survivability     = std::isnan( stats_recent.rescues / ( stats_recent.deaths + suicides + stats_recent.rescues ) ) ? 0.0f : stats_recent.rescues * 100.0f / ( stats_recent.deaths + suicides + stats_recent.rescues );

  const float avgBulletsForKill  = std::isnan( stats_recent.shots / totalKills ) ? 0.0f : stats_recent.shots / totalKills;

  if ( ( totalKills > 0 || stats_recent.deaths > 0 ) && suicides + stats_recent.rescues == 0 )
    selfPreservation = 100.0f;

  if ( totalKills > 0 && stats_recent.rescues + stats_recent.deaths + suicides == 0 )
    survivability = 100.0f;

  draw_text(        "      Last dogfight stats:      ", 0, 0 );
  char textbuf[40];
  sprintf( textbuf, "  %d shots", stats_recent.shots );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.050 );
  sprintf( textbuf, "  %d plane hits", stats_recent.plane_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.100 );
  sprintf( textbuf, "  %d chute hits", stats_recent.chute_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.150 );
  sprintf( textbuf, "  %d pilot hits", stats_recent.pilot_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.200 );
  sprintf( textbuf, "  %d misses", misses );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.250 );
  sprintf( textbuf, "Your accuracy: %.2f%%", accuracy );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.300 );
  sprintf( textbuf, "Average shots per kill: %.2f", avgBulletsForKill );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.350 );

  sprintf( textbuf, "  %d jumps", stats_recent.jumps );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.450 );
  sprintf( textbuf, "  %d crashes", stats_recent.crashes );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.500 );
  sprintf( textbuf, "  %d fall deaths", stats_recent.falls );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.550 );
  sprintf( textbuf, "  %d successful jumps", stats_recent.rescues );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.600 );
  sprintf( textbuf, "Your self-preservation: %.2f%%", selfPreservation );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.650 );

  sprintf( textbuf, "  %d plane kills", stats_recent.plane_kills );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.750 );
  sprintf( textbuf, "  %d total kills", totalKills );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.800 );
  sprintf( textbuf, "  %d total KIA", stats_recent.deaths );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.850 );
  sprintf( textbuf, "Kills/Deaths: %.2f", killsDeaths );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.900 );
  sprintf( textbuf, "Survivability: %.2f%%", survivability );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.950 );
}

// TOTAL STATS PAGE 1
void menu_total_stats_page1()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  const float total_hits        = stats_total.plane_hits + stats_total.chute_hits + stats_total.pilot_hits;
  const int   misses            = stats_total.shots - total_hits;
  const float accuracy          = std::isnan( total_hits / stats_total.shots ) ? 0.0f : total_hits * 100.0f / stats_total.shots;

  const float suicides          = stats_total.crashes + stats_total.falls;
        float selfPreservation  = std::isnan( stats_total.rescues / ( suicides + stats_total.rescues ) ) ? 0.0f : stats_total.rescues * 100.0f / ( suicides + stats_total.rescues );
  const float totalKills        = stats_total.plane_kills + stats_total.pilot_hits;
  const float avgBulletsPerKill = std::isnan( stats_total.shots / totalKills ) ? 0.0f : stats_total.shots / totalKills;

  if ( ( totalKills > 0 || stats_total.deaths > 0 ) && suicides + stats_total.rescues == 0 )
    selfPreservation = 100.0f;

  draw_text(        "          Total stats:          ", 0, 0 );
  char textbuf[40];
  sprintf( textbuf, "  %d shots", stats_total.shots );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.050 );
  sprintf( textbuf, "  %d plane hits", stats_total.plane_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.100 );
  sprintf( textbuf, "  %d chute hits", stats_total.chute_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.150 );
  sprintf( textbuf, "  %d pilot hits", stats_total.pilot_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.200 );
  sprintf( textbuf, "  %d misses", misses );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.250 );
  sprintf( textbuf, "Your accuracy: %.2f%%", accuracy );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.300 );
  sprintf( textbuf, "Average shots per kill: %.2f", avgBulletsPerKill );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.350 );

  sprintf( textbuf, "  %d jumps", stats_total.jumps );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.450 );
  sprintf( textbuf, "  %d crashes", stats_total.crashes );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.500 );
  sprintf( textbuf, "  %d fall deaths", stats_total.falls );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.550 );
  sprintf( textbuf, "  %d successful jumps", stats_total.rescues );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.600 );
  sprintf( textbuf, "Your self-preservation: %.2f%%", selfPreservation );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.650 );
}

// TOTAL STATS PAGE 2
void menu_total_stats_page2()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  const float suicides          = stats_total.crashes + stats_total.falls;
  const int   totalKills        = stats_total.plane_kills + stats_total.pilot_hits;
  const float killsDeaths       = std::isnan( totalKills / ( suicides + stats_total.deaths ) ) ? 0.0f : totalKills / ( suicides + stats_total.deaths );
        float survivability     = std::isnan( stats_total.rescues / ( stats_total.deaths + suicides + stats_total.rescues ) ) ? 0.0f : stats_total.rescues * 100.0f / ( stats_total.deaths + suicides + stats_total.rescues );
  const float winLose           = std::isnan( (float) stats_total.wins / stats_total.losses ) ? 0.0f : stats_total.wins * 100.0f / stats_total.losses;

  if ( stats_total.wins > 0 && stats_total.rescues + stats_total.deaths + suicides == 0 )
    survivability = 100.0f;

  draw_text(        "          Total stats:          ", 0, 0 );
  char textbuf[40];
  sprintf( textbuf, "  %d plane kills", stats_total.plane_kills );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.100 );
  sprintf( textbuf, "  %d pilot kills", stats_total.pilot_hits );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.150 );
  sprintf( textbuf, "  %d total kills", totalKills );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.200 );
  sprintf( textbuf, "  %d total KIA", stats_total.deaths );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.250 );
  sprintf( textbuf, "Kills/Deaths: %.2f", killsDeaths );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.300 );
  sprintf( textbuf, "Survivability: %.2f%%", survivability );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.350 );

  sprintf( textbuf, "  %d wins", stats_total.wins );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.450 );
  sprintf( textbuf, "  %d losses", stats_total.losses );
  draw_text( textbuf, sizes.screen_width * 0.025, sizes.screen_height * 0.500 );
  sprintf( textbuf, "Win/Lose: %.2f%%", winLose );
  draw_text( textbuf, 0,                          sizes.screen_height * 0.550 );
}

// RESET STATS
void stats_update()
{
  if ( game_finished )
  {
    stats_total.chute_hits  += stats_recent.chute_hits;
    stats_total.crashes     += stats_recent.crashes;
    stats_total.deaths      += stats_recent.deaths;
    stats_total.falls       += stats_recent.falls;
    stats_total.jumps       += stats_recent.jumps;
    stats_total.losses      += stats_recent.losses;
    stats_total.pilot_hits  += stats_recent.pilot_hits;
    stats_total.plane_hits  += stats_recent.plane_hits;
    stats_total.plane_kills += stats_recent.plane_kills;
    stats_total.rescues     += stats_recent.rescues;
    stats_total.shots       += stats_recent.shots;
    stats_total.wins        += stats_recent.wins;
  }
  else
    stats_recent = Statistics();
}

// CONTROLS
void menu_settings_controls()
{
  if ( !game_pause )
  {
    SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
    SDL_RenderClear( gRenderer );

    draw_background();
    draw_barn();
  }


  // Menu box
  textures.destrect = { 0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288 + sizes.screen_height * 0.05775 * 3.0 };
  SDL_RenderCopy( gRenderer, textures.menu_settings_controls_box, NULL, &textures.destrect );


  // Menu button
  draw_menu_button();


  // Menu text
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

  if ( menu.getDefiningKey() )
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

// PAUSE MENU
void menu_pause()
{
  // Menu box
  textures.destrect = { 0, sizes.screen_height * 0.3, sizes.screen_width, sizes.screen_height * 0.288 };
  SDL_RenderCopy( gRenderer, textures.menu_box, NULL, &textures.destrect );


  // Menu button
  draw_menu_button();


  // Menu text
  draw_text( "GAME PAUSED       ", sizes.screen_width * 0.250, sizes.screen_height * 0.2855 );
  draw_text( "On With the Show! ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Controls          ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Help              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Bail Out!         ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
}

// COPYRIGHT SCREEN
void menu_copyright()
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

// SPLASH SCREEN
void menu_splash()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  textures.destrect = { sizes.screen_width * 0.5f - sizes.screen_height * 0.5f, 0, sizes.screen_height, sizes.screen_height };
  SDL_RenderCopy( gRenderer, textures.menu_logo, NULL, &textures.destrect );
  draw_text( "  Press [SPACE],[ESC]or[RETURN] ", 0, sizes.screen_height * 0.900f );
}

// RESIZE WINDOW
void window_resize()
{
  if ( SDL_GetWindowDisplayIndex( gWindow ) != DISPLAY_INDEX )
  {
    DISPLAY_INDEX = SDL_GetWindowDisplayIndex( gWindow );
    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode( SDL_GetWindowDisplayIndex( gWindow ), &dm );
    SCREEN_WIDTH = dm.w;
    SCREEN_HEIGHT = dm.h;
    SDL_SetWindowMinimumSize( gWindow, SCREEN_HEIGHT * 1.23 * 0.2, SCREEN_HEIGHT * 0.2 );
    SDL_SetWindowMaximumSize( gWindow, SCREEN_HEIGHT * 1.23, SCREEN_HEIGHT );
  }

  sizes.screen_height = sizes.screen_height_new;
  sizes.screen_width = sizes.screen_height * 1.23f;
  sizes.screen_width_new = sizes.screen_width;
  SDL_SetWindowSize( gWindow, sizes.screen_width, sizes.screen_height );

  init_vars();
  SDL_RenderClear( gRenderer );
}
