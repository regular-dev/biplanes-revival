/*
  Biplanes Revival
  Copyright (C) 2019-2024 Regular-dev community
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


void settingsWrite();
bool settingsParse( std::ifstream&, std::string& );
void logVersionAndReadSettings();

void log_message(
  const std::string& message, const std::string& buffer1 = {},
  const std::string& buffer2 = {}, const std::string& buffer3 = {} );

void logSDL2Version();
bool stats_write();
bool statsRead();

std::string checkIp( const std::string& );
bool checkPort( const std::string& );
bool checkPass( const std::string& );
bool checkScoreToWin( const std::string& );
bool checkPercentage( const std::string& );

size_t fractionToPercentage( const double );
