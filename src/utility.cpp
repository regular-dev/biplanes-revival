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

#include <include/utility.hpp>
#include <include/sdl.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/controls.hpp>
#include <include/stats.hpp>
#include <include/variables.hpp>

#include <lib/picojson.h>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>

#define CONFIG_FILENAME BIPLANES_EXE_NAME ".conf"
#define STATS_FILENAME BIPLANES_EXE_NAME ".stats"
#define LOG_FILENAME BIPLANES_EXE_NAME ".log"


static std::string
get_appimage_dir()
{
#if defined(_WIN32) || defined(__APPLE__) || defined(__MACH__)
  return {};
#endif


  const auto appImageDir = std::getenv("BIPLANES_APPIMAGE_DIR");

  if (  appImageDir != nullptr &&
        std::string{appImageDir}.empty() == false )
    return {appImageDir};

  return {};
}

static std::string
get_config_path()
{
#if defined(_WIN32) || defined(__APPLE__) || defined(__MACH__)
  return CONFIG_FILENAME;
#endif


  const auto appImageDir = get_appimage_dir();

  if ( appImageDir.empty() == false )
    return appImageDir + "/" CONFIG_FILENAME;


  const auto configParentPath = std::getenv("XDG_CONFIG_HOME");

  if (  configParentPath == nullptr ||
        std::string{configParentPath}.empty() == true )
    return CONFIG_FILENAME;

  return std::string{configParentPath} + "/" CONFIG_FILENAME;
}

static std::string
get_log_path()
{
#if defined(_WIN32) || defined(__APPLE__) || defined(__MACH__)
  return LOG_FILENAME;
#endif


  const auto appImageDir = get_appimage_dir();

  if ( appImageDir.empty() == false )
    return appImageDir + "/" LOG_FILENAME;


  auto logParentPath = std::getenv("XDG_STATE_HOME");

  if (  logParentPath == nullptr ||
        std::string{logParentPath}.empty() == true )
  {
    logParentPath = std::getenv("XDG_CACHE_HOME");

    if ( logParentPath == nullptr )
      return LOG_FILENAME;
  }

  return std::string{logParentPath} + "/" LOG_FILENAME;
}

static std::string
get_stats_path()
{
#if defined(_WIN32) || defined(__APPLE__) || defined(__MACH__)
  return STATS_FILENAME;
#endif


  const auto appImageDir = get_appimage_dir();

  if ( appImageDir.empty() == false )
    return appImageDir + "/" STATS_FILENAME;


  auto statsParentPath = std::getenv("XDG_DATA_HOME");

  if (  statsParentPath == nullptr ||
        std::string{statsParentPath}.empty() == true )
    return STATS_FILENAME;

  return std::string{statsParentPath} + "/" STATS_FILENAME;
}


