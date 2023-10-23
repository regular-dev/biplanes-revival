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


void
Menu::screen_sp()
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
    sizes.screen_height * 0.288
  };

  SDL_RenderCopy(
    gRenderer,
    textures.menu_box,
    nullptr,
    &menuRect );


  DrawButton();


  const auto& game = gameState();

  const std::string gameMode =
    game.gameMode == GAME_MODE::HUMAN_VS_BOT
    ? "Human vs Bot" : "Bot vs Bot";

  std::string aiDifficulty {};


  switch (game.botDifficulty)
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


  draw_text( "CHOOSE OPPONENT   ", sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Setup Game        ", sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Game mode:        ", sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( gameMode.c_str(),     sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "AI difficulty:    ", sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( aiDifficulty.c_str(), sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ", sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );
}

void
Menu::screen_sp_setup()
{
  SDL_SetRenderDrawColor( gRenderer, 0, 154, 239, 255 );
  SDL_RenderClear(gRenderer);

  draw_background();
  draw_barn();


  const std::string hardcore =
    gameState().isHardcoreEnabled == true
    ? "On" : "Off";


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


  draw_text( "SETUP GAME        ",     sizes.screen_width * 0.025, sizes.screen_height * 0.2855 );
  draw_text( "Start Game        ",     sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 );
  draw_text( "Score to win:     ",     sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( mInputScoreToWin.c_str(), sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey );
  draw_text( "Hardcore mode:    ",     sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( hardcore.c_str(),         sizes.screen_width * 0.500, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 2.0 );
  draw_text( "Back              ",     sizes.screen_width * 0.040, sizes.screen_height * 0.2855 + sizes.screen_height * 0.0721 + sizes.button_sizey * 3.0 );


  if ( isSpecifyingVar(MENU_SPECIFY::WIN_SCORE) == true )
  {
    draw_text( "Press [RETURN] to finish", sizes.screen_width * 0.250, sizes.screen_height * 0.600 );
    draw_text( "specifying win score... ", sizes.screen_width * 0.250, sizes.screen_height * 0.650 );

    return;
  }


  if ( mSelectedButton == MENU_SP_SETUP::WIN_SCORE )
  {
    draw_text( "Press [RETURN] to specify win score",
      sizes.screen_width * 0.005, sizes.screen_height * 0.650 );
  }
}
