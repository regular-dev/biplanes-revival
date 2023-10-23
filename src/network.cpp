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
#include <include/sizes.hpp>
#include <include/variables.hpp>

#include <lib/Net.h>

#include <deque>


static std::deque <unsigned char> eventsLocal( 32, 'n' );
static bool eventsIterationFinished {true};

static uint8_t eventCounterLoc {};
static uint8_t eventCounterOpp {};

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
    eventsLocal.push_back(eventCounterLoc);
    eventsIterationFinished = false;

    if ( ++eventCounterLoc > 63 )
      eventCounterLoc = 0;
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
  eventCounterLoc = 0;
  eventCounterOpp = 0;
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

  localData.x       = data.x / sizes.screen_width;
  localData.y       = data.y / sizes.screen_height;
  localData.dir     = data.dir;
  localData.pilot_x = data.pilot_x / sizes.screen_width;
  localData.pilot_y = data.pilot_y / sizes.screen_height;
}

void
processOpponentData()
{
  PlaneData data
  {
    .dir = opponentData.dir,
    .pilot_x = opponentData.pilot_x * sizes.screen_width,
    .pilot_y = opponentData.pilot_y * sizes.screen_height,
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
        if ( opponentData.events[i] == eventCounterOpp )
          eventNewIteration = false;

        continue;
      }

      switch ( opponentData.events[i] )
      {
        case (uint8_t) EVENTS::NONE:
          continue;

        case (uint8_t) EVENTS::NO_HARDCORE:
        {
          gameState().isHardcoreEnabled = false;
          continue;
        }

        case (uint8_t) EVENTS::SHOOT:
        {
          planeRemote.input.Shoot();
          continue;
        }

        case (uint8_t) EVENTS::EJECT:
        {
          planeRemote.input.Jump();
          continue;
        }

        case (uint8_t) EVENTS::HIT_PLANE:
        {
          planeRemote.Hit(planeLocal);
          continue;
        }

        case (uint8_t) EVENTS::HIT_CHUTE:
        {
          planeRemote.pilot.ChuteHit(planeLocal);
          continue;
        }

        case (uint8_t) EVENTS::HIT_PILOT:
        {
          planeRemote.pilot.Kill(planeLocal);
          continue;
        }

        case (uint8_t) EVENTS::PLANE_DEATH:
        {
          planeRemote.Crash();
          continue;
        }

        case (uint8_t) EVENTS::PILOT_DEATH:
        {
          planeRemote.pilot.Death();
          planeRemote.ScoreChange(-1);
          planeRemote.mStats.falls++;
          log_message("processOpponentData() planeRemote.mStats.falls++\n");

          continue;
        }

        case (uint8_t) EVENTS::PLANE_RESPAWN:
        {
          planeRemote.Respawn();
          continue;
        }

        case (uint8_t) EVENTS::PILOT_RESPAWN:
        {
          planeRemote.pilot.Rescue();
          continue;
        }

        case (uint8_t) EVENTS::PILOT_LAND:
        {
          planeRemote.pilot.FallSurvive();
          continue;
        }

        default:
        {
          if ( eventCounterOpp < 63 )
            ++eventCounterOpp;
          else
            eventCounterOpp = 0;

          break;
        }
      }
    }

    if ( eventCounterOpp < 63 )
      ++eventCounterOpp;
    else
      eventCounterOpp = 0;
  }

  opponentDataPrev = opponentData;

  if ( networkState().sendCoordsTimer.isReady() == false )
    return;


  data.x = opponentData.x * sizes.screen_width;
  data.y = opponentData.y * sizes.screen_height;

  planeRemote.setCoords(data);
}
