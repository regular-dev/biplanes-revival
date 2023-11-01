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
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/controls.hpp>
#include <include/variables.hpp>
#include <include/utility.hpp>


void
Menu::UpdateControls()
{
  const Uint8* keyboardState = SDL_GetKeyboardState({});

  if (  keyboardState[SDL_SCANCODE_RETURN] == 0 &&
        keyboardState[SDL_SCANCODE_DOWN] == 0 &&
        keyboardState[SDL_SCANCODE_UP] == 0 &&
        keyboardState[SDL_SCANCODE_SPACE] == 0 &&
        keyboardState[SDL_SCANCODE_ESCAPE] == 0 &&
        keyboardState[SDL_SCANCODE_F1] == 0 &&
        mButtonWasPressed == true )
    mButtonWasPressed = false;


  if ( mCurrentRoom == ROOMS::GAME )
  {
    if ( keyboardState[SDL_SCANCODE_ESCAPE] == 1 && mButtonWasPressed == false )
      GoBack();

    else if ( keyboardState[SDL_SCANCODE_ESCAPE] == 0 && mButtonWasPressed == true )
      mButtonWasPressed = false;

    return;
  }

  if ( mIsTyping == true )
    UpdateTyping();

  else if ( mIsDefiningKey == true )
    UpdateDefiningKey();

  else
  {
    auto& game = gameState();

    if ( mButtonWasPressed == false )
    {
      if ( keyboardState[SDL_SCANCODE_DOWN] == 1 )
        ButtonDown();

      else if ( keyboardState[SDL_SCANCODE_UP] == 1 )
        ButtonUp();

      else if ( keyboardState[SDL_SCANCODE_ESCAPE] == 1 )
        GoBack();

      else if ( keyboardState[SDL_SCANCODE_DELETE] == 1 )
        ResetKey();

      else if ( keyboardState[SDL_SCANCODE_SPACE] == 1 )
        GoBack();

      else if ( keyboardState[SDL_SCANCODE_F1] == 1 &&
                mCurrentRoom == ROOMS::MENU_MAIN )
      {
        setMessage(MESSAGE_TYPE::NONE);
        ChangeRoom(ROOMS::MENU_RECENT_STATS);

        auto& stats = game.stats;
        calcDerivedStats(stats.recent[PLANE_TYPE::RED]);
        calcDerivedStats(stats.recent[PLANE_TYPE::BLUE]);
        calcDerivedStats(stats.total);
      }
    }

    if ( windowEvent.type == SDL_QUIT && game.isPaused == false )
      game.isExiting = true;
  }

  if ( keyboardState[SDL_SCANCODE_RETURN] == 1 && mButtonWasPressed == false )
    Select();
}

void
Menu::ToggleTyping(
  const MENU_SPECIFY varToSpecify )
{
  if ( mIsTyping == true )
    return EndTyping(varToSpecify);

  mIsTyping = true;
  mSpecifyingVarState[varToSpecify] = true;
  SDL_StartTextInput();
}

void
Menu::EndTyping(
  const MENU_SPECIFY varToSpecify )
{
  mIsTyping = false;
  mSpecifyingVarState[varToSpecify] = false;
  SDL_StopTextInput();

  switch (varToSpecify)
  {
    case MENU_SPECIFY::IP:
    {
      SERVER_IP = checkIp(mInputIp);

      if ( SERVER_IP.empty() == true )
        SERVER_IP = DEFAULT_SERVER_IP;

      mInputIp = SERVER_IP;

      break;
    }

    case MENU_SPECIFY::PORT:
    {
      if ( mCurrentRoom == ROOMS::MENU_MP_DC_HOST )
      {
        if ( checkPort(mInputPortHost) == true )
          LOCAL_PORT = stoi(mInputPortHost);
        else
        {
          LOCAL_PORT = DEFAULT_LOCAL_PORT;
          mInputPortHost = std::to_string(LOCAL_PORT);
        }

        break;
      }

      if ( checkPort(mInputPortClient) == true )
        REMOTE_PORT = stoi(mInputPortClient);
      else
      {
        REMOTE_PORT = DEFAULT_REMOTE_PORT;
        mInputPortClient = std::to_string(REMOTE_PORT);
      }

      break;
    }

    case MENU_SPECIFY::PASSWORD:
    {
      if ( checkPass(mInputPassword) == true )
        MMAKE_PASSWORD = mInputPassword;
      else
        MMAKE_PASSWORD = {};

      break;
    }

    case MENU_SPECIFY::WIN_SCORE:
    {
      const auto defaultWinScore = constants::defaultWinScore;

      if ( mInputScoreToWin.empty() == true )
        mInputScoreToWin = defaultWinScore;


      if ( checkScoreToWin(mInputScoreToWin) == true )
        gameState().winScore = stoi(mInputScoreToWin);
      else
      {
        gameState().winScore = defaultWinScore;
        mInputScoreToWin = std::to_string(defaultWinScore);
      }

      break;
    }

    default:
      break;
  }

  settingsWrite();
}

