#ifndef H_MATCHMAKE
#define H_MATCHMAKE

#include <string>

#include "Net.h"


#define MATCHMAKE_MSG_TYPE "type"
#define MATCHMAKE_MSG_PASS "matchpass"
#define MATCHMAKE_SRV_IP "217.182.21.102"
#define MATCHMAKE_SRC_PORT 2000
#define MATCH_MAKE_TIMEOUT 3 // 3 sec

enum class MatchConnectStatus
{
  FIND = 1000,
  CONNECTED = 1001,
  CANNOTCONNECT = 1002,
  P2PACCEPT = 1003,
  GOODBYE = 1004
};

net::Address toAddress(std::string inputAddr, std::string inputPort);

class MatchMaker
{
private:
  MatchMaker();
  MatchMaker(const MatchMaker &);
  MatchMaker &operator=(MatchMaker &);

  std::string passwd = "";

public:
  static MatchMaker &Inst()
  {
    static MatchMaker inst;
    return inst;
  }

  inline std::string password() const { return passwd; }
  inline void setPassword(const std::string &s) { passwd = s; }

  // false for server && true for client
  net::Address matchWaitForOpponent(bool &client_or_srv);
  void matchInitForOpponent();
  void matchSendStatus(MatchConnectStatus mcs, net::Address addr_send);
};

#endif
