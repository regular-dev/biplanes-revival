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

#include <include/utility.hpp>
#include <include/sdl.hpp>
#include <include/game_state.hpp>
#include <include/controls.hpp>
#include <include/stats.hpp>
#include <include/variables.hpp>

#include <lib/picojson.h>

#include <fstream>
#include <sstream>
#include <iostream>

#define CONFIG_FILENAME BIPLANES_EXE_NAME ".conf"
#define STATS_FILENAME BIPLANES_EXE_NAME ".stats"
#define LOG_FILENAME BIPLANES_EXE_NAME ".log"


void
settingsWrite()
{
  const auto& game = gameState();

  std::ofstream settings {CONFIG_FILENAME, std::ios::out};

  if ( settings.is_open() == false )
  {
    log_message( "LOG: Can't write to " CONFIG_FILENAME "!" );
    log_message( "User key bindings will not be saved!\n\n" );
    show_warning( "Warning!", "Can't write to " CONFIG_FILENAME "!\nCustom key bindings will not be saved!" );

    return;
  }

  picojson::object jsonAutoFill;
  jsonAutoFill["HARDCORE_MODE"]   = picojson::value( game.isHardcoreEnabled );
  jsonAutoFill["LOCAL_PORT"]      = picojson::value( (double) LOCAL_PORT );
  jsonAutoFill["REMOTE_PORT"]     = picojson::value( (double) REMOTE_PORT );
  jsonAutoFill["SERVER_IP"]       = picojson::value( SERVER_IP );
  jsonAutoFill["MMAKE_PASSWORD"]  = picojson::value( MMAKE_PASSWORD );

  picojson::object jsonConfig;
  jsonConfig["AutoSkipIntro"]     = picojson::value( game.autoSkipIntro );
  jsonConfig["EnableAudio"]       = picojson::value( game.isAudioEnabled );
  jsonConfig["EnableVSync"]       = picojson::value( game.isVSyncEnabled );
  jsonConfig["AudioVolume"]       = picojson::value( game.audioVolume );
  jsonConfig["StereoDepth"]       = picojson::value( game.stereoDepth );

  picojson::object jsonControls;
  jsonControls["FIRE"]            = picojson::value( (double) FIRE );
  jsonControls["JUMP"]            = picojson::value( (double) JUMP );
  jsonControls["THROTTLE_DOWN"]   = picojson::value( (double) THROTTLE_DOWN );
  jsonControls["THROTTLE_UP"]     = picojson::value( (double) THROTTLE_UP );
  jsonControls["TURN_LEFT"]       = picojson::value( (double) TURN_LEFT );
  jsonControls["TURN_RIGHT"]      = picojson::value( (double) TURN_RIGHT );

  picojson::object jsonUtility;
  jsonUtility["LogToConsole"]     = picojson::value( game.output.toConsole );
  jsonUtility["LogToFile"]        = picojson::value( game.output.toFile);
  jsonUtility["StatsOutput"]      = picojson::value( game.output.stats );
  jsonUtility["ShowAiLayer"]      = picojson::value( game.debug.ai );
  jsonUtility["ShowCollisions"]   = picojson::value( game.debug.collisions );

  picojson::object jsonSettings;
  jsonSettings["AutoFill"]        = picojson::value( jsonAutoFill );
  jsonSettings["Config"]          = picojson::value( jsonConfig );
  jsonSettings["Controls"]        = picojson::value( jsonControls );
  jsonSettings["Utility"]         = picojson::value( jsonUtility );

  std::string jsonOutput = picojson::value( jsonSettings ).serialize( true );
  settings << jsonOutput;
  settings.close();
}

