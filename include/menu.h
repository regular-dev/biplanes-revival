#ifndef MENU_H
#define MENU_H

#include <map>

#include "structures.h"
#include "utility.h"


class Menu
{
private:
  ROOMS current_room;
  std::map <ROOMS, unsigned char> buttons;
  unsigned char button_selected;
  bool button_pressed;

  bool specifying_var[3];
  bool typing;
  unsigned char define_key;
  bool defining_key;

  std::string inputIp;
  std::string inputPortHost;
  std::string inputPortClient;
  std::string inputPass;

  MESSAGE_TYPE message;
  Timer *connected_message_timer;

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


void game_loop();
void game_close();
void menu_main();
void menu_settings_controls();
void menu_help();

void menu_mp();
void menu_mp_help();

void menu_mp_mmake();
void menu_mp_mmake_find_game();
void menu_mp_mmake_help();

void menu_mp_dc();
void menu_mp_dc_host();
void menu_mp_dc_join();
void menu_mp_dc_help();

void menu_pause();
void menu_copyright();
void menu_splash();

void window_resize();
void sendDisconnectMessage();

#endif // MENU_H
