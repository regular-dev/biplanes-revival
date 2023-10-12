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

#include <iosfwd>
#include <string>


double getCurrentTime();
double countDelta();


void settings_write();
bool settings_parse( std::ifstream&, std::string& );
void logVerReadSettings();

void log_message( const std::string& message, const std::string& buffer1 = {},
                  const std::string& buffer2 = {}, const std::string& buffer3 = {} );

void logSDL2Ver();
bool stats_write();
bool stats_read();

std::string checkIp( std::string );
bool checkPort( std::string );
bool checkPass( std::string );

class Timer
{
private:
  float timeout {};
  float counter {};
  bool counting {};

public:
  Timer( const float newTimeout );

  void Update();
  void Start();
  void Stop();
  void Reset();
  void SetNewTimeout( const float );

  float remainderTime() const;
  bool isReady() const;
};
