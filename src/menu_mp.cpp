/*
  Biplanes Revival
  Copyright (C) 2019-2024 Regular-dev community
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
#include <include/render.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>


void
Menu::screen_mp()
{
  namespace button = constants::menu::button;


  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  draw_background();
  draw_barn();

  DrawMenuRect();
  DrawButton();

  draw_text( "TWO PLAYER GAME", 0.250f, 0.2855f );
  draw_text( "Matchmaking    ", 0.255f, 0.2855f + 0.0721f );
  draw_text( "Direct Connect ", 0.255f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Help           ", 0.255f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "Back           ", 0.255f, 0.2855f + 0.0721f + button::sizeY * 3.f );
}

void
Menu::screen_mp_mmake()
{
  namespace button = constants::menu::button;


  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  draw_background();
  draw_barn();

  DrawMenuRect();
  DrawButton();


  const auto& features = gameState().featuresLocal;

  const std::string extraClouds =
    features.extraClouds == true
    ? "On" : "Off";

  const std::string oneShotKills =
    features.oneShotKills == true
    ? "On" : "Off";

  const std::string altHitboxes =
    features.alternativeHitboxes == true
    ? "On" : "Off";


  draw_text( "MATCHMAKING   ",    0.025f, 0.2855f );
  draw_text( "Find Game     ",    0.040f, 0.2855f + 0.0721f );
  draw_text( "Password:     ",    0.040f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( mInputPassword,      0.500f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Extra clouds: ",    0.040f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( extraClouds,         0.500f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "One-shot kills: ",  0.040f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( oneShotKills,        0.500f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( "Alt. hitboxes: ",   0.040f, 0.2855f + 0.0721f + button::sizeY * 4.f );
  draw_text( altHitboxes,         0.500f, 0.2855f + 0.0721f + button::sizeY * 4.f );
  draw_text( "Back          ",    0.040f, 0.2855f + 0.0721f + button::sizeY * 5.f );


  if ( isSpecifyingVar(MENU_SPECIFY::PASSWORD) == true )
  {
    draw_text( "Press [RETURN] to finish", 0.25f, 0.6f );
    draw_text( " specifying password... ", 0.25f, 0.65f );

    return;
  }


  switch (mSelectedItem)
  {
    case MENU_MP_MMAKE::FIND_GAME:
    {
      draw_text( "Search for opponents", 0.005f, 0.725f );
      break;
    }

    case MENU_MP_MMAKE::SPECIFY_PASSWORD:
    {
      draw_text( "Press[RETURN]to specify password", 0.005f, 0.725f );
      break;
    }

    case MENU_MP_MMAKE::EXTRA_CLOUDS:
    {
      draw_text( "Enable extra clouds             ", 0.005f, 0.725f );
      draw_text( "Both opponents  should have this", 0.005f, 0.775f );
      draw_text( "           turned  on           ", 0.005f, 0.825f );

      break;
    }

    case MENU_MP_MMAKE::ONESHOT_KILLS:
    {
      draw_text( "Enable one-shot kills           ", 0.005f, 0.725f );
      draw_text( "Both opponents  should have this", 0.005f, 0.775f );
      draw_text( "           turned  on           ", 0.005f, 0.825f );

      break;
    }

    case MENU_MP_MMAKE::ALT_HITBOXES:
    {
      draw_text( "Enable alternative plane hitbox ", 0.005f, 0.725f );
      draw_text( "Both opponents  should have this", 0.005f, 0.775f );
      draw_text( "           turned  on           ", 0.005f, 0.825f );

      break;
    }

    default:
      break;
  }
}

void
Menu::screen_mp_dc()
{
  namespace button = constants::menu::button;


  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  draw_background();
  draw_barn();

  DrawMenuRect();
  DrawButton();

  draw_text( "DIRECT CONNECT    ", 0.250f, 0.2855f );
  draw_text( "Start Network Game", 0.255f, 0.2855f + 0.0721f );
  draw_text( "Join Network Game ", 0.255f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Help              ", 0.255f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "Back              ", 0.255f, 0.2855f + 0.0721f + button::sizeY * 3.f );
}

void
Menu::screen_mp_dc_host()
{
  namespace button = constants::menu::button;


  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  draw_background();
  draw_barn();

  DrawMenuRect();
  DrawButton();


  const auto& features = gameState().featuresLocal;

  const std::string extraClouds =
    features.extraClouds == true
    ? "On" : "Off";

  const std::string oneShotKills =
    features.oneShotKills == true
    ? "On" : "Off";

  const std::string altHitboxes =
    features.alternativeHitboxes == true
    ? "On" : "Off";

  draw_text( "HOST TWO PLAYER GAME ", 0.025f, 0.2855f );
  draw_text( "Start Two Player Game", 0.040f, 0.2855f + 0.0721f );
  draw_text( "Host Port:           ", 0.040f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( mInputPortHost,          0.825f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Extra clouds: ",    0.040f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( extraClouds,         0.825f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "One-shot kills: ",  0.040f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( oneShotKills,        0.825f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( "Alt. hitboxes: ",   0.040f, 0.2855f + 0.0721f + button::sizeY * 4.f );
  draw_text( altHitboxes,         0.825f, 0.2855f + 0.0721f + button::sizeY * 4.f );
  draw_text( "Back          ",    0.040f, 0.2855f + 0.0721f + button::sizeY * 5.f );


  if ( isSpecifyingVar(MENU_SPECIFY::PORT) == true )
  {
    draw_text( "Press [RETURN] to finish    ",  0.250f, 0.60f );
    draw_text( "     specifying port...     ",  0.250f, 0.65f );

    return;
  }

  switch (mSelectedItem)
  {
    case MENU_MP_DC_HOST::HOST_START:
    {
      draw_text( "Start server at port " + std::to_string(LOCAL_PORT),
        0.005f, 0.725f );

      break;
    }

    case MENU_MP_DC_HOST::SPECIFY_PORT:
    {
      draw_text( "Press [RETURN] to specify port",
        0.005f, 0.725f );
      break;
    }

    case MENU_MP_MMAKE::EXTRA_CLOUDS:
    {
      draw_text( "Enable extra clouds             ", 0.005f, 0.725f );
      break;
    }

    case MENU_MP_MMAKE::ONESHOT_KILLS:
    {
      draw_text( "Enable one-shot kills           ", 0.005f, 0.725f );
      break;
    }

    case MENU_MP_MMAKE::ALT_HITBOXES:
    {
      draw_text( "Enable alternative plane hitbox ", 0.005f, 0.725f );
      draw_text( "       More challenge for       ", 0.005f, 0.775f );
      draw_text( "           experienced players  ", 0.005f, 0.825f );
      break;
    }

    default:
      break;
  }
}

void
Menu::screen_mp_dc_join()
{
  namespace button = constants::menu::button;


  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();

  DrawMenuRect();
  DrawButton();

  draw_text( "JOIN TWO PLAYER GAME",  0.025f, 0.2855f );
  draw_text( "Connect             ",  0.040f, 0.2855f + 0.0721f );
  draw_text( "Server IP:          ",  0.040f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( mInputIp,                0.500f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Server Port:        ",  0.040f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( mInputPortClient,        0.500f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "Back                ",  0.040f, 0.2855f + 0.0721f + button::sizeY * 3.f );


  if ( isSpecifyingVar(MENU_SPECIFY::IP) == true )
  {
    draw_text( "Press [RETURN] to finish", 0.250f, 0.600f );
    draw_text( "specifying server IP... ", 0.250f, 0.650f );

    return;
  }

  else if ( isSpecifyingVar(MENU_SPECIFY::PORT) == true )
  {
    draw_text( "Press [RETURN] to finish", 0.250f, 0.600f );
    draw_text( "specifying server port  ", 0.250f, 0.650f );

    return;
  }


  switch (mSelectedItem)
  {
    case MENU_MP_DC_JOIN::JOIN:
    {
      const auto text =
        "Connect to " + SERVER_IP + ":" +
        std::to_string(REMOTE_PORT);

      draw_text( text, 0.005f, 0.650f );

      break;
    }

    case MENU_MP_DC_JOIN::SPECIFY_IP:
    {
      draw_text( "Press [RETURN] to specify IP", 0.005f, 0.650f );

      break;
    }

    case MENU_MP_DC_JOIN::SPECIFY_PORT:
    {
      draw_text( "Press [RETURN] to specify port", 0.005f, 0.650f );

      break;
    }

    default:
      break;
  }
}
