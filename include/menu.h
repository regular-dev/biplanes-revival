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

#include <include/structures.h>
#include <include/utility.h>

#include <map>


class Menu
{
private:
  ROOMS current_room {ROOMS::MENU_COPYRIGHT};
  std::map <ROOMS, unsigned char> buttons {};
  uint8_t button_selected {};
  bool button_pressed {true};

  bool specifying_var[3] {};
  bool typing {};
  uint8_t define_key {};
  bool defining_key {};

  std::string inputIp {};
  std::string inputPortHost {};
  std::string inputPortClient {};
  std::string inputPass {};

  MESSAGE_TYPE message {};
  Timer* connected_message_timer {new Timer(3.0f)};

public:
  Menu();
  void ResizeWindow();
  void DrawMenu();
  void setMessage( MESSAGE_TYPE );
  void ChangeRoom( ROOMS );
  void UpdateControls();
  void UpdateTyping();
  void UpdateDefiningKey();
  void AnimateButton();
  void ButtonUp();
  void ButtonDown();
  void Select();
  void GoBack();
  void ToggleTyping( unsigned char );
  void EndTyping( unsigned char );
  void ToggleDefiningKey( unsigned char );
  void UpdateDefiningKey( unsigned char );
  void ResetKey();
  void ReturnToMainMenu();

  unsigned char getSelectedButton();
  bool getSpecifyingVar( unsigned char );
  bool getDefiningKey();
  std::string getInputIp();
  std::string getInputPortHost();
  std::string getInputPortClient();
  std::string getInputPass();
  MESSAGE_TYPE getMessage();
};


void game_loop_mp();
void game_close();
void menu_main();
void menu_settings_controls();
void menu_help();
void menu_recent_stats();
void menu_total_stats_page1();
void menu_total_stats_page2();
void stats_update();

void menu_sp();
void menu_sp_setup();

void menu_mp();

void menu_mp_mmake();

void menu_mp_dc();
void menu_mp_dc_host();
void menu_mp_dc_join();

void menu_mp_dc_help();
void menu_mp_help_page1();
void menu_mp_help_page2();
void menu_mp_help_page3();
void menu_mp_help_page4();
void menu_mp_help_page5();
void menu_mp_help_page6();
void menu_mp_help_page7();
void menu_mp_help_page8();

void menu_pause();
void menu_copyright();
void menu_splash();

void window_resize();
void sendDisconnectMessage();
