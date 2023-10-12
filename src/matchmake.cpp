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

#include <include/matchmake.hpp>
#include <lib/picojson.h>

#include <sstream>
#include <chrono>


MatchMaker& MatchMaker::operator= ( MatchMaker& ) {}
MatchMaker::MatchMaker( const MatchMaker& ) {}
MatchMaker::MatchMaker()
{
  if ( net::InitializeSockets() )
  {
    log_message( "NETWORK MMAKE: Failed to initialize sockets!\n" );
    menu.setMessage( MESSAGE_TYPE::SOCKET_INIT_FAILED );
    return;
  }

  _mmakeServAddr = toAddress( MATCHMAKE_SRV_IP, std::to_string( MATCHMAKE_SRV_PORT ) );
  timer = new Timer( 0.0f );

  srand( time(NULL) );

  m_mm_stream_sock.Open( MATCHMAKE_SOCKET_PORT );
}

void MatchMaker::matchInitForOpponent()
{
  m_client_id = rand() % 1000000;

  connection->Start( _mmakeServAddr.GetPort() );
  connection->Connect( _mmakeServAddr );

  _state = MatchMakerState::FIND_BEGIN;
  menu.setMessage( MESSAGE_TYPE::MMAKE_CONNECTING_TO_SERVER );
  log_message( "NETWORK MMAKE: Connecting to matchmake server", "\n" );
}

