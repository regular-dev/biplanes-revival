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

#include <deque>


extern std::deque <unsigned char> eventsLocal;
extern bool eventNewIterationLoc;
extern unsigned char eventCounterLoc;
extern unsigned char eventCounterOpp;

void prepare_local_data();
void transform_opponent_data();
void prepare_plane_coords();

void event_push( unsigned char );
void events_pack();
void events_reset();


enum class EVENTS : unsigned char
{
  NONE        = 'n',
  NO_HARDCORE = 'h',

  SHOOT       = 's',
  EJECT       = 'e',

  HIT_PLANE   = 'P',
  HIT_CHUTE   = 'c',
  HIT_PILOT   = 'p',
  PLANE_DEATH = 'D',
  PILOT_DEATH = 'd',
  PLANE_RESP  = 'R',
  PILOT_LAND  = 'l'
};


struct Packet
{
  char pitch;
  char throttle;
  bool disconnect;

  float x;
  float y;
  float dir;
  float pilot_x;
  float pilot_y;

  unsigned char events[32];
};

void erasePacket( Packet& );