bool
settingsParse(
  std::ifstream& settings,
  std::string& jsonErrors )
{
  const std::string jsonInput
  {
    std::istreambuf_iterator <char> {settings},
    std::istreambuf_iterator <char> {},
  };

  picojson::value jsonParsed;
  jsonErrors = picojson::parse( jsonParsed, jsonInput );

  if ( jsonErrors.empty() == false )
    return false;

  if ( jsonParsed.is <picojson::object> () == false )
    return false;

  auto& game = gameState();

  auto& jsonValue = jsonParsed.get <picojson::object> ();

  try
  {
    auto& jsonAutoFill = jsonValue["AutoFill"].get <picojson::object> ();

    try { game.isHardcoreEnabled = jsonAutoFill.at( "HARDCORE_MODE" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { LOCAL_PORT = jsonAutoFill.at( "LOCAL_PORT" ).get <double> (); }
    catch ( const std::exception& ) {};

    if ( checkPort( std::to_string(LOCAL_PORT) ) == false )
      LOCAL_PORT = DEFAULT_LOCAL_PORT;

    try { REMOTE_PORT = jsonAutoFill.at( "REMOTE_PORT" ).get <double> (); }
    catch ( const std::exception& ) {};

    if ( checkPort( std::to_string(REMOTE_PORT) ) == false )
      REMOTE_PORT = DEFAULT_REMOTE_PORT;

    try { SERVER_IP = jsonAutoFill.at( "SERVER_IP" ).get <std::string> (); }
    catch ( const std::exception& ) {};

    if ( checkIp(SERVER_IP).empty() == true )
      SERVER_IP = DEFAULT_SERVER_IP;

    try { MMAKE_PASSWORD = jsonAutoFill.at( "MMAKE_PASSWORD" ).get <std::string> (); }
    catch ( const std::exception& ) {};

    if ( checkPass(MMAKE_PASSWORD) == false )
      MMAKE_PASSWORD = "";
  }
  catch ( const std::exception& ) {};

  try
  {
    auto& jsonConfig = jsonValue["Config"].get <picojson::object> ();

    try { game.autoSkipIntro = jsonConfig.at( "AutoSkipIntro" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.isAudioEnabled = jsonConfig.at( "EnableAudio" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.isVSyncEnabled = jsonConfig.at( "EnableVSync" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.audioVolume = jsonConfig.at( "AudioVolume" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { game.stereoDepth = jsonConfig.at( "StereoDepth" ).get <double> (); }
    catch ( const std::exception& ) {};

    game.stereoDepth = std::clamp(game.stereoDepth, 0.f, 1.f);
    game.audioVolume = std::clamp(game.audioVolume, 0.f, 1.f);
  }
  catch ( const std::exception& ) {};

  try
  {
    auto& jsonControls = jsonValue["Controls"].get <picojson::object> ();

    try { FIRE = (SDL_Scancode) jsonControls.at( "FIRE" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { JUMP = (SDL_Scancode) jsonControls.at( "JUMP" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { THROTTLE_DOWN = (SDL_Scancode) jsonControls.at( "THROTTLE_DOWN" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { THROTTLE_UP = (SDL_Scancode) jsonControls.at( "THROTTLE_UP" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { TURN_LEFT = (SDL_Scancode) jsonControls.at( "TURN_LEFT" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { TURN_RIGHT = (SDL_Scancode) jsonControls.at( "TURN_RIGHT" ).get <double> (); }
    catch ( const std::exception& ) {};


    if ( FIRE >= SDL_NUM_SCANCODES )
      FIRE = DEFAULT_FIRE;

    if ( JUMP >= SDL_NUM_SCANCODES )
      JUMP = DEFAULT_JUMP;

    if ( THROTTLE_DOWN >= SDL_NUM_SCANCODES )
      THROTTLE_DOWN = DEFAULT_THROTTLE_DOWN;

    if ( THROTTLE_UP >= SDL_NUM_SCANCODES )
      THROTTLE_UP = DEFAULT_THROTTLE_UP;

    if ( TURN_LEFT >= SDL_NUM_SCANCODES )
      TURN_LEFT = DEFAULT_TURN_LEFT;

    if ( TURN_RIGHT >= SDL_NUM_SCANCODES )
      TURN_RIGHT = DEFAULT_TURN_RIGHT;
  }
  catch ( const std::exception& ) {};

  try
  {
    auto& jsonUtility  = jsonValue["Utility"].get <picojson::object> ();

    try { game.output.toConsole = jsonUtility.at( "LogToConsole" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.output.toFile = jsonUtility.at( "LogToFile" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.output.stats = jsonUtility.at( "StatsOutput" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.debug.ai = jsonUtility.at( "ShowAiLayer" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.debug.collisions = jsonUtility.at( "ShowCollisions" ).get <bool> (); }
    catch ( const std::exception& ) {};
  }
  catch ( const std::exception& ) {};

  return true;
}

void
logVersionAndReadSettings()
{
  const auto& game = gameState();

  bool settingsReadSuccess = true;
  std::string jsonErrors;

  std::ifstream settings {CONFIG_FILENAME};

  if ( settings.is_open() == true )
    settingsReadSuccess = settingsParse( settings, jsonErrors );

  if ( game.output.toFile == true )
  {
    std::ofstream logFile { LOG_FILENAME, std::ios::trunc };
    logFile.close();
  }

  log_message( "Biplanes Revival version " BIPLANES_VERSION "\n" );
  logSDL2Version();

  if ( settings.is_open() == false )
  {
    log_message( "LOG: Can't find " CONFIG_FILENAME "! " );
    log_message( "Creating new " CONFIG_FILENAME, "\n" );
    settingsWrite();
  }
  else if ( settingsReadSuccess == false )
  {
    log_message( "LOG: Failed to parse " CONFIG_FILENAME "! Error:\n", jsonErrors, "\n\n" );
    log_message( "LOG: Creating new " CONFIG_FILENAME "\n" );
    settings.close();
    settingsWrite();
  }
  else
    settings.close();

  if ( game.output.stats == true )
  {
    if ( statsRead() == false )
    {
      log_message( "LOG: Failed to read " STATS_FILENAME "! " );
      log_message( "Creating new " STATS_FILENAME "\n\n" );

      if ( stats_write() == false )
      {
        log_message( "LOG: Failed to create " STATS_FILENAME "! " );
        log_message( "Player statistics won't be saved!", "\n\n" );
      }
    }
  }
}

void
log_message(
  const std::string& message,
  const std::string& buffer1,
  const std::string& buffer2,
  const std::string& buffer3 )
{
  const auto& game = gameState();

  if ( game.output.toConsole == true )
    std::cout << message << buffer1 << buffer2 << buffer3;

  if ( game.output.toFile == true )
  {
    std::ofstream logFile { LOG_FILENAME, std::ios::app };

    if ( logFile.is_open() == true )
      logFile << message << buffer1 << buffer2 << buffer3;
  }
}


void
logSDL2Version()
{
  SDL_version SDL2_libVer;
  SDL_version SDL2_dllVer;
  SDL_version SDL2_img_libVer;
  SDL_version SDL2_mix_libVer;

  SDL_VERSION( &SDL2_libVer );
  SDL_GetVersion( &SDL2_dllVer );

  SDL_IMAGE_VERSION( &SDL2_img_libVer );
  const SDL_version* SDL2_img_dllVer = IMG_Linked_Version();

  SDL_MIXER_VERSION( &SDL2_mix_libVer );
  const SDL_version* SDL2_mix_dllVer = Mix_Linked_Version();

  std::string libVer;
  std::string dllVer;

  libVer =  std::to_string( SDL2_libVer.major ) + ".";
  libVer += std::to_string( SDL2_libVer.minor ) + ".";
  libVer += std::to_string( SDL2_libVer.patch );

  dllVer =  std::to_string( SDL2_dllVer.major ) + ".";
  dllVer += std::to_string( SDL2_dllVer.minor ) + ".";
  dllVer += std::to_string( SDL2_dllVer.patch );

  log_message( "SDL2 lib ver: ", libVer, "\n" );
  log_message( "SDL2.dll ver: ", dllVer, "\n" );

  libVer =  std::to_string( SDL2_img_libVer.major ) + ".";
  libVer += std::to_string( SDL2_img_libVer.minor ) + ".";
  libVer += std::to_string( SDL2_img_libVer.patch );

  dllVer =  std::to_string( SDL2_img_dllVer->major ) + ".";
  dllVer += std::to_string( SDL2_img_dllVer->minor ) + ".";
  dllVer += std::to_string( SDL2_img_dllVer->patch );

  log_message( "SDL2_image lib ver: ", libVer, "\n" );
  log_message( "SDL2_image.dll ver: ", dllVer, "\n" );

  libVer =  std::to_string( SDL2_mix_libVer.major ) + ".";
  libVer += std::to_string( SDL2_mix_libVer.minor ) + ".";
  libVer += std::to_string( SDL2_mix_libVer.patch );

  dllVer =  std::to_string( SDL2_mix_dllVer->major ) + ".";
  dllVer += std::to_string( SDL2_mix_dllVer->minor ) + ".";
  dllVer += std::to_string( SDL2_mix_dllVer->patch );

  log_message( "SDL2_mixer lib ver: ", libVer, "\n" );
  log_message( "SDL2_mixer.dll ver: ", dllVer, "\n" );

  log_message( "\n" );
}


bool
stats_write()
{
  std::ofstream statsOut { STATS_FILENAME, std::ios::trunc };

  if ( statsOut.is_open() == false )
    return false;

  const auto& stats = gameState().stats.total;

  picojson::object jsonStats;
  jsonStats["ChuteHits"]    = picojson::value( (double) stats.chute_hits );
  jsonStats["Crashes"]      = picojson::value( (double) stats.crashes );
  jsonStats["PlaneDeaths"]  = picojson::value( (double) stats.plane_deaths );
  jsonStats["PilotDeaths"]  = picojson::value( (double) stats.pilot_deaths );
  jsonStats["Falls"]        = picojson::value( (double) stats.falls );
  jsonStats["Jumps"]        = picojson::value( (double) stats.jumps );
  jsonStats["Kills"]        = picojson::value( (double) stats.plane_kills );
  jsonStats["Losses"]       = picojson::value( (double) stats.losses );
  jsonStats["PilotHits"]    = picojson::value( (double) stats.pilot_hits );
  jsonStats["PlaneHits"]    = picojson::value( (double) stats.plane_hits );
  jsonStats["Rescues"]      = picojson::value( (double) stats.rescues );
  jsonStats["Shots"]        = picojson::value( (double) stats.shots );
  jsonStats["Wins"]         = picojson::value( (double) stats.wins );


  std::string jsonOutput = picojson::value( jsonStats ).serialize( true );
  statsOut << jsonOutput;

  statsOut.close();

  return true;
}

bool
statsRead()
{
  std::ifstream statsInput {STATS_FILENAME};

  if ( statsInput.is_open() == false )
    return false;


  const std::string jsonInput
  {
    std::istreambuf_iterator <char> {statsInput},
    std::istreambuf_iterator <char> {},
  };

  statsInput.close();

  picojson::value jsonParsed;
  std::string jsonErrors = picojson::parse(
    jsonParsed, jsonInput );

  if ( jsonErrors.empty() == false )
    return false;

  if ( jsonParsed.is <picojson::object> () == false )
    return false;


  auto& stats = gameState().stats.total;

  auto& jsonStats = jsonParsed.get <picojson::object> ();

  try
  {
    try { stats.chute_hits = jsonStats.at( "ChuteHits" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.crashes = jsonStats.at( "Crashes" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.plane_deaths = jsonStats.at( "PlaneDeaths" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.pilot_deaths = jsonStats.at( "PilotDeaths" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.falls = jsonStats.at( "Falls" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.jumps = jsonStats.at( "Jumps" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.plane_kills = jsonStats.at( "Kills" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.losses = jsonStats.at( "Losses" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.pilot_hits = jsonStats.at( "PilotHits" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.plane_hits = jsonStats.at( "PlaneHits" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.rescues = jsonStats.at( "Rescues" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.shots = jsonStats.at( "Shots" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.wins = jsonStats.at( "Wins" ).get <double> (); }
    catch ( const std::exception& ) {};
  }
  catch ( const std::exception& ) {};

  calcDerivedStats(stats);

  return true;
}


std::string
checkIp(
  const std::string& ipAddress )
{
  if ( ipAddress.length() == 0 || ipAddress.length() > 15 )
    return {};

  std::stringstream stream {ipAddress};
  int a, b, c, d;
  char ch;
  stream >> a >> ch >> b >> ch >> c >> ch >> d;

  if ( a > 255 || a < 0 ||
       b > 255 || b < 0 ||
       c > 255 || c < 0 ||
       d > 255 || d < 0 ||
       ch != '.' )
    return {};

  return
  {
    std::to_string(a) + "." +
    std::to_string(b) + "." +
    std::to_string(c) + "." +
    std::to_string(d)
  };
}

bool
checkPort(
  const std::string& port )
{
  if ( port.length() == 0 )
    return false;


  for ( const auto& digit : port )
    if ( isdigit(digit) == false )
      return false;


  const auto portNum = stoi(port);

  if ( portNum > 1024 && portNum <= 65535 )
    return true;

  return false;
}

bool
checkPass(
  const std::string& password )
{
  if ( password.length() == 0 )
    return true;

  if ( password.length() > 15 )
    return false;


  for ( const auto& letter : password )
  {
    if ( letter >= '0' && letter <= '9' )
      continue;

    if ( letter >= 'A' && letter <= 'Z' )
      continue;

    if ( letter >= 'a' && letter <= 'z' )
      continue;

    return false;
  }

  return true;
}

bool
checkScoreToWin(
  const std::string& score )
{
  if ( score.length() > 3 )
    return false;

  for ( const auto& digit : score )
    if ( isdigit(digit) == false )
      return false;

  const auto scoreNum = stoi(score);
  const auto maxScore = std::numeric_limits <uint8_t>::max();

  return
    scoreNum >= 0 &&
    scoreNum <= maxScore;
}

bool
checkPercentage(
  const std::string& percentage )
{
  if ( percentage.empty() == true )
    return false;

  for ( const auto& digit : percentage )
    if ( std::isdigit(digit) == false )
      return false;

  const auto percentageNum = std::stoi(percentage);

  return
    percentageNum >= 0u &&
    percentageNum <= 100u;
}

size_t
percentageToInteger(
  const float percentage )
{
  return std::clamp(percentage * 100.f, 0.f, 100.f);
}
