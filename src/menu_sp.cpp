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

#include <include/menu.hpp>
#include <include/sdl.hpp>
#include <include/render.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/textures.hpp>
#include <include/utility.hpp>


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
      aiDifficulty = "Veteran";
      break;
    }

    case DIFFICULTY::DEVELOPER:
    {
      aiDifficulty = "Developer";
      break;
    }

    case DIFFICULTY::INSANE:
    {
      aiDifficulty = "Insane 0_o";
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

  if ( gameState().botDifficulty == DIFFICULTY::INSANE && !isInsaneUnlocked() &&
       gameState().gameMode != GAME_MODE::BOT_VS_BOT)
    draw_text( "First, beat up Developer!", 0.025f, 0.65f );
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

  const auto winScoreText = std::to_string(gameState().winScore);


  draw_text( "SETUP GAME    ",    0.025f, 0.2855f );
  draw_text( "Start Game    ",    0.040f, 0.2855f + 0.0721f );
  draw_text( "Score to win: ",    0.040f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( winScoreText,        0.550f, 0.2855f + 0.0721f + button::sizeY );
  draw_text( "Extra clouds: ",    0.040f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( extraClouds,         0.550f, 0.2855f + 0.0721f + button::sizeY * 2.f );
  draw_text( "One-shot kills: ",  0.040f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( oneShotKills,        0.550f, 0.2855f + 0.0721f + button::sizeY * 3.f );
  draw_text( "Alt. hitboxes: ",   0.040f, 0.2855f + 0.0721f + button::sizeY * 4.f );
  draw_text( altHitboxes,         0.550f, 0.2855f + 0.0721f + button::sizeY * 4.f );
  draw_text( "Back          ",    0.040f, 0.2855f + 0.0721f + button::sizeY * 5.f );


  if ( isSpecifyingVar(MENU_SPECIFY::WIN_SCORE) == true )
  {
    draw_text( "Press [RETURN] to finish", 0.250f, 0.725f );
    draw_text( "specifying win score... ", 0.250f, 0.775f );

    return;
  }


  switch (mSelectedItem)
  {
    case MENU_SP_SETUP::WIN_SCORE:
      draw_text( "Press [RETURN] to specify score", 0.005f, 0.725f );
      break;

    case MENU_SP_SETUP::ALT_HITBOXES:
      draw_text( "Enable alternative plane hitbox ", 0.005f, 0.725f );
      draw_text( " More challenge for experienced ", 0.005f, 0.775f );
      draw_text( "                       players  ", 0.005f, 0.825f );
      break;
  }
}
