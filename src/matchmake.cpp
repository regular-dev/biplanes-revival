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

#include <include/matchmake.hpp>
#include <include/network_state.hpp>
#include <include/menu.hpp>
#include <include/variables.hpp>

#include <lib/picojson.h>


net::Address MatchMaker::GetServerAddress()
{
  static auto mmakeAddr = net::Address::ResolveHostname(
    MATCHMAKE_SRV_HOSTNAME);

  if ( mmakeAddr.GetAddress() == 0 )
  {
    mmakeAddr = net::Address::FromString(
      MATCHMAKE_SRV_IP,
      std::to_string(MATCHMAKE_SRV_PORT) );
  }
  else
    mmakeAddr = {mmakeAddr.GetAddress(), MATCHMAKE_SRV_PORT};

  return mmakeAddr;
}


bool
MatchMaker::initNewSession()
{
  if ( mSocket.IsOpen() == false )
  {
    if ( net::InitializeSockets() != 0 )
    {
      log_message( "NETWORK MMAKE: Failed to initialize sockets!\n" );
      menu.setMessage(MESSAGE_TYPE::SOCKET_INIT_FAILED);

      return false;
    }

    srand( time(nullptr) );

    if ( mSocket.Open(MATCHMAKE_SOCKET_PORT) == false )
    {
      log_message( "NETWORK MMAKE: Failed to initialize socket for matchmaking!\n" );
      menu.setMessage(MESSAGE_TYPE::CANT_START_CONNECTION);

      return false;
    }
  }

  mClientId = rand() % 1'000'000;


  auto connection = networkState().connection;

  const auto serverAddress = GetServerAddress();

  if ( connection->Start( serverAddress.GetPort() ) == false )
  {
    log_message( "NETWORK MMAKE: Failed to initialize matchmaking session", "\n" );
    menu.setMessage(MESSAGE_TYPE::CANT_START_CONNECTION);
    return false;
  }

//  TODO: remove this ?
//  connection->Connect(serverAddress);

  mState = MatchMakerState::FIND_BEGIN;
  menu.setMessage(MESSAGE_TYPE::MMAKE_CONNECTING_TO_SERVER);
  log_message( "NETWORK MMAKE: Connecting to matchmake server", "\n" );

  return true;
}

void
MatchMaker::sendStatus(
  const MatchConnectStatus status,
  const net::Address destination )
{
  picojson::object payload_json {};

  payload_json[MATCHMAKE_MSG_CID] = picojson::value{(double) mClientId};
  payload_json[MATCHMAKE_MSG_TYPE] = picojson::value{(double) status};

  if ( status == MatchConnectStatus::FIND )
    payload_json[MATCHMAKE_MSG_PASS] = picojson::value{mPassword};

  const auto payload_string = picojson::value(payload_json).serialize();

  networkState().connection->socket.Send(
    destination,
    payload_string.c_str(),
    payload_string.size() );
}