void
settingsWrite()
{
  log_message("OUTPUT: Writing settings", "\n");

  const auto& game = gameState();

  const auto configPath = get_config_path();

  std::ofstream settings {configPath, std::ios::out};

  if ( settings.is_open() == false )
  {
    log_message( "LOG: Can't write to '" + configPath + "' !" );
    log_message( "User settings & key binds won't be saved!\n\n" );
    show_warning( "Warning!", "Can't write to '" + configPath + "' !\nUser settings & key binds won't be saved!" );

    return;
  }

  picojson::object jsonAutoFill;
  jsonAutoFill["ExtraClouds"]     = picojson::value( game.featuresLocal.extraClouds );
  jsonAutoFill["OneShotKills"]    = picojson::value( game.featuresLocal.oneShotKills );
  jsonAutoFill["AltHitboxes"]     = picojson::value( game.featuresLocal.alternativeHitboxes );

  jsonAutoFill["LOCAL_PORT"]      = picojson::value( (double) LOCAL_PORT );
  jsonAutoFill["REMOTE_PORT"]     = picojson::value( (double) REMOTE_PORT );
  jsonAutoFill["SERVER_IP"]       = picojson::value( SERVER_IP );
  jsonAutoFill["MMAKE_PASSWORD"]  = picojson::value( MMAKE_PASSWORD );

  picojson::object jsonConfig;
  jsonConfig["AutoSkipIntro"]     = picojson::value( game.autoSkipIntro );
  jsonConfig["EnableAudio"]       = picojson::value( game.isAudioEnabled );
  jsonConfig["EnableVSync"]       = picojson::value( game.isVSyncEnabled );
  jsonConfig["AudioVolume"]       = picojson::value( game.audioVolume / 100. );
  jsonConfig["StereoDepth"]       = picojson::value( game.stereoDepth / 100. );

  picojson::object jsonControls;
  jsonControls["FIRE"]            = picojson::value( (double) bindings::player1.fire );
  jsonControls["JUMP"]            = picojson::value( (double) bindings::player1.jump );
  jsonControls["THROTTLE_DOWN"]   = picojson::value( (double) bindings::player1.throttleDown );
  jsonControls["THROTTLE_UP"]     = picojson::value( (double) bindings::player1.throttleUp );
  jsonControls["TURN_LEFT"]       = picojson::value( (double) bindings::player1.turnLeft );
  jsonControls["TURN_RIGHT"]      = picojson::value( (double) bindings::player1.turnRight );

  jsonControls["FIRE_P2"]            = picojson::value( (double) bindings::player2.fire );
  jsonControls["JUMP_P2"]            = picojson::value( (double) bindings::player2.jump );
  jsonControls["THROTTLE_DOWN_P2"]   = picojson::value( (double) bindings::player2.throttleDown );
  jsonControls["THROTTLE_UP_P2"]     = picojson::value( (double) bindings::player2.throttleUp );
  jsonControls["TURN_LEFT_P2"]       = picojson::value( (double) bindings::player2.turnLeft );
  jsonControls["TURN_RIGHT_P2"]      = picojson::value( (double) bindings::player2.turnRight );

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

    try { game.featuresLocal.extraClouds = jsonAutoFill.at( "ExtraClouds" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.featuresLocal.oneShotKills = jsonAutoFill.at( "OneShotKills" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.featuresLocal.alternativeHitboxes = jsonAutoFill.at( "AltHitboxes" ).get <bool> (); }
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
    double audioVolume {};
    double stereoDepth {};


    auto& jsonConfig = jsonValue["Config"].get <picojson::object> ();

    try { game.autoSkipIntro = jsonConfig.at( "AutoSkipIntro" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.isAudioEnabled = jsonConfig.at( "EnableAudio" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { game.isVSyncEnabled = jsonConfig.at( "EnableVSync" ).get <bool> (); }
    catch ( const std::exception& ) {};

    try { audioVolume = jsonConfig.at( "AudioVolume" ).get <double> (); }
    catch ( const std::exception& ) { audioVolume = game.audioVolume / 100.; };

    try { stereoDepth = jsonConfig.at( "StereoDepth" ).get <double> (); }
    catch ( const std::exception& ) { stereoDepth = game.stereoDepth / 100.; };


    if ( audioVolume >= 0.0 && audioVolume <= 1.0 )
      game.audioVolume = fractionToPercentage(audioVolume);

    if ( stereoDepth >= 0.0 && stereoDepth <= 1.0 )
      game.stereoDepth = fractionToPercentage(stereoDepth);
  }
  catch ( const std::exception& ) {};

  try
  {
    auto& jsonControls = jsonValue["Controls"].get <picojson::object> ();

    try { bindings::player1.fire = (SDL_Scancode) jsonControls.at( "FIRE" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player1.jump = (SDL_Scancode) jsonControls.at( "JUMP" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player1.throttleDown = (SDL_Scancode) jsonControls.at( "THROTTLE_DOWN" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player1.throttleUp = (SDL_Scancode) jsonControls.at( "THROTTLE_UP" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player1.turnLeft = (SDL_Scancode) jsonControls.at( "TURN_LEFT" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player1.turnRight = (SDL_Scancode) jsonControls.at( "TURN_RIGHT" ).get <double> (); }
    catch ( const std::exception& ) {};


    try { bindings::player2.fire = (SDL_Scancode) jsonControls.at( "FIRE_P2" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player2.jump = (SDL_Scancode) jsonControls.at( "JUMP_P2" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player2.throttleDown = (SDL_Scancode) jsonControls.at( "THROTTLE_DOWN_P2" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player2.throttleUp = (SDL_Scancode) jsonControls.at( "THROTTLE_UP_P2" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player2.turnLeft = (SDL_Scancode) jsonControls.at( "TURN_LEFT_P2" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { bindings::player2.turnRight = (SDL_Scancode) jsonControls.at( "TURN_RIGHT_P2" ).get <double> (); }
    catch ( const std::exception& ) {};


    bindings::player1.verifyAndFix(bindings::defaults::player1);
    bindings::player2.verifyAndFix(bindings::defaults::player2);
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

  const auto configPath = get_config_path();
  const auto statsPath = get_stats_path();

  std::ifstream settings {configPath};

  if ( settings.is_open() == true )
    settingsReadSuccess = settingsParse( settings, jsonErrors );

  if ( game.output.toFile == true )
  {
    std::ofstream logFile { get_log_path(), std::ios::trunc };
    logFile.close();
  }

  log_message( "Biplanes Revival version " BIPLANES_VERSION "\n" );
  logSDL2Version();

  if ( settings.is_open() == false )
  {
    log_message( "LOG: Can't find '" + configPath + "'! " );
    log_message( "Creating new '" + configPath + "'\n" );
    settingsWrite();
  }
  else if ( settingsReadSuccess == false )
  {
    log_message( "LOG: Failed to parse '" + configPath + "'! Error:\n", jsonErrors, "\n\n" );
    log_message( "LOG: Creating new '" + configPath + "'\n" );
    settings.close();
    settingsWrite();
  }
  else
    settings.close();

  if ( game.output.stats == true )
  {
    if ( statsRead() == false )
    {
      log_message( "LOG: Can't find '" + statsPath + "'! " );
      log_message( "Creating new '" + statsPath + "'\n\n" );

      if ( stats_write() == false )
      {
        log_message( "LOG: Failed to create '" + statsPath + "'! " );
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
    std::ofstream logFile { get_log_path(), std::ios::app };

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
  log_message("OUTPUT: Writing stats", "\n");

  std::ofstream statsOut { get_stats_path(), std::ios::trunc };

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
  jsonStats["WinsVsDeveloper"] = picojson::value( (double) stats.wins_vs_developer );
  jsonStats["WinsVsInsane"] = picojson::value( (double) stats.wins_vs_insane );


  std::string jsonOutput = picojson::value( jsonStats ).serialize( true );
  statsOut << jsonOutput;

  statsOut.close();

  return true;
}

bool
statsRead()
{
  std::ifstream statsInput {get_stats_path()};

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

    try { stats.wins_vs_developer = jsonStats.at( "WinsVsDeveloper" ).get <double> (); }
    catch ( const std::exception& ) {};

    try { stats.wins_vs_insane = jsonStats.at( "WinsVsInsane" ).get <double> (); }
    catch ( const std::exception& ) {};
  }
  catch ( const std::exception& ) {};

  calcDerivedStats(stats);

  return true;
}

bool
isInsaneUnlocked()
{
  const auto& stats = gameState().stats.total;
  return stats.wins_vs_developer > 0;
}


std::string
checkIp(
  const std::string& ipAddress )
{
  if ( ipAddress.length() == 0 ||
       ipAddress.length() > constants::menu::maxInputFieldTextLength )
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
    if ( std::isdigit(digit) == false )
      return false;


  const auto portNum = std::stoi(port);

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

  if ( password.length() > constants::menu::maxInputFieldTextLength )
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
  const auto maxWinScoreTextLength =
    std::to_string(constants::maxWinScore).size();

  if ( score.length() > maxWinScoreTextLength )
    return false;

  for ( const auto& digit : score )
    if ( std::isdigit(digit) == false )
      return false;

  const auto scoreNum = std::stoi(score);

  return
    scoreNum >= 0 &&
    scoreNum <= constants::maxWinScore;
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
fractionToPercentage(
  const double fraction )
{
  return std::round(
    std::clamp(fraction * 100., 0., 100.) );
}
