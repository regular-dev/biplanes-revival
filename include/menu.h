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

#pragma once

#include <include/fwd.hpp>
#include <include/enums.hpp>
#include <include/timer.hpp>

#include <map>
#include <string>


class Menu
{
  using MENU_SPECIFY = MENU_SPECIFY::MENU_SPECIFY;
  using MENU_SETTINGS_CONTROLS = MENU_SETTINGS_CONTROLS::MENU_SETTINGS_CONTROLS;
  using DIFFICULTY = DIFFICULTY::DIFFICULTY;


  ROOMS mCurrentRoom {ROOMS::MENU_COPYRIGHT};
  std::map <ROOMS, unsigned char> mButtons {};
  uint8_t mSelectedButton {};
  bool mButtonWasPressed {true};

  bool mSpecifyingVarState[4] {};
  bool mIsTyping {};
  MENU_SETTINGS_CONTROLS mKeyToDefine {};
  bool mIsDefiningKey {};

  std::string mInputIp {};
  std::string mInputPortHost {};
  std::string mInputPortClient {};
  std::string mInputPassword {};
  std::string mInputScoreToWin {};

  MESSAGE_TYPE mCurrentMessage {};
  Timer mConnectedMessageTimer {0.0};


public:
  Menu();

  void ResizeWindow();
  void DrawMenu();
  void DrawButton();
  void setMessage( const MESSAGE_TYPE );
  void ChangeRoom( const ROOMS );
  void UpdateControls();
  void UpdateTyping();
  void UpdateDefiningKey();
  void AnimateButton();
  void ButtonUp();
  void ButtonDown();
  void Select();
  void GoBack();
  void ToggleTyping( const MENU_SPECIFY );
  void EndTyping( const MENU_SPECIFY );
  void ToggleDefiningKey( const MENU_SETTINGS_CONTROLS );
  void UpdateDefiningKey( const MENU_SETTINGS_CONTROLS );
  void ResetKey();
  void ReturnToMainMenu();

  void screen_main();
  void screen_settings();
  void screen_help();
  void screen_pause();
  void screen_copyright();
  void screen_splash();

  void screen_stats_recent();
  void screen_stats_total_page1();
  void screen_stats_total_page2();

  void screen_sp();
  void screen_sp_setup();

  void screen_mp();
  void screen_mp_mmake();

  void screen_mp_dc();
  void screen_mp_dc_host();
  void screen_mp_dc_join();

  void screen_mp_dc_help();
  void screen_mp_help_page1();
  void screen_mp_help_page2();
  void screen_mp_help_page3();
  void screen_mp_help_page4();
  void screen_mp_help_page5();
  void screen_mp_help_page6();
  void screen_mp_help_page7();
  void screen_mp_help_page8();


  bool isSpecifyingVar( const MENU_SPECIFY ) const;
  bool isDefiningKey() const;
};

extern class Menu menu;

void window_resize();