void
Menu::UpdateTyping()
{
  SDL_StartTextInput();

  if ( mSpecifyingVarState[MENU_SPECIFY::IP] == true )
  {
    if ( windowEvent.type == SDL_KEYDOWN )
    {
      if ( windowEvent.key.keysym.sym == SDLK_BACKSPACE && mInputIp.length() > 0 )
        mInputIp.pop_back();

      else if ( windowEvent.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
        SDL_SetClipboardText( mInputIp.c_str() );

      else if ( windowEvent.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
        if ( checkIp( SDL_GetClipboardText() ).empty() == false )
          mInputIp = SDL_GetClipboardText();
    }

    if ( windowEvent.type == SDL_TEXTINPUT )
    {
      if ( (  ( windowEvent.text.text[0] == 'c' ||
                windowEvent.text.text[0] == 'C') &&
              ( windowEvent.text.text[0] == 'v' ||
                windowEvent.text.text[0] == 'V' ) &&
              SDL_GetModState() & KMOD_CTRL ) == false )
      {
        if ( mInputIp.length() < 15 )
          mInputIp += windowEvent.text.text;
      }

      SDL_StopTextInput();
    }

    return;
  }

  if ( mSpecifyingVarState[MENU_SPECIFY::PORT] == true )
  {
    std::string inputPort {};

    if ( mCurrentRoom == ROOMS::MENU_MP_DC_HOST )
      inputPort = mInputPortHost;
    else
      inputPort = mInputPortClient;

    if ( windowEvent.type == SDL_KEYDOWN )
    {
      if ( windowEvent.key.keysym.sym == SDLK_BACKSPACE && inputPort.length() > 0 )
        inputPort.pop_back();

      else if ( windowEvent.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
        SDL_SetClipboardText(inputPort.c_str());

      else if ( windowEvent.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
        if ( checkPort( SDL_GetClipboardText() ) == true )
          inputPort = SDL_GetClipboardText();
    }

    else if ( windowEvent.type == SDL_TEXTINPUT )
    {
      if ( (  ( windowEvent.text.text[0] == 'c' ||
                windowEvent.text.text[0] == 'C') &&
              ( windowEvent.text.text[0] == 'v' ||
                windowEvent.text.text[0] == 'V' ) &&
              SDL_GetModState() & KMOD_CTRL ) == false )
      {
        if ( inputPort.length() < 5 )
          inputPort += windowEvent.text.text;
      }
    }

    if ( mCurrentRoom == ROOMS::MENU_MP_DC_HOST )
      mInputPortHost = inputPort;
    else
      mInputPortClient = inputPort;

    return;
  }

  if ( mSpecifyingVarState[MENU_SPECIFY::PASSWORD] == true )
  {
    if ( windowEvent.type == SDL_KEYDOWN )
    {
      if ( windowEvent.key.keysym.sym == SDLK_BACKSPACE && mInputPassword.length() > 0 )
        mInputPassword.pop_back();

      else if ( windowEvent.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
        SDL_SetClipboardText(mInputPassword.c_str());

      else if ( windowEvent.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
        if ( checkPass( SDL_GetClipboardText() ) )
          mInputPassword = SDL_GetClipboardText();
    }

    else if ( windowEvent.type == SDL_TEXTINPUT )
    {
      if ( (  ( windowEvent.text.text[0] == 'c' ||
                windowEvent.text.text[0] == 'C') &&
              ( windowEvent.text.text[0] == 'v' ||
                windowEvent.text.text[0] == 'V' ) &&
              SDL_GetModState() & KMOD_CTRL ) == false )
      {
        if (  mInputPassword.length() < 15 &&
              checkPass( {windowEvent.text.text} ) == false )
          mInputPassword += windowEvent.text.text;
      }
    }
  }

  if ( mSpecifyingVarState[MENU_SPECIFY::WIN_SCORE] == true )
  {
    if ( windowEvent.type == SDL_KEYDOWN )
    {
      if ( windowEvent.key.keysym.sym == SDLK_BACKSPACE && mInputScoreToWin.length() > 0 )
        mInputScoreToWin.pop_back();

      else if ( windowEvent.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
        SDL_SetClipboardText(mInputScoreToWin.c_str());

      else if ( windowEvent.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
        if ( checkPort( SDL_GetClipboardText() ) == true )
          mInputScoreToWin = SDL_GetClipboardText();
    }

    else if ( windowEvent.type == SDL_TEXTINPUT )
    {
      if ( (  ( windowEvent.text.text[0] == 'c' ||
                windowEvent.text.text[0] == 'C') &&
              ( windowEvent.text.text[0] == 'v' ||
                windowEvent.text.text[0] == 'V' ) &&
              SDL_GetModState() & KMOD_CTRL ) == false )
      {
        if ( mInputScoreToWin.length() < 3 )
          mInputScoreToWin += windowEvent.text.text;
      }
    }

    return;
  }
}

void
Menu::ToggleDefiningKey(
  const MENU_SETTINGS_CONTROLS action_to_define )
{
  if ( mIsDefiningKey == true )
  {
    mIsDefiningKey = false;
    return;
  }

  mIsDefiningKey = true;
  mKeyToDefine = action_to_define;
}

void
Menu::UpdateDefiningKey()
{
  const Uint8* keyboardState = SDL_GetKeyboardState({});

  if (  keyboardState[SDL_SCANCODE_ESCAPE] == 1 ||
        ( keyboardState[SDL_SCANCODE_RETURN] == 1 &&
          mButtonWasPressed == false ) )
  {
    mButtonWasPressed = true;
    ToggleDefiningKey(mKeyToDefine);
    return;
  }

  if (  windowEvent.type == SDL_KEYDOWN &&
        mButtonWasPressed == false )
  {
    const auto newKey = windowEvent.key.keysym.sym;

    switch (mKeyToDefine)
    {
      case MENU_SETTINGS_CONTROLS::ACCELERATE:
      {
        assignKeyBinding(THROTTLE_UP, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::DECELERATE:
      {
        assignKeyBinding(THROTTLE_DOWN, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::LEFT:
      {
        assignKeyBinding(TURN_LEFT, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::RIGHT:
      {
        assignKeyBinding(TURN_RIGHT, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::SHOOT:
      {
        assignKeyBinding(FIRE, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::EJECT:
      {
        assignKeyBinding(JUMP, newKey);
        break;
      }

      default:
        break;
    }

    mIsDefiningKey = false;
    mButtonWasPressed = true;
    SDL_FlushEvent(SDL_KEYDOWN);

    return;
  }

  if (  keyboardState[SDL_SCANCODE_RETURN] == 0 &&
        keyboardState[SDL_SCANCODE_ESCAPE] == 0 &&
        mButtonWasPressed == true )
    mButtonWasPressed = false;
}

void
Menu::ResetKey()
{
  mButtonWasPressed = true;

  if ( mCurrentRoom != ROOMS::MENU_SETTINGS_CONTROLS )
    return;


  switch (mSelectedButton)
  {
    case MENU_SETTINGS_CONTROLS::ACCELERATE:
    {
      THROTTLE_UP = DEFAULT_THROTTLE_UP;
      break;
    }

    case MENU_SETTINGS_CONTROLS::DECELERATE:
    {
      THROTTLE_DOWN = DEFAULT_THROTTLE_DOWN;
      break;
    }

    case MENU_SETTINGS_CONTROLS::LEFT:
    {
      TURN_LEFT = DEFAULT_TURN_LEFT;
      break;
    }

    case MENU_SETTINGS_CONTROLS::RIGHT:
    {
      TURN_RIGHT = DEFAULT_TURN_RIGHT;
      break;
    }

    case MENU_SETTINGS_CONTROLS::SHOOT:
    {
      FIRE = DEFAULT_FIRE;
      break;
    }

    case MENU_SETTINGS_CONTROLS::EJECT:
    {
      JUMP = DEFAULT_JUMP;
      break;
    }

    default:
      break;
  }
}
