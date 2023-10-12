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

#if PLATFORM == PLATFORM_UNIX
  #include <time.h>
#endif

#include <include/variables.h>
#include <include/utility.h>
#include <lib/picojson.h>

#include <fstream>
#include <sstream>
#include <iostream>


double getCurrentTime()
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
  // POSIX implementation
  timespec time;
  clock_gettime( CLOCK_MONOTONIC, &time );
  return static_cast<uint64_t>( time.tv_sec ) * 1000000 + time.tv_nsec / 1000;
#endif
}


double countDelta()
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
  EndingTime = getCurrentTime() / 1000000.0;
  deltaTime = ( EndingTime - StartingTime );
  StartingTime = getCurrentTime() / 1000000.0;

#elif PLATFORM == PLATFORM_WINDOWS
  QueryPerformanceFrequency( &Frequency );
  QueryPerformanceCounter( &EndingTime );
  deltaTime = static_cast <double> ( EndingTime.QuadPart - StartingTime.QuadPart ) / Frequency.QuadPart;
  QueryPerformanceCounter( &StartingTime );
#endif

  return deltaTime;
}


// WRITE CHANGES TO BIPLANES.INI
void settings_write()
{
  std::ofstream settings( "biplanes.ini", std::ios::out );
  if ( settings.is_open() )
  {
    picojson::object jsonAutoFill;
    jsonAutoFill["HARDCORE_MODE"]   = picojson::value( HARDCORE_MODE );
    jsonAutoFill["HOST_PORT"]       = picojson::value( (double) HOST_PORT );
    jsonAutoFill["MMAKE_PASSWORD"]  = picojson::value( MMAKE_PASSWORD );
    jsonAutoFill["SERVER_IP"]       = picojson::value( SERVER_IP );
    jsonAutoFill["SERVER_PORT"]     = picojson::value( (double) SERVER_PORT );

    picojson::object jsonControls;
    jsonControls["FIRE"]            = picojson::value( (double) FIRE );
    jsonControls["JUMP"]            = picojson::value( (double) JUMP );
    jsonControls["THROTTLE_DOWN"]   = picojson::value( (double) THROTTLE_DOWN );
    jsonControls["THROTTLE_UP"]     = picojson::value( (double) THROTTLE_UP );
    jsonControls["TURN_LEFT"]       = picojson::value( (double) TURN_LEFT );
    jsonControls["TURN_RIGHT"]      = picojson::value( (double) TURN_RIGHT );

    picojson::object jsonUtility;
    jsonUtility["CmdOutput"]        = picojson::value( consoleOutput );
    jsonUtility["LogOutput"]        = picojson::value( logFileOutput );
    jsonUtility["StatsOutput"]      = picojson::value( statsOutput );
    jsonUtility["ShowHitboxes"]     = picojson::value( show_hitboxes );

    picojson::object jsonSettings;
    jsonSettings["AutoFill"]        = picojson::value( jsonAutoFill );
    jsonSettings["Controls"]        = picojson::value( jsonControls );
    jsonSettings["Utility"]         = picojson::value( jsonUtility );

    std::string jsonOutput = picojson::value( jsonSettings ).serialize( true );
    settings << jsonOutput;
    settings.close();
  }
  else
  {
    log_message( "LOG: Can't write to biplanes.ini! " );
    log_message( "User key bindings will not be saved!\n\n" );
    show_warning( "Warning!", "Can't write to biplanes.ini!\nCustom key bindings will not be saved!" );
  }
}