void MatchMaker::update()
{
  timer->Update();

  switch ( _state )
  {
    case MatchMakerState::IDLE:
      break;

    case MatchMakerState::FIND_BEGIN:
    {
      if ( !timer->isReady() )
        break;

      // send from mm_socket
      picojson::object msg_mm_socket;
      msg_mm_socket[MATCHMAKE_MSG_TYPE] = picojson::value( (double) MatchConnectStatus::MMSTREAM );
      msg_mm_socket[MATCHMAKE_MSG_CID]  = picojson::value( (double) m_client_id );

      std::string str_msg_mm_socket = picojson::value(msg_mm_socket).serialize();
      m_mm_stream_sock.Send( _mmakeServAddr, str_msg_mm_socket.c_str(), str_msg_mm_socket.size() );
      log_message( "NETWORK MMAKE: Sent status to matchmake server from mm_socket", "\n" );

      timer->SetNewTimeout( 0.6f );
      timer->Start();

      _state = MatchMakerState::FIND_END;

      break;
    }
    case MatchMakerState::FIND_END:
    {
      if ( !timer->isReady() )
        break;

      matchSendStatus( MatchConnectStatus::FIND, _mmakeServAddr );
      log_message( "NETWORK MMAKE: Sent status to matchmake server from game socket", "\n" );
      log_message( "NETWORK MMAKE: Waiting for matchmake server reply...", "\n" );

      timer->SetNewTimeout( 5.0f );
      timer->Start();

      _state = MatchMakerState::MATCH_WAIT;

      break;
    }
    case MatchMakerState::MATCH_WAIT:
    {
      char buf[512];
      net::Address tmp_address;
      int recvd_bytes;

      recvd_bytes = m_mm_stream_sock.Receive( tmp_address, buf, 512 );
      if ( recvd_bytes <= 0 )
      {
        if ( timer->isReady() )
          _state = MatchMakerState::FIND_BEGIN;

        break;
      }

      log_message( "NETWORK MMAKE: Received reply from matchmake server", "\n" );

      std::string str_recvd( buf );
      picojson::value json_recvd;

      std::string parse_errs = picojson::parse( json_recvd, str_recvd );

      if ( !parse_errs.empty() )
      {
        log_message( "NETWORK MMAKE: Failed to parse json from matchmake server:", "\n" );
        log_message( parse_errs.c_str() );
        log_message( "\n", "\n" );
        menu.setMessage( MESSAGE_TYPE::MMAKE_BAD_SERVER_REPLY );
        break;
      }

      bool foundOpponent = false;
      std::string opp_ip = "";
      std::string opp_port = "";
      std::string instanceSrvCli = "";

      const picojson::value::object& reply = json_recvd.get<picojson::object>();
      for ( picojson::value::object::const_iterator param = reply.begin();
            param != reply.end();
            ++param )
      {
        if ( param->first == "type" )
        {
          int msgType = param->second.get <double> ();
          if ( msgType == (int) MatchConnectStatus::MMECHO )
          {
            log_message( "NETWORK MMAKE: Matchmake server echo", "\n", "\n" );
            menu.setMessage( MESSAGE_TYPE::MMAKE_SEARCHING_OPP );
            _state = MatchMakerState::FIND_BEGIN;
            break;
          }
          else if ( msgType == (int) MatchConnectStatus::MMOPPONENT )
          {
            log_message( "NETWORK MMAKE: Found opponent!", "\n" );
            foundOpponent = true;
          }
        }
        if ( param->first == "ip" )
          opp_ip = param->second.to_str();

        if ( param->first == "port" )
          opp_port = param->second.to_str();

        if ( param->first == "cs" )
        {
          if ( param->second.to_str() == "server" )
            _srv_or_cli = false;
          else if ( param->second.to_str() == "client" )
            _srv_or_cli = true;
          else
          {
            log_message( "NETWORK MMAKE: Malformed matchmake server reply!", "\n" );
            log_message( "NETWORK MMAKE: Aborting matchmaking session", "\n" );

            break;
          }

          instanceSrvCli = param->second.to_str();
        }
      }

      if ( !foundOpponent )
        break;

      _opponentAddress = toAddress( opp_ip, opp_port );

      menu.setMessage( MESSAGE_TYPE::P2P_ESTABLISHING );
      log_message( "NETWORK MMAKE: Opponent address: " );
      log_message( opp_ip.c_str(), ":", opp_port.c_str(), "\n" );
      log_message( "NETWORK MMAKE: Game instance is: ", instanceSrvCli, "\n" );

      log_message( "NETWORK MMAKE: Executing NAT hole punching...", "\n" );
      matchSendStatus( MatchConnectStatus::P2PACCEPT, _opponentAddress );

      timer->SetNewTimeout( 0.100f );
      timer->Start();
      _state = MatchMakerState::MATCH_NAT_PUNCH_0;

      break;
    }
    case MatchMakerState::MATCH_NAT_PUNCH_0:
    {
      if ( !timer->isReady() )
        break;

      matchSendStatus( MatchConnectStatus::P2PACCEPT, _opponentAddress );
      if ( _srv_or_cli )
      {
        // wait for opponent's NAT entry
        timer->SetNewTimeout( 5.0f );
        timer->Start();
      }
      _state = MatchMakerState::MATCH_NAT_PUNCH_1;

      break;
    }
    case MatchMakerState::MATCH_NAT_PUNCH_1:
    {
      if ( !timer->isReady() )
        break;

//      send several: first packet could be dropped by NAT
      matchSendStatus( MatchConnectStatus::P2PACCEPT, _opponentAddress );
      matchSendStatus( MatchConnectStatus::P2PACCEPT, _opponentAddress );
      matchSendStatus( MatchConnectStatus::P2PACCEPT, _opponentAddress );

      timer->SetNewTimeout( 0.200f );
      timer->Start();

      _state = MatchMakerState::MATCH_NAT_PUNCH_2;

      break;
    }
    case MatchMakerState::MATCH_NAT_PUNCH_2:
    {
      if ( !timer->isReady() )
        break;

      matchSendStatus( MatchConnectStatus::P2PACCEPT, _opponentAddress );
      matchSendStatus( MatchConnectStatus::P2PACCEPT, _opponentAddress );
      matchSendStatus( MatchConnectStatus::P2PACCEPT, _opponentAddress );

      log_message( "NETWORK MMAKE: Finished NAT hole punching", "\n" );
      log_message( "NETWORK MMAKE: Awaiting opponent reply...", "\n" );
      menu.setMessage( MESSAGE_TYPE::P2P_WAIT_ANSWER );

      timer->SetNewTimeout( MATCH_MAKE_TIMEOUT );
      timer->Start();

      _state = MatchMakerState::MATCH_NAT_PUNCH_3;

      break;
    }
    case MatchMakerState::MATCH_NAT_PUNCH_3:
    {
      char buf[512];
      int recvd_bytes = connection->socket.Receive( _opponentAddress, buf, 512 );
      if ( recvd_bytes > 0 )
      {
        _state = MatchMakerState::MATCH_READY;
        break;
      }

      if ( timer->isReady() )
      {
        _state = MatchMakerState::MATCH_TIMEOUT;
        break;
      }
      break;
    }
    default:
      break;
  }
}

void MatchMaker::Reset()
{
  _state = MatchMakerState::IDLE;
}

MatchMakerState MatchMaker::state()
{
  return _state;
}

bool MatchMaker::srv_or_cli()
{
  return _srv_or_cli;
}

net::Address MatchMaker::opponentAddress()
{
  return _opponentAddress;
}

void MatchMaker::matchSendStatus( MatchConnectStatus mcs, net::Address addr_send )
{
  picojson::object payload_json;

  payload_json[MATCHMAKE_MSG_CID] = picojson::value((double)m_client_id);
  payload_json[MATCHMAKE_MSG_TYPE] = picojson::value((double)mcs);
  if ( mcs == MatchConnectStatus::FIND )
    payload_json[MATCHMAKE_MSG_PASS] = picojson::value( passwd );

  std::string payload_string = picojson::value(payload_json).serialize();

  connection->socket.Send( addr_send, payload_string.c_str(), payload_string.size() );
}


net::Address toAddress( const std::string& inputAddr, const std::string& inputPort )
{
  std::stringstream s( inputAddr );
  int a, b, c, d;
  char ch;
  s >> a >> ch >> b >> ch >> c >> ch >> d;

  return net::Address( a, b, c, d, stoi( inputPort ) );
}
