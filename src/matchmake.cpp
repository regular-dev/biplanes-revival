#include <sstream>

#include "../include/matchmake.hpp"
#include "../include/picojson.h"
#include "../include/tcpsock.hpp"

MatchMaker &MatchMaker::operator=(MatchMaker &) {}
MatchMaker::MatchMaker(const MatchMaker &) {}
MatchMaker::MatchMaker()
{
  if (net::InitializeSockets())
  {
    log_message("NETWORK MMAKE:: Failed to initialize sockets!\n");
    menu.setMessage(MESSAGE_TYPE::SOCKET_INIT_FAILED);
  }
}

void MatchMaker::matchInitForOpponent()
{
  net::Address address = toAddress(MATCHMAKE_SRV_IP, std::to_string(MATCHMAKE_SRC_PORT));
  // MatchMaker SRV IP
  connection->Start(MATCHMAKE_SRC_PORT);
  connection->Connect(address);
  matchSendStatus(MatchConnectStatus::FIND, address);
  // we also need to get agree from server
}

net::Address MatchMaker::matchWaitForOpponent(bool &client_or_srv)
{
  char buf[512];
  net::Address tmp_address;
  int recvd_bytes;
  net::Address badopp(0, 0, 0, 0, 0); // 0.0.0.0:0

  recvd_bytes = connection->socket.Receive(tmp_address, buf, 512);
  if (!(recvd_bytes > 0)) // if received nothing then exit from func
    return badopp;

  std::string str_recvd(buf);
  picojson::value json_recvd;

  std::string parse_errs = picojson::parse(json_recvd, str_recvd);

  if (!parse_errs.empty())
  {
    log_message(parse_errs.c_str());
    return badopp;
  }

  std::string opp_ip = "";
  std::string opp_port = "";
  // just output gathered json
  const picojson::value::object &obj = json_recvd.get<picojson::object>();
  for (picojson::value::object::const_iterator i = obj.begin();
       i != obj.end();
       ++i)
  {
    net::Address opp_ip_port;
    if (i->first == "ip")
    {
      opp_ip = i->second.to_str();
    }
    if (i->first == "port")
    {
      opp_port = i->second.to_str();
    }
    if (i->first == "cs")
    {
      if (i->second.to_str() == "server")
        client_or_srv = false;
      else
        client_or_srv = true;
    }
  }
  log_message("NETWORK MMAKE: Opponent ip: ", opp_ip.c_str(),  "\n");
  log_message("NETWORK MMAKE: Opponent port: ", opp_port.c_str(),  "\n");

  ///////////////////
  // PART TWO P2P ///
  ///////////////////

  matchSendStatus(MatchConnectStatus::P2PACCEPT, toAddress(opp_ip, opp_port));
  int refuse_connect_sec = 0;

  while (1)
  {
    int recvd_bytes = connection->socket.Receive(tmp_address, buf, 512);
    if (recvd_bytes > 0)
      break;
    std::string log_ip =  std::to_string(tmp_address.GetA()) + "." +
              std::to_string(tmp_address.GetB()) + "." +
              std::to_string(tmp_address.GetC()) + "." +
              std::to_string(tmp_address.GetD()) + ":" +
              std::to_string(tmp_address.GetPort()); // ip : port output

    log_message("NETWORK MMAKE: Opponent address: ", log_ip, "\n\n");
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    sleep(1);
#elif PLATFORM == PLATFORM_WINDOWS
    Sleep(1000);
#endif
    refuse_connect_sec++;
    if (refuse_connect_sec >= MATCH_MAKE_TIMEOUT)
    {
      // return second octet 0.1.0.0 2000 if timeout
      return net::Address(0, 1, 0, 0, 0);
    }
  }
  return tmp_address;
}

void MatchMaker::matchSendStatus(MatchConnectStatus mcs, net::Address addr_send)
{
  picojson::object payload_json;
  payload_json[MATCHMAKE_MSG_TYPE] = picojson::value((double)mcs);

  if (mcs == MatchConnectStatus::FIND)
    payload_json[MATCHMAKE_MSG_PASS] = picojson::value(passwd);

  std::string payload_string = picojson::value(payload_json).serialize();

  connection->socket.Send(addr_send, payload_string.c_str(), payload_string.size());
}

net::Address toAddress(std::string inputAddr, std::string inputPort)
{
  std::stringstream s(inputAddr);
  int a, b, c, d;
  char ch;
  s >> a >> ch >> b >> ch >> c >> ch >> d;

  return net::Address(a, b, c, d, stoi(inputPort));
}
