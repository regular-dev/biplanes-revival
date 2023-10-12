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

#include <lib/Net.h>

#include <string>


#define MATCHMAKE_SOCKET_PORT 50005
#define MATCHMAKE_MSG_TYPE "type"
#define MATCHMAKE_MSG_PASS "matchpass"
#define MATCHMAKE_MSG_CID  "client_id"
#define MATCHMAKE_SRV_IP "217.182.21.102"
#define MATCHMAKE_SRV_PORT 2000
#define MATCH_MAKE_TIMEOUT 15


enum class MatchConnectStatus
{
  FIND = 1000,
  CONNECTED = 1001,
  CANNOTCONNECT = 1002,
  P2PACCEPT = 1003,
  GOODBYE = 1004,
  MMSTREAM = 1005,
  MMECHO = 1006,
  MMOPPONENT = 1007
};

enum class MatchMakerState
{
  IDLE,
  FIND_BEGIN,
  FIND_END,
  MATCH_WAIT,
  MATCH_NAT_PUNCH_0,
  MATCH_NAT_PUNCH_1,
  MATCH_NAT_PUNCH_2,
  MATCH_NAT_PUNCH_3,
  MATCH_READY,
  MATCH_TIMEOUT
};

net::Address toAddress( const std::string& inputAddr, const std::string& inputPort);

class MatchMaker
{
private:
  MatchMaker();
  MatchMaker( const MatchMaker& );
  MatchMaker& operator= ( MatchMaker& );

  std::string passwd = "";
  net::Socket m_mm_stream_sock;
  int m_client_id;

  MatchMakerState _state;
  Timer* timer;

  net::Address _mmakeServAddr;
  net::Address _opponentAddress;
  bool _srv_or_cli;

public:
  static MatchMaker& Inst()
  {
    static MatchMaker inst;
    return inst;
  }

  inline std::string password() const { return passwd; }
  inline void setPassword( const std::string& s ) { passwd = s; }

  void matchInitForOpponent();
  void matchSendStatus( MatchConnectStatus mcs, net::Address addr_send );

  void Reset();

  void update();
  MatchMakerState state();
  bool srv_or_cli();
  net::Address opponentAddress();
};
