#include <fstream>
#include <sstream>
#include <time.h>

#include "../include/variables.h"
#include "../include/utility.h"

double getCurrentTime()
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
  // POSIX implementation
  timespec time;
  clock_gettime( CLOCK_MONOTONIC, &time );
  return static_cast<uint64_t>( time.tv_sec ) * 1000000 + time.tv_nsec / 1000;
#endif
}


void countDelta()
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
  EndingTime = getCurrentTime() / 1000000.0;
  deltaTime = ( EndingTime - StartingTime );
  StartingTime = getCurrentTime() / 1000000.0;

#elif PLATFORM == PLATFORM_WINDOWS
  QueryPerformanceFrequency( &Frequency );
  QueryPerformanceCounter( &EndingTime );
  deltaTime = static_cast<double>( EndingTime.QuadPart - StartingTime.QuadPart ) / Frequency.QuadPart;
  QueryPerformanceCounter( &StartingTime );
#endif
}


// WRITE CHANGES TO SETTINGS FILE
void settings_write()
{
  std::ofstream settings( "settings.ini", std::ios::out );
  if ( settings.is_open() )
  {
    settings <<
      THROTTLE_UP << "\n" <<
      THROTTLE_DOWN << "\n" <<
      TURN_LEFT << "\n" <<
      TURN_RIGHT << "\n" <<
      FIRE << "\n" <<
      JUMP << "\n" <<
      HOST_PORT << "\n" <<
      SERVER_IP << "\n" <<
      CLIENT_PORT << "\n" <<
      PLANE_COLLISIONS << "\n" <<
      CLOUDS_OPAQUE << "\n" <<
      MMAKE_PASSWORD;
    settings.close();
  }
  else
  {
    log_message( "LOG: Can't create user settings file! User key bindings will not be saved!\n\n" );
    show_warning( "Warning!", "Can't create user settings file!\nUser key bindings will not be saved!" );
  }
}


// LOG MESSAGES
void log_message( const char *message, const char *buffer1,  const char *buffer2, const char *buffer3 )
{
  printf( message, buffer1, buffer2, buffer3 );
  std::ofstream log("log.log", std::ios::app);
  if ( log.is_open() )
    log << message << buffer1 << buffer2 << buffer3;
}

void log_message( std::string message, std::string buffer1, std::string buffer2, std::string buffer3 )
{
//  std::cout << message << buffer1 << buffer2 << buffer3;  // requires iostream included
  std::ofstream log("log.log", std::ios::app);
	if ( log.is_open() )
    log << message << buffer1 << buffer2 << buffer3;
}


Timer::Timer( float set_cooldown )
{
  counter = 0.0f;
  cooldown = set_cooldown;
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
  counter = cooldown;
}

void Timer::SetNewCounter( float new_counter )
{
  cooldown = new_counter;
}

bool Timer::isReady()
{
  return counter <= 0.0f;
}


std::string checkIp( std::string IpToCheck )
{
  if ( IpToCheck.length() > 0 )
  {
    std::stringstream s( IpToCheck );
    int a, b, c, d;
    char ch;
    s >> a >> ch >> b >> ch >> c >> ch >> d;
    if ( a > 255 || a < 0 ||
         b > 255 || b < 0 ||
         c > 255 || c < 0 ||
         d > 255 || d < 0 ||
         ch != '.' )
    {
      return "";
    }
    else
    {
      IpToCheck = std::to_string( a ) + "." +
                  std::to_string( b ) + "." +
                  std::to_string( c ) + "." +
                  std::to_string( d );
      return IpToCheck;
    }
  }
  else
    return "";
}

bool checkPort( std::string PortToCheck )
{
  if ( PortToCheck.length() > 0 )
  {
    for ( unsigned int i = 0; i < PortToCheck.length(); i++)
    {
      if ( !isdigit( PortToCheck[i] ) )
        return false;
    }

    if ( stoi ( PortToCheck ) <= 100 )
      return false;
    else if ( stoi ( PortToCheck ) > 65535 )
      return false;
    else
      return true;
  }
  else
    return false;
}

bool checkPass( std::string PassToCheck )
{
  if ( PassToCheck.length() > 0 )
  {
    if ( PassToCheck.find( " " ) != std::string::npos )
      return false;

    return true;
  }
  else
    return false;
}