// PARSE JSON SETTINGS FROM BIPLANES.INI
bool settings_parse( std::ifstream& settings, std::string& jsonErrors )
{
  std::string jsonInput(  ( std::istreambuf_iterator <char> ( settings )  ),
                          ( std::istreambuf_iterator <char> ()      ) );

  picojson::value jsonParsed;
  jsonErrors = picojson::parse( jsonParsed, jsonInput );
  if ( !jsonErrors.empty() )
    return false;

  if ( !jsonParsed.is <picojson::object> () )
    return false;

  picojson::value::object& jsonValue = jsonParsed.get <picojson::object> ();
  try
  {
    picojson::value::object& jsonAutoFill = jsonValue["AutoFill"].get <picojson::object> ();

    try { HARDCORE_MODE = jsonAutoFill.at( "HARDCORE_MODE" ).get <bool> (); }
    catch ( std::out_of_range &e ) {};

    try { HOST_PORT = jsonAutoFill.at( "HOST_PORT" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    if ( !checkPort( std::to_string(HOST_PORT) ) )
      HOST_PORT = DEFAULT_HOST_PORT;

    try { MMAKE_PASSWORD = jsonAutoFill.at( "MMAKE_PASSWORD" ).get <std::string> (); }
    catch ( std::out_of_range &e ) {};

    if ( !checkPass( MMAKE_PASSWORD ) )
      MMAKE_PASSWORD = "";

    try { SERVER_IP = jsonAutoFill.at( "SERVER_IP" ).get <std::string> (); }
    catch ( std::out_of_range &e ) {};

    if ( checkIp( SERVER_IP ).empty() )
      SERVER_IP = DEFAULT_SERVER_IP;

    try { SERVER_PORT = jsonAutoFill.at( "SERVER_PORT" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    if ( !checkPort( std::to_string(SERVER_PORT) ) )
      SERVER_PORT = DEFAULT_SERVER_PORT;
  }
  catch ( std::out_of_range &e ) {};

  try
  {
    picojson::value::object& jsonControls = jsonValue["Controls"].get <picojson::object> ();

    try { FIRE = jsonControls.at( "FIRE" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { JUMP = jsonControls.at( "JUMP" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { THROTTLE_DOWN = jsonControls.at( "THROTTLE_DOWN" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { THROTTLE_UP = jsonControls.at( "THROTTLE_UP" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { TURN_LEFT = jsonControls.at( "TURN_LEFT" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { TURN_RIGHT = jsonControls.at( "TURN_RIGHT" ).get <double> (); }
    catch ( std::out_of_range &e ) {};
  }
  catch ( std::out_of_range &e ) {};

  try
  {
    picojson::value::object& jsonUtility  = jsonValue["Utility"].get <picojson::object> ();

    try { consoleOutput = jsonUtility.at( "CmdOutput" ).get <bool> (); }
    catch ( std::out_of_range &e ) {};

    try { logFileOutput = jsonUtility.at( "LogOutput" ).get <bool> (); }
    catch ( std::out_of_range &e ) {};

    try { statsOutput   = jsonUtility.at( "StatsOutput" ).get <bool> (); }
    catch ( std::out_of_range &e ) {};

    try { show_hitboxes = jsonUtility.at( "ShowHitboxes" ).get <bool> (); }
    catch ( std::out_of_range &e ) {};
  }
  catch ( std::out_of_range &e ) {};

  return true;
}

// READ SETTINGS AND PRINT GAME VERSION
void logVerReadSettings()
{
  bool settingsReadSuccess = true;
  std::string jsonErrors;

  std::ifstream settings( "biplanes.ini" );
  if ( settings.is_open() )
    settingsReadSuccess = settings_parse( settings, jsonErrors );

  if ( logFileOutput )
  {
    std::ofstream log( "biplanes.log", std::ios::trunc );
    log.close();
  }

  log_message( "Bluetooth Biplanes Revival version 0.9", "\n" );
  logSDL2Ver();

  if ( !settings.is_open() )
  {
    log_message( "LOG: Can't find biplanes.ini! " );
    log_message( "Creating new biplanes.ini", "\n" );
    settings_write();
  }
  else if ( !settingsReadSuccess )
  {
    std::ofstream log( "biplanes.log", std::ios::trunc );
    log.close();
    log_message( "LOG: Failed to parse biplanes.ini! Error:\n", jsonErrors, "\n\n" );
    log_message( "LOG: Creating new biplanes.ini\n" );
    settings_write();
  }
  else
    settings.close();

  if ( statsOutput )
  {
    if ( !stats_read() )
    {
      log_message( "LOG: Failed to read biplanes.stats! " );
      log_message( "Creating new biplanes.stats", "\n\n" );

      if ( !stats_write() )
      {
        log_message( "LOG: Failed to create biplanes.stats! " );
        log_message( "Player statistics won't be saved!", "\n\n" );
      }
    }
  }
}

void log_message( const std::string& message, const std::string& buffer1, const std::string& buffer2, const std::string& buffer3 )
{
  if ( consoleOutput )
    std::cout << message << buffer1 << buffer2 << buffer3;

  if ( logFileOutput )
  {
    std::ofstream log( "biplanes.log", std::ios::app );
    if ( log.is_open() )
      log << message << buffer1 << buffer2 << buffer3;
  }
}


// LOG SDL2 Version
void logSDL2Ver()
{
  SDL_version SDL2_libVer,
              SDL2_dllVer,
              SDL2_img_libVer,
              SDL2_mix_libVer;

  std::string libVer, dllVer;

  SDL_VERSION( &SDL2_libVer );
  SDL_GetVersion( &SDL2_dllVer );

  SDL_IMAGE_VERSION( &SDL2_img_libVer );
  const SDL_version* SDL2_img_dllVer = IMG_Linked_Version();

  SDL_MIXER_VERSION( &SDL2_mix_libVer );
  const SDL_version* SDL2_mix_dllVer = Mix_Linked_Version();


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


bool stats_write()
{
  std::ofstream statsOut( "biplanes.stats", std::ios::trunc );
  if ( !statsOut.is_open() )
    return false;

  picojson::object jsonStats;
  jsonStats["ChuteHits"]  = picojson::value( (double) stats_total.chute_hits );
  jsonStats["Crashes"]    = picojson::value( (double) stats_total.crashes );
  jsonStats["Deaths"]     = picojson::value( (double) stats_total.deaths );
  jsonStats["Falls"]      = picojson::value( (double) stats_total.falls );
  jsonStats["Jumps"]      = picojson::value( (double) stats_total.jumps );
  jsonStats["Kills"]      = picojson::value( (double) stats_total.plane_kills );
  jsonStats["Losses"]     = picojson::value( (double) stats_total.losses );
  jsonStats["PilotHits"]  = picojson::value( (double) stats_total.pilot_hits );
  jsonStats["PlaneHits"]  = picojson::value( (double) stats_total.plane_hits );
  jsonStats["Rescues"]    = picojson::value( (double) stats_total.rescues );
  jsonStats["Shots"]      = picojson::value( (double) stats_total.shots );
  jsonStats["Wins"]       = picojson::value( (double) stats_total.wins );


  std::string jsonOutput = picojson::value( jsonStats ).serialize( true );
  statsOut << jsonOutput;

  statsOut.close();

  return true;
}

bool stats_read()
{
  std::ifstream statsInput( "biplanes.stats" );

  if ( !statsInput.is_open() )
    return false;

  std::string jsonInput(  ( std::istreambuf_iterator <char> ( statsInput )  ),
                          ( std::istreambuf_iterator <char> ()      ) );

  statsInput.close();

  picojson::value jsonParsed;
  std::string jsonErrors = picojson::parse( jsonParsed, jsonInput );
  if ( !jsonErrors.empty() )
    return false;

  if ( !jsonParsed.is <picojson::object> () )
    return false;

  picojson::value::object& jsonStats = jsonParsed.get <picojson::object> ();
  try
  {
    try { stats_total.chute_hits = jsonStats.at( "ChuteHits" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.crashes = jsonStats.at( "Crashes" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.deaths = jsonStats.at( "Deaths" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.falls = jsonStats.at( "Falls" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.jumps = jsonStats.at( "Jumps" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.plane_kills = jsonStats.at( "Kills" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.losses = jsonStats.at( "Losses" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.pilot_hits = jsonStats.at( "PilotHits" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.plane_hits = jsonStats.at( "PlaneHits" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.rescues = jsonStats.at( "Rescues" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.shots = jsonStats.at( "Shots" ).get <double> (); }
    catch ( std::out_of_range &e ) {};

    try { stats_total.wins = jsonStats.at( "Wins" ).get <double> (); }
    catch ( std::out_of_range &e ) {};
  }
  catch ( std::out_of_range &e ) {};

  return true;
}


Timer::Timer( const float newTimeout )
{
  counter = 0.0f;
  timeout = newTimeout;
  counting = false;
}

void Timer::Update()
{
  if ( counting )
  {
    if ( counter > 0.0f )
      counter -= deltaTime;
    else
      Stop();
  }
}

void Timer::Start()
{
  counting = true;
  Reset();
}

void Timer::Stop()
{
  counting = false;
  counter = 0.0f;
}

void Timer::Reset()
{
    counter = timeout;
}

void Timer::SetNewTimeout( float newTimeout )
{
    timeout = newTimeout;
}

float Timer::remainderTime() const
{
  return counter;
}

bool Timer::isReady() const
{
  return counter <= 0.0f;
}


std::string checkIp( std::string IpToCheck )
{
  if (  IpToCheck.length() == 0 ||
        IpToCheck.length() > 15 )
    return "";

  std::stringstream s( IpToCheck );
  int a, b, c, d;
  char ch;
  s >> a >> ch >> b >> ch >> c >> ch >> d;
  if ( a > 255 || a < 0 ||
       b > 255 || b < 0 ||
       c > 255 || c < 0 ||
       d > 255 || d < 0 ||
       ch != '.' )
    return "";

  return  std::to_string( a ) + "." +
          std::to_string( b ) + "." +
          std::to_string( c ) + "." +
          std::to_string( d );
}

bool checkPort( std::string PortToCheck )
{
  if ( PortToCheck.length() == 0 )
    return false;

  for ( char digit : PortToCheck )
    if ( !isdigit( digit ) )
      return false;

//  for ( unsigned int i = 0; i < PortToCheck.length(); i++)
//  {
//    if ( !isdigit( PortToCheck[i] ) )
//      return false;
//  }

  if (  stoi( PortToCheck ) > 1024 &&
        stoi( PortToCheck ) <= 65535 )
    return true;

  return false;
}

bool checkPass( std::string PassToCheck )
{
  if ( PassToCheck.length() == 0 )
    return true;

  if ( PassToCheck.length() > 15 )
    return false;

  for ( char letter : PassToCheck )
    if (  letter >= '0' &&
          letter <= '9' )
      continue;
    else if ( letter >= 'A' &&
              letter <= 'Z' )
      continue;
    else if ( letter >= 'a' &&
              letter <= 'z' )
      continue;
    else
      return false;

  return true;
}

