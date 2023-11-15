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
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/textures.hpp>


void
Menu::screen_sp()
{
  namespace button = constants::menu::button;


  setRenderColor(constants::colors::background);
  SDL_RenderClear( gRenderer );

  draw_background();
  draw_barn();

  DrawMenuRect();
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


  draw_text( "CHOOSE OPPONENT", 0.025f, 0.2855f );
  draw_text( "Setup Game     ", 0.040f, 0.2855f + 0.0721f );
  draw_text( "Game mode:     ", 0.040f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( gameMode,          0.500f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "AI difficulty: ", 0.040f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( aiDifficulty,      0.500f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "Back           ", 0.040f, 0.2855f + 0.0721f + button::sizeY * 3.f );
}

void
Menu::screen_sp_setup()
{
  namespace button = constants::menu::button;


  setRenderColor(constants::colors::background);
  SDL_RenderClear(gRenderer);

  draw_background();
  draw_barn();

  DrawMenuRect();
  DrawButton();


  const std::string hardcore =
    gameState().isHardcoreEnabled == true
    ? "On" : "Off";

  draw_text( "SETUP GAME    ",  0.025f, 0.2855f );
  draw_text( "Start Game    ",  0.040f, 0.2855f + 0.0721f );
  draw_text( "Score to win: ",  0.040f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( mInputScoreToWin,  0.500f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Hardcore mode:",  0.040f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( hardcore,          0.500f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "Back          ",  0.040f, 0.2855f + 0.0721f + button::sizeY * 3.f );


  if ( isSpecifyingVar(MENU_SPECIFY::WIN_SCORE) == true )
  {
    draw_text( "Press [RETURN] to finish", 0.250f, 0.600f );
    draw_text( "specifying win score... ", 0.250f, 0.650f );

    return;
  }


  if ( mSelectedItem == MENU_SP_SETUP::WIN_SCORE )
    draw_text( "Press [RETURN] to specify win score", 0.005f, 0.650f );
}
