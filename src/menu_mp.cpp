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
#include <include/render.hpp>
#include <include/game_state.hpp>
#include <include/sizes.hpp>
#include <include/textures.hpp>
#include <include/variables.hpp>


void
Menu::screen_mp()
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
    sizes.screen_height * 0.288,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_box,
    nullptr,
    &menuRect );


  DrawButton();


  draw_text( "TWO PLAYER GAME   ", sizes.screen_width * 0.250, sizes.screen_height * 0.2855 );
  draw_text( "Matchmaking       ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Direct Connect    ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Help              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
}

void
Menu::screen_mp_mmake()
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
    sizes.screen_height * 0.288,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_box,
    nullptr,
    &menuRect );


  DrawButton();


  const std::string hardcore =
    gameState().isHardcoreEnabled == true
    ? "On" : "Off";

  draw_text( "MATCHMAKING       ",   sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Find Game         ",   sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Password:         ",   sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( mInputPassword.c_str(), sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Hardcore mode:    ",   sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( hardcore.c_str(),       sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ",   sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );

  if ( isSpecifyingVar(MENU_SPECIFY::PASSWORD) == true )
  {
    draw_text( "Press [RETURN] to finish", sizes.screen_width * 0.25, sizes.screen_height * 0.6 );
    draw_text( " specifying password... ", sizes.screen_width * 0.25, sizes.screen_height * 0.65 );

    return;
  }

  switch (mSelectedButton)
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

void
Menu::screen_mp_dc()
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
    sizes.screen_height * 0.288,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_box,
    nullptr,
    &menuRect );


  DrawButton();


  draw_text( "DIRECT CONNECT    ", sizes.screen_width * 0.250, sizes.screen_height * 0.2855 );
  draw_text( "Start Network Game", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Join Network Game ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Help              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ", sizes.screen_width * 0.255, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
}

void
Menu::screen_mp_dc_host()
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
    sizes.screen_height * 0.288,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_box,
    nullptr,
    &menuRect );


  DrawButton();


  const std::string hardcore =
    gameState().isHardcoreEnabled == true
    ? "On" : "Off";

  draw_text( "HOST TWO PLAYER GAME  ", sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Start Two Player Game ", sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Host Port:            ", sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( mInputPortHost.c_str(),   sizes.screen_width * 0.825, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Hardcore mode:        ", sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( hardcore.c_str(),         sizes.screen_width * 0.825, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back                  ", sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );


  if ( isSpecifyingVar(MENU_SPECIFY::PORT) == true )
  {
    draw_text( "Press [RETURN] to finish",  sizes.screen_width * 0.250, sizes.screen_height * 0.60 );
    draw_text( "     specifying port...     ",  sizes.screen_width * 0.250, sizes.screen_height * 0.65 );

    return;
  }

  switch (mSelectedButton)
  {
    case MENU_MP_DC_HOST::HOST_START:
    {
      char textbuf[30];
      sprintf( textbuf, "Start server at port %d", LOCAL_PORT );
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

void
Menu::screen_mp_dc_join()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();


  const SDL_Rect menuRect
  {
    0,
    sizes.screen_height * 0.3,
    sizes.screen_width,
    sizes.screen_height * 0.288,
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_box,
    nullptr,
    &menuRect );


  DrawButton();


  draw_text( "JOIN TWO PLAYER GAME",   sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Connect             ",   sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Server IP:          ",   sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( mInputIp.c_str(),         sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Server Port:        ",   sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( mInputPortClient.c_str(), sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back                ",   sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );


  if ( isSpecifyingVar(MENU_SPECIFY::IP) == true )
  {
    draw_text( "Press [RETURN] to finish",       sizes.screen_width * 0.250, sizes.screen_height * 0.600 );
    draw_text( "specifying server IP... ",       sizes.screen_width * 0.250, sizes.screen_height * 0.650 );

    return;
  }

  else if ( isSpecifyingVar(MENU_SPECIFY::PORT) == true )
  {
    draw_text( "Press [RETURN] to finish",       sizes.screen_width * 0.250, sizes.screen_height * 0.600 );
    draw_text( "specifying server port  ",       sizes.screen_width * 0.250, sizes.screen_height * 0.650 );

    return;
  }


  switch (mSelectedButton)
  {
    case MENU_MP_DC_JOIN::JOIN:
    {
      char textbuf[33];
      sprintf( textbuf, "Connect to %s:%d", (char*) SERVER_IP.data(), REMOTE_PORT );
      draw_text( textbuf, sizes.screen_width * 0.005, sizes.screen_height * 0.650 );

      break;
    }

    case MENU_MP_DC_JOIN::SPECIFY_IP:
    {
      draw_text( "Press [RETURN] to specify IP",
        sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
      break;
    }

    case MENU_MP_DC_JOIN::SPECIFY_PORT:
    {
      draw_text( "Press [RETURN] to specify port",
        sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
      break;
    }

    default:
      break;
  }
}
