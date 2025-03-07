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
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/ai_stuff.hpp>
#include <include/controls.hpp>
#include <include/variables.hpp>
#include <include/utility.hpp>


void
Menu::UpdateControls()
{
  auto& game = gameState();

#if defined(BIPLANES_STEP_DEBUGGING_ENABLED)
  game.debug.stepByStepMode = isKeyDown(SDL_SCANCODE_Q);

  if ( isKeyPressed(SDL_SCANCODE_E) == true )
      game.debug.advanceOneTick = true;
#endif


  if ( mCurrentRoom == ROOMS::GAME )
  {
    if ( isKeyPressed(SDL_SCANCODE_ESCAPE) == true )
      GoBack();

    return;
  }


  if ( mIsTyping == true )
  {
    UpdateTyping();

    if ( isKeyPressed(SDL_SCANCODE_RETURN) == true )
      Select();

    return;
  }

  else if ( mIsEditingSlider == true )
  {
    UpdateSliderEditing();

    if ( isKeyPressed(SDL_SCANCODE_RETURN) == true )
      Select();

    return;
  }

  else if ( mIsDefiningKey == true )
    return UpdateDefiningKey();

  else
  {
    auto& game = gameState();

    if (  isKeyPressed(SDL_SCANCODE_DOWN) == true ||
          isKeyPressed(SDL_SCANCODE_S) == true )
      MenuItemNext();

    else if ( isKeyPressed(SDL_SCANCODE_UP) == true ||
              isKeyPressed(SDL_SCANCODE_W) == true )
      MenuItemPrevious();

    else if ( isKeyPressed(SDL_SCANCODE_ESCAPE) == true ||
              isKeyPressed(SDL_SCANCODE_LEFT) == true ||
              isKeyPressed(SDL_SCANCODE_A) == true )
      GoBack();

    else if ( isKeyPressed(SDL_SCANCODE_DELETE) == true )
      ResetKey();

    else if ( isKeyPressed(SDL_SCANCODE_F1) == true &&
              mCurrentRoom == ROOMS::MENU_MAIN )
    {
      setMessage(MESSAGE_TYPE::NONE);
      ChangeRoom(ROOMS::MENU_RECENT_STATS_PAGE1);

      auto& stats = game.stats;
      calcDerivedStats(stats.recent[PLANE_TYPE::RED]);
      calcDerivedStats(stats.recent[PLANE_TYPE::BLUE]);
      calcDerivedStats(stats.total);
    }
  }

  if (  isKeyPressed(SDL_SCANCODE_RETURN) == true ||
        isKeyPressed(SDL_SCANCODE_RIGHT) == true ||
        isKeyPressed(SDL_SCANCODE_D) == true )
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
          LOCAL_PORT = std::stoi(mInputPortHost);
        else
        {
          LOCAL_PORT = DEFAULT_LOCAL_PORT;
          mInputPortHost = std::to_string(LOCAL_PORT);
        }

        break;
      }

      if ( checkPort(mInputPortClient) == true )
        REMOTE_PORT = std::stoi(mInputPortClient);
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

    default:
      break;
  }

  settingsWrite();
}

void
Menu::UpdateTyping()
{
  const auto maxInputFieldTextLength = constants::menu::maxInputFieldTextLength;


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
      if ( mInputIp.length() < maxInputFieldTextLength )
      {
        for ( const auto& digit : windowEvent.text.text )
          if ( std::isdigit(digit) == true || digit == '.' )
            mInputIp += digit;
          else
            break;
      }
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
      if ( inputPort.length() < 5 )
      {
        for ( const auto& digit : windowEvent.text.text )
          if ( std::isdigit(digit) == true )
            inputPort += digit;
          else
            break;
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
      if (  mInputPassword.length() < maxInputFieldTextLength &&
            checkPass( {windowEvent.text.text} ) == true )
        mInputPassword += windowEvent.text.text;
    }

    return;
  }
}

void
Menu::ToggleSliderEditing( const MENU_SPECIFY varToSpecify )
{
  if ( mIsEditingSlider == true )
    return EndSliderEditing(varToSpecify);

  mIsEditingSlider = true;
  mSpecifyingVarState[varToSpecify] = true;
}

void
Menu::EndSliderEditing(
  const MENU_SPECIFY varToSpecify )
{
  mIsEditingSlider = false;
  mSpecifyingVarState[varToSpecify] = false;


  switch (varToSpecify)
  {
    case MENU_SPECIFY::WIN_SCORE:
    {
      break;
    }

    case MENU_SPECIFY::AUDIO_VOLUME:
    {
      setSoundVolume(gameState().audioVolume / 100.f);
      break;
    }

    case MENU_SPECIFY::STEREO_DEPTH:
    {
      break;
    }

    default:
      break;
  }


  settingsWrite();
}

