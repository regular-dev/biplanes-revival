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

#include <include/network.hpp>
#include <include/plane.hpp>
#include <include/controls.hpp>
#include <include/game_state.hpp>
#include <include/network_data.hpp>
#include <include/network_state.hpp>
#include <include/variables.hpp>

#include <lib/Net.h>

#include <deque>


static std::deque <unsigned char> eventsLocal( 32, 'n' );
static bool eventsIterationFinished {true};

static uint8_t eventCounterLocal {};
static uint8_t eventCounterRemote {};

static bool sentGameParams {};


void
erasePacket(
  Packet& packet )
{
  packet = {};

  std::fill(
    packet.events,
    packet.events + sizeof(packet.events),
    'n' );
}

void
sendDisconnectMessage()
{
  localData.disconnect = true;

  uint8_t packetLocal[sizeof(Packet)];
  memcpy( packetLocal, &localData, sizeof(packetLocal) );

  localData.disconnect = false;

  networkState().connection->SendPacket(
    packetLocal, sizeof(packetLocal) );
}

void
eventPush(
  const EVENTS newEvent )
{
  if (  gameState().gameMode != GAME_MODE::HUMAN_VS_HUMAN ||
        networkState().isOpponentConnected == false )
    return;


  if ( eventsIterationFinished == true )
  {
    eventsLocal.pop_front();
    eventsLocal.push_back(eventCounterLocal);
    eventsIterationFinished = false;

    if ( ++eventCounterLocal >= 64 )
      eventCounterLocal = 0;
  }

  eventsLocal.pop_front();
  eventsLocal.push_back( (unsigned char) newEvent );
  log_message("pushing event '", {(char) newEvent}, "'\n");
}

void
eventsPack()
{
  for ( uint8_t i = 0; i < 32; i++ )
    localData.events[i] = eventsLocal.at(i);
}

void
eventsReset()
{
  eventCounterLocal = 0;
  eventCounterRemote = 0;
  eventsIterationFinished = true;
  sentGameParams = false;

  eventsLocal.clear();
  eventsLocal.resize( 32, 'n' );
}

void
eventsFinishIteration()
{
  eventsIterationFinished = true;
}

void
packLocalData()
{
  localData.throttle = controls_local.throttle;
  localData.pitch    = controls_local.pitch;

  packPlaneCoords();

  if ( sentGameParams == false )
  {
    if ( gameState().isHardcoreEnabled == false )
      eventPush(EVENTS::NO_HARDCORE);

    sentGameParams = true;
  }

  eventsPack();
}

void
packPlaneCoords()
{
  const auto& planeRed = planes.at(PLANE_TYPE::RED);
  const auto& planeBlue = planes.at(PLANE_TYPE::BLUE);


  const PlaneData data =
    planeRed.isLocal() == true
    ? planeRed.getData()
    : planeBlue.getData();

  localData.x       = data.x;
  localData.y       = data.y;
  localData.dir     = data.dir;
  localData.pilot_x = data.pilot_x;
  localData.pilot_y = data.pilot_y;
}

void
processOpponentData()
{
  PlaneData data
  {
    .dir = opponentData.dir,
    .pilot_x = opponentData.pilot_x,
    .pilot_y = opponentData.pilot_y,
  };

  auto& planeRed = planes.at(PLANE_TYPE::RED);
  auto& planeBlue = planes.at(PLANE_TYPE::BLUE);

  auto& planeLocal =
    planeRed.isLocal() == true
    ? planeRed : planeBlue;

  auto& planeRemote =
    planeRed.isLocal() == false
    ? planeRed : planeBlue;

  planeRemote.setDir( data.dir );
  planeRemote.pilot.setX( data.pilot_x );
  planeRemote.pilot.setY( data.pilot_y );


  const bool eventsChanged = std::equal(
    std::begin(opponentData.events),
    std::end(opponentData.events),
    std::begin(opponentDataPrev.events) ) == false;

  if ( eventsChanged == true )
  {
    bool eventNewIteration = true;

    for ( uint8_t i = 0;
          i < sizeof(opponentData.events);
          ++i )
    {
      if ( eventNewIteration == true )
      {
        if ( opponentData.events[i] == eventCounterRemote )
          eventNewIteration = false;

        continue;
      }

      switch ( opponentData.events[i] )
      {
        case (uint8_t) EVENTS::NONE:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": NONE", "\n");
          continue;
        }

        case (uint8_t) EVENTS::NO_HARDCORE:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": NO_HARDCORE", "\n");
          gameState().isHardcoreEnabled = false;
          continue;
        }

        case (uint8_t) EVENTS::SHOOT:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": SHOOT", "\n");
          planeRemote.input.Shoot();
          continue;
        }

        case (uint8_t) EVENTS::EJECT:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": EJECT", "\n");
          planeRemote.input.Jump();
          continue;
        }

        case (uint8_t) EVENTS::HIT_PLANE:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": HIT_PLANE", "\n");
          planeRemote.Hit(planeLocal);
          continue;
        }

        case (uint8_t) EVENTS::HIT_CHUTE:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": HIT_CHUTE", "\n");
          planeRemote.pilot.ChuteHit(planeLocal);
          continue;
        }

        case (uint8_t) EVENTS::HIT_PILOT:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": HIT_PILOT", "\n");
          planeRemote.pilot.Kill(planeLocal);
          continue;
        }

        case (uint8_t) EVENTS::PLANE_DEATH:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": PLANE_DEATH", "\n");
          planeRemote.Crash();
          continue;
        }

        case (uint8_t) EVENTS::PILOT_DEATH:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": PILOT_DEATH", "\n");
          planeRemote.pilot.Death();

          planeRemote.ScoreChange(-1);
          planeRemote.mStats.falls++;
          log_message("processOpponentData() planeRemote.mStats.falls++\n");

          continue;
        }

        case (uint8_t) EVENTS::PLANE_RESPAWN:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": PLANE_RESPAWN", "\n");
          planeRemote.Respawn();
          continue;
        }

        case (uint8_t) EVENTS::PILOT_RESPAWN:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": PILOT_RESPAWN", "\n");
          planeRemote.pilot.Rescue();
          continue;
        }

        case (uint8_t) EVENTS::PILOT_LAND:
        {
          log_message("network event ", std::to_string(eventCounterRemote), ": PILOT_LAND", "\n");
          planeRemote.pilot.FallSurvive();
          continue;
        }

        default:
        {
          if ( ++eventCounterRemote >= 64 )
            eventCounterRemote = 0;

          break;
        }
      }
    }

    if ( eventNewIteration == false )
    {
      const auto& events = opponentData.events;

      std::string str {events, events + sizeof(events)};

      for ( size_t i = 0; i < str.size(); ++i )
      {
        if ( str[i] >= 'A' )
          continue;

        const auto eventId = std::to_string(str[i]);

        str.erase(i, 1);
        str.insert(i, eventId);
        i += eventId.size() - 1;
      }

      log_message("events: '", str, "'\n");
    }

    if ( ++eventCounterRemote >= 64 )
      eventCounterRemote = 0;
  }

  opponentDataPrev = opponentData;

  if ( networkState().sendCoordsTimer.isReady() == false )
    return;


  data.x = opponentData.x;
  data.y = opponentData.y;

  planeRemote.setCoords(data);
}
