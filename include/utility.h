//    Biplanes Revival
//    Copyright (C) 2019-2020 Regular-dev community
//    https://regular-dev.org/
//    regular.dev.org@gmail.com
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.


#ifndef UTILITY_H
#define UTILITY_H


double getCurrentTime();
void countDelta();


void settings_write();
bool settings_parse( std::ifstream&, std::string& );
void logVerReadSettings();

void log_message( std::string message, std::string buffer1 = "",
                  std::string buffer2 = "", std::string buffer3 = "" );

void logSDL2Ver();
bool stats_write();
bool stats_read();

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

  float remainderTime();
  bool isReady();
};


#endif // UTILITY_H