void
Menu::UpdateSliderEditing()
{
  uint8_t* sliderValue {};

  if ( mSpecifyingVarState[MENU_SPECIFY::WIN_SCORE] == true )
    sliderValue = &gameState().winScore;

  else if ( mSpecifyingVarState[MENU_SPECIFY::AUDIO_VOLUME] == true )
    sliderValue = &gameState().audioVolume;

  else if ( mSpecifyingVarState[MENU_SPECIFY::STEREO_DEPTH] == true )
    sliderValue = &gameState().stereoDepth;


  if ( sliderValue == nullptr )
    return;


  const uint8_t maxSliderValue = 100;


  if ( isKeyPressed(SDL_SCANCODE_LEFT) == true ||
       isKeyPressed(SDL_SCANCODE_A) == true )
  {
    if ( *sliderValue >= 10 )
      *sliderValue -= 10;
    else
      *sliderValue = 0;
  }

  else if ( isKeyPressed(SDL_SCANCODE_DOWN) == true ||
            isKeyPressed(SDL_SCANCODE_S) == true )
  {
    if ( *sliderValue > 0 )
      --*sliderValue;
  }

  else if ( isKeyPressed(SDL_SCANCODE_RIGHT) == true ||
            isKeyPressed(SDL_SCANCODE_D) == true )
  {
    if ( *sliderValue < maxSliderValue - 10 )
      *sliderValue += 10;
    else
      *sliderValue = maxSliderValue;
  }

  else if ( isKeyPressed(SDL_SCANCODE_UP) == true ||
            isKeyPressed(SDL_SCANCODE_W) == true )
  {
    if ( *sliderValue < maxSliderValue )
      ++*sliderValue;
  }
}

void
Menu::ToggleDefiningKey(
  const MENU_SETTINGS_CONTROLS actionToDefine )
{
  if ( mIsDefiningKey == true )
  {
    mIsDefiningKey = false;
    return;
  }

  mIsDefiningKey = true;
  mKeyToDefine = actionToDefine;
}

void
Menu::UpdateDefiningKey()
{
  if (  isKeyPressed(SDL_SCANCODE_ESCAPE) == true ||
        isKeyPressed(SDL_SCANCODE_RETURN) == true )
  {
    ToggleDefiningKey(mKeyToDefine);
    return;
  }

  if (  windowEvent.type == SDL_KEYDOWN &&
        windowEvent.key.repeat == 0 )
  {
    auto& playerBindings =
      mCurrentRoom == ROOMS::MENU_SETTINGS_CONTROLS_PLAYER1
        ? bindings::player1
        : bindings::player2;


    const auto newKey = windowEvent.key.keysym.scancode;

    switch (mKeyToDefine)
    {
      case MENU_SETTINGS_CONTROLS::ACCELERATE:
      {
        assignKeyBinding(playerBindings.throttleUp, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::DECELERATE:
      {
        assignKeyBinding(playerBindings.throttleDown, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::LEFT:
      {
        assignKeyBinding(playerBindings.turnLeft, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::RIGHT:
      {
        assignKeyBinding(playerBindings.turnRight, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::SHOOT:
      {
        assignKeyBinding(playerBindings.fire, newKey);
        break;
      }

      case MENU_SETTINGS_CONTROLS::EJECT:
      {
        assignKeyBinding(playerBindings.jump, newKey);
        break;
      }

      default:
        break;
    }

    mIsDefiningKey = false;
    SDL_FlushEvent(SDL_KEYDOWN);
  }
}

void
Menu::ResetKey()
{
  if ( mCurrentRoom != ROOMS::MENU_SETTINGS_CONTROLS_PLAYER1 &&
       mCurrentRoom != ROOMS::MENU_SETTINGS_CONTROLS_PLAYER2 )
    return;


  auto& playerBindings =
    mCurrentRoom == ROOMS::MENU_SETTINGS_CONTROLS_PLAYER1
      ? bindings::player1
      : bindings::player2;

  const auto defaultBindings =
    mCurrentRoom == ROOMS::MENU_SETTINGS_CONTROLS_PLAYER1
      ? bindings::defaults::player1
      : bindings::defaults::player2;


  switch (mSelectedItem)
  {
    case MENU_SETTINGS_CONTROLS::ACCELERATE:
    {
      assignKeyBinding(
        playerBindings.throttleUp,
        defaultBindings.throttleUp );

      break;
    }

    case MENU_SETTINGS_CONTROLS::DECELERATE:
    {
      assignKeyBinding(
        playerBindings.throttleDown,
        defaultBindings.throttleDown );

      break;
    }

    case MENU_SETTINGS_CONTROLS::LEFT:
    {
      assignKeyBinding(
        playerBindings.turnLeft,
        defaultBindings.turnLeft );

      break;
    }

    case MENU_SETTINGS_CONTROLS::RIGHT:
    {
      assignKeyBinding(
        playerBindings.turnRight,
        defaultBindings.turnRight );

      break;
    }

    case MENU_SETTINGS_CONTROLS::SHOOT:
    {
      assignKeyBinding(
        playerBindings.fire,
        defaultBindings.fire );

      break;
    }

    case MENU_SETTINGS_CONTROLS::EJECT:
    {
      assignKeyBinding(
        playerBindings.jump,
        defaultBindings.jump );

      break;
    }

    default:
      break;
  }
}