void
MatchMaker::Update()
{
  mTimer.Update();

  switch (mState)
  {
    case MatchMakerState::IDLE:
      break;

    case MatchMakerState::FIND_BEGIN:
    {
      if ( mTimer.isReady() == false )
        break;

      picojson::object msg_mm_socket {};
      msg_mm_socket[MATCHMAKE_MSG_TYPE] = picojson::value( (double) MatchConnectStatus::MMSTREAM );
      msg_mm_socket[MATCHMAKE_MSG_CID]  = picojson::value( (double) mClientId );

      std::string str_msg_mm_socket = picojson::value(msg_mm_socket).serialize();

      mSocket.Send(
        GetServerAddress(),
        str_msg_mm_socket.c_str(),
        str_msg_mm_socket.size() );

      log_message( "NETWORK MMAKE: Sent MMSTREAM status to matchmake server", "\n" );

      mTimer.SetNewTimeout(0.6f);
      mTimer.Start();

      mState = MatchMakerState::FIND_END;

      break;
    }

    case MatchMakerState::FIND_END:
    {
      if ( mTimer.isReady() == false )
        break;

      sendStatus( MatchConnectStatus::FIND, GetServerAddress() );
      log_message( "NETWORK MMAKE: Sent FIND status to matchmake server", "\n" );
      log_message( "NETWORK MMAKE: Waiting for matchmake server reply...", "\n" );

      mTimer.SetNewTimeout(5.0f);
      mTimer.Start();

      mState = MatchMakerState::MATCH_WAIT;

      break;
    }

    case MatchMakerState::MATCH_WAIT:
    {
      char buf[512] {};
      net::Address tmp_address {};
      int recvd_bytes {};

      recvd_bytes = mSocket.Receive(
        tmp_address, buf, 512 );

      if ( recvd_bytes <= 0 )
      {
        if ( mTimer.isReady() == true )
          mState = MatchMakerState::FIND_BEGIN;

        break;
      }

      log_message( "NETWORK MMAKE: Received reply from matchmake server", "\n" );

      std::string str_recvd{buf};
      picojson::value json_recvd {};

      std::string parse_errs = picojson::parse(
        json_recvd, str_recvd );

      if ( parse_errs.empty() == false )
      {
        log_message( "NETWORK MMAKE: Failed to parse json from matchmake server:", "\n" );
        log_message( parse_errs.c_str() );
        log_message( "\n", "\n" );
        menu.setMessage(MESSAGE_TYPE::MMAKE_BAD_SERVER_REPLY);
        break;
      }

      bool foundOpponent = false;
      std::string opp_ip {};
      std::string opp_port {};
      std::string instanceSrvCli {};

      const picojson::value::object& reply = json_recvd.get<picojson::object>();
      for ( picojson::value::object::const_iterator param = reply.begin();
            param != reply.end();
            ++param )
      {
        if ( param->first == "type" )
        {
          const auto msgType = static_cast <MatchConnectStatus> (param->second.get <double> ());

          if ( msgType == MatchConnectStatus::MMECHO )
          {
            log_message( "NETWORK MMAKE: Matchmake server echo", "\n", "\n" );
            menu.setMessage(MESSAGE_TYPE::MMAKE_SEARCHING_OPPONENT);
            mState = MatchMakerState::FIND_BEGIN;

            break;
          }

          else if ( msgType == MatchConnectStatus::MMOPPONENT )
          {
            log_message( "NETWORK MMAKE: Found opponent!", "\n" );
            foundOpponent = true;
          }
          else
          {
            log_message( "NETWORK MMAKE: Received unknown message type from matchmake server!", "\n" );
            log_message( "NETWORK MMAKE: Aborting matchmaking session", "\n" );

            foundOpponent = false;
            break;
          }
        }

        if ( param->first == "ip" )
          opp_ip = param->second.to_str();

        if ( param->first == "port" )
          opp_port = param->second.to_str();

        if ( param->first == "cs" )
        {
          if ( param->second.to_str() == "server" )
            mClientNodeType = SRV_CLI::SERVER;

          else if ( param->second.to_str() == "client" )
            mClientNodeType = SRV_CLI::CLIENT;

          else
          {
            log_message( "NETWORK MMAKE: Malformed matchmake server reply!", "\n" );
            log_message( "NETWORK MMAKE: Aborting matchmaking session", "\n" );

            foundOpponent = false;
            break;
          }

          instanceSrvCli = param->second.to_str();
        }
      }

      if ( foundOpponent == false )
        break;

      mOpponentAddress = net::Address::FromString(opp_ip, opp_port);

      menu.setMessage(MESSAGE_TYPE::P2P_ESTABLISHING);
      log_message( "NETWORK MMAKE: Opponent address: " );
      log_message( opp_ip.c_str(), ":", opp_port.c_str(), "\n" );
      log_message( "NETWORK MMAKE: Game instance is: ", instanceSrvCli, "\n" );

      log_message( "NETWORK MMAKE: Executing NAT hole punching...", "\n" );
      sendStatus( MatchConnectStatus::P2PACCEPT, mOpponentAddress );

      mTimer.SetNewTimeout(0.100f);
      mTimer.Start();
      mState = MatchMakerState::MATCH_NAT_PUNCH_0;
      mSocket.Close();

      break;
    }

    case MatchMakerState::MATCH_NAT_PUNCH_0:
    {
      if ( mTimer.isReady() == false )
        break;

      sendStatus( MatchConnectStatus::P2PACCEPT, mOpponentAddress );

      if ( mClientNodeType == SRV_CLI::CLIENT )
      {
//        wait for opponent's NAT entry
        mTimer.SetNewTimeout(5.0f);
        mTimer.Start();
      }

      mState = MatchMakerState::MATCH_NAT_PUNCH_1;

      break;
    }

    case MatchMakerState::MATCH_NAT_PUNCH_1:
    {
      if ( mTimer.isReady() == false )
        break;

//      send several: first packet could be dropped by NAT
      sendStatus( MatchConnectStatus::P2PACCEPT, mOpponentAddress );
      sendStatus( MatchConnectStatus::P2PACCEPT, mOpponentAddress );
      sendStatus( MatchConnectStatus::P2PACCEPT, mOpponentAddress );

      mTimer.SetNewTimeout(0.200f);
      mTimer.Start();

      mState = MatchMakerState::MATCH_NAT_PUNCH_2;

      break;
    }

    case MatchMakerState::MATCH_NAT_PUNCH_2:
    {
      if ( mTimer.isReady() == false )
        break;

      sendStatus( MatchConnectStatus::P2PACCEPT, mOpponentAddress );
      sendStatus( MatchConnectStatus::P2PACCEPT, mOpponentAddress );
      sendStatus( MatchConnectStatus::P2PACCEPT, mOpponentAddress );

      log_message( "NETWORK MMAKE: Finished NAT hole punching", "\n" );
      log_message( "NETWORK MMAKE: Awaiting opponent reply...", "\n" );
      menu.setMessage(MESSAGE_TYPE::P2P_WAIT_ANSWER);

      mTimer.SetNewTimeout(MATCH_MAKE_TIMEOUT);
      mTimer.Start();

      mState = MatchMakerState::MATCH_NAT_PUNCH_3;

      break;
    }

    case MatchMakerState::MATCH_NAT_PUNCH_3:
    {
      char buf[512] {};

      int recvd_bytes = networkState().connection->socket.Receive(
        mOpponentAddress, buf, 512 );

      if ( recvd_bytes > 0 )
      {
        mState = MatchMakerState::MATCH_READY;
        break;
      }

      if ( mTimer.isReady() == true )
      {
        mState = MatchMakerState::MATCH_TIMEOUT;
        break;
      }
      break;
    }

    default:
      break;
  }
}

void
MatchMaker::Reset()
{
  mSocket.Close();
  mState = MatchMakerState::IDLE;
}

MatchMakerState
MatchMaker::state()
{
  return mState;
}

SRV_CLI
MatchMaker::clientNodeType()
{
  return mClientNodeType;
}

net::Address
MatchMaker::opponentAddress()
{
  return mOpponentAddress;
}

std::string
MatchMaker::password() const
{
  return mPassword;
}

void
MatchMaker::setPassword(
  const std::string& newPassword )
{
  mPassword = newPassword;
}
