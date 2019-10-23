#ifndef TCPSOCK_H
#define TCPSOCK_H

#include <string>

#include "../include/variables.h"


class TcpSocket {
  private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;

  public:
    TcpSocket();
    bool connectTo(const std::string& ip_, uint16_t port);
    bool sendString(const std::string& data);
    std::string receiveString(int sizerecv = 512);
};

#endif
