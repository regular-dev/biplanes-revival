#include <string.h>
#include <iostream>

#include "../include/tcpsock.hpp"
#include "../include/utility.h"
#include "../include/Net.h"


TcpSocket::TcpSocket() {
    sock = -1;
    port = 0;
    address = "";
    net::InitializeSockets();
}

bool TcpSocket::connectTo(const std::string& ip_, uint16_t port_) {
    if (sock <= 0) {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock <= 0) {
            log_message( "TCPSOCK: Could not create socket\n" );
        }
        else
          log_message( "TCPSOCK: Socket created\n" );
    }
  address = ip_;
    if (inet_addr(address.c_str()) == -1) {
        struct hostent* he;
        struct in_addr** addr_list;

        if ((he = gethostbyname(address.c_str())) == NULL) {
            log_message( "TCPSOCK: Failed to resolve hostname\n" );
            return false;
        }
        addr_list = (struct in_addr**) he->h_addr_list;
        for (int i = 0; addr_list[i] != NULL; i++) {
            server.sin_addr = *addr_list[i];
            std::cout << address << " resolved to " << inet_ntoa(*addr_list[i]) << "\n";
            log_message( address.c_str(), " resolved to ", inet_ntoa(*addr_list[i]), "\n" );
            break;
        }
    }
    server.sin_addr.s_addr = inet_addr(address.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port_);
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        log_message( "TCPSOCK: Connection failed. Error\n" );
        #if PLATFORM == PLATFORM_WINDOWS
          log_message( std::to_string( WSAGetLastError() ).c_str() );
        #endif
        return 1;
    }

    log_message( "TCPSOCK: Connected\n" );
    return true;
}

bool TcpSocket::sendString(const std::string& data) {
    if (send(sock, data.c_str(), strlen(data.c_str()), 0) < 0) {
        log_message( "TCPSOCK: String send failed : \n" );
        return false;
    }
    log_message( "TCPSOCK: String sent\n" );
    return true;
}

std::string TcpSocket::receiveString(int sizerecv) {
    char buffer[sizerecv];
    std::string reply;
    if (recv(sock, buffer, sizeof(buffer), 0) < 0) {
        log_message( "TCPSOCK: recv failed() : \n" );
    }

    reply = buffer;
    return reply;
}
