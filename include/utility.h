#ifndef UTILITY_H
#define UTILITY_H


double getCurrentTime();
void countDelta();


void settings_write();
void log_message( const char *message, const char *buffer1 = NULL,
                  const char *buffer2 = NULL, const char *buffer3 = NULL );

void log_message( std::string message, std::string buffer1 = "",
                  std::string buffer2 = "", std::string buffer3 = "" );


std::string checkIp( std::string );
bool checkPort( std::string );
bool checkPass( std::string );

class Timer
{
private:
  float cooldown;
  float counter;
  bool counting;

public:
  Timer( float );
  void Update();
  void Start();
  void Stop();
  void Reset();
  void SetNewCounter( float );

  bool isReady();
};


#endif // UTILITY_H
