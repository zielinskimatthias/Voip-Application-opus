/******************************************************************************/
/*
 *  Project: Voice-over-IP
 *  Author: Matthias Tessmann <matthias.tessmann@th-nuernberg.de
 *  Date: October, 5th 2015
 *
 *  Contents: Very simple socket wrapper (UDP & IPv4 only).
 *            Intended for educational purposes. BSD implementation.
 *
 *  Additoinal notes:
 *         - There is very little error handling.
 *         - The implementation may contain bugs of any kind!
 *
 * The MIT License (MIT)
 *
 *  Copyright (c) 2015 Matthias Teßmann <matthias.tessmann@th-nuernberg.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
 /******************************************************************************/

#include "socket.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define NOMINMAX 1

#include <winsock2.h>
#include <ws2tcpip.h>

namespace util {

/* Startup Windows sockets */
static inline int32_t InitWinsock() {
	WSADATA data;
  return WSAStartup(MAKEWORD(2,0), &data);
}

/******************************************************************************/
/* class SocketAddress */
/******************************************************************************/
struct Ipv4SocketAddress::Ipv4SocketAddressImpl {
  Ipv4SocketAddressImpl() : ipv4address(), port(0), valid(false) {
    ::memset(&ipv4address, 0, sizeof(ipv4address));
  };
  ~Ipv4SocketAddressImpl() {};

  void reset() {
    ::memset(&ipv4address, 0, sizeof(ipv4address));
    port  = 0;
    valid = false;
  }

  sockaddr_in ipv4address;
  uint16_t    port;
  bool        valid;
};

/******************************************************************************/
/* class SocketAddress - public interface */
/******************************************************************************/
Ipv4SocketAddress::Ipv4SocketAddress(uint16_t p)
  : pImpl_(new Ipv4SocketAddress::Ipv4SocketAddressImpl())
{
  if (InitWinsock() != 0) {
    std::cerr << "Error initializing Windows sockets!" << std::endl;
  }

  pImpl_->port                 = p;
  pImpl_->ipv4address.sin_port = htons(p);
  setAddress("");
}

Ipv4SocketAddress::Ipv4SocketAddress(std::string const& addr, uint16_t p)
  : pImpl_(new Ipv4SocketAddress::Ipv4SocketAddressImpl)
{
  if (InitWinsock() != 0) {
    std::cerr << "Error initializing Windows sockets!" << std::endl;
  }
  pImpl_->port                 = p;
  pImpl_->ipv4address.sin_port = htons(p);
  setAddress(addr);
}

Ipv4SocketAddress::Ipv4SocketAddress(Ipv4SocketAddress const& rhs)
  : pImpl_(new Ipv4SocketAddress::Ipv4SocketAddressImpl)
{
  if (InitWinsock() != 0) {
    std::cerr << "Error initializing Windows sockets!" << std::endl;
  }

  *pImpl_ = *(rhs.pImpl_);
}

Ipv4SocketAddress& Ipv4SocketAddress::operator=(Ipv4SocketAddress const& rhs) {
  if (this != &rhs) { // self assignment check
    if (pImpl_)
      delete pImpl_;
    pImpl_ = new Ipv4SocketAddress::Ipv4SocketAddressImpl;
    *pImpl_ = *(rhs.pImpl_);
  }
  return *this;
}

Ipv4SocketAddress::~Ipv4SocketAddress() {
  if (pImpl_)
    delete pImpl_;
}

std::string Ipv4SocketAddress::toString(bool wPort) const {
  std::stringstream r;
  r << address();
  if (wPort) r << ":" << port();
  return r.str();
}

uint16_t Ipv4SocketAddress::port() const {
  return pImpl_->port;
}

void Ipv4SocketAddress::setPort(uint16_t p) {
  pImpl_->port                 = p;
  pImpl_->ipv4address.sin_port = htons(p);
}

std::string Ipv4SocketAddress::address() const {
  char name[128];
  if (isWildcardAdress()) {
    memcpy(name, "0.0.0.0", sizeof("0.0.0.0"));
  } else {
    DWORD name_len = sizeof(name);
    WSAAddressToString((LPSOCKADDR) &pImpl_->ipv4address, sizeof(pImpl_->ipv4address), NULL, name, &name_len);
    name[name_len] = '\0';
  }
  return std::string(name);
}

void Ipv4SocketAddress::setAddress(std::string const& addr) {
  if (addr.empty()) {
    pImpl_->ipv4address.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    struct addrinfo addrhints;
    struct addrinfo *res;
    int err;
    memset(&addrhints, 0, sizeof(addrhints));

    if ((err = getaddrinfo(addr.c_str(), NULL, &addrhints, &res)) != 0) {
      std::cerr << "SocketAddress::setAddress(): Error in socket address translation for " << addr << ": " <<  gai_strerror(err) << std::endl;
    } else {
      if (res->ai_canonname)
        std::cerr << "Canonical name of socket: " << res->ai_canonname << std::endl;

      switch (res->ai_family) {
        case AF_INET:
          memcpy(&pImpl_->ipv4address, res->ai_addr, sizeof(pImpl_->ipv4address));
          setPort(port()); // Fix port in sockaddr
        break;
        default:
          std::cerr << "SocketAddress::setAddress(): Socket addresses other than ipv4 not supported!" << std::endl;
        break;
      }
      freeaddrinfo(res);
    }
  }
}

bool Ipv4SocketAddress::isWildcardAdress() const {
  return (pImpl_->ipv4address.sin_addr.s_addr == htonl(INADDR_ANY));
}

/******************************************************************************/
/* class UdpSocket */
/******************************************************************************/
class UdpSocket::UdpSocketImpl {
public:
  UdpSocketImpl() : sockfd(0), isOpen(false) {};
  ~UdpSocketImpl() {};

  int  sockfd;
  bool isOpen;
};

UdpSocket::UdpSocket() : pImpl_(new UdpSocket::UdpSocketImpl()) {
  if (InitWinsock() != 0) {
    std::cerr << "Error initializing Windows sockets!" << std::endl;
  }
}

UdpSocket::~UdpSocket() {
  if (pImpl_)
    delete pImpl_;
}

bool UdpSocket::open() {
  if (isOpen())
    close();

  uint32_t family = AF_INET;
  uint32_t type   = SOCK_DGRAM;
  uint32_t proto  = IPPROTO_UDP;

  pImpl_->sockfd = socket(family, type, proto);

  if (pImpl_->sockfd == INVALID_SOCKET) {
    int e = WSAGetLastError();
    std::cerr << "Error opening socket: " << e << std::endl;
    return false;
  }

  pImpl_->isOpen = true;

  int32_t reuseaddr = 1;
  if (::setsockopt(pImpl_->sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseaddr, sizeof(reuseaddr))) {
    std::cerr << "Error setting reuse option on socket!" << std::endl;
  }

  return true;
}

bool UdpSocket::isOpen() {
  return pImpl_->isOpen;
}

bool UdpSocket::bind(Ipv4SocketAddress const& addr) {
  if (!isOpen()) {
    std::cerr << "Error: trying to bind a non open socket!" << std::endl;
    return false;
  }

  struct sockaddr* a = NULL;
  socklen_t    a_len = 0;

  a = (struct sockaddr*) &addr.pImpl_->ipv4address;
  ((sockaddr_in*)a)->sin_family = AF_INET;
  a_len = sizeof(sockaddr_in);

  if (::bind(pImpl_->sockfd, a, a_len) != 0) {
    int e = WSAGetLastError();
    std::cerr << "Error binding socket: " << e << std::endl;
    return false;
  }

  return true;
}

uint32_t UdpSocket::sendto(Ipv4SocketAddress const& addr, std::vector<uint8_t> const& data, bool bBlock) {
  if (!isOpen())
    return 0;

  struct sockaddr* a = NULL;
  socklen_t    a_len = 0;
  a     = (struct sockaddr*) &addr.pImpl_->ipv4address;
  a_len = sizeof(sockaddr_in);

  u_long nonblocking = 0;
  if (!bBlock) {
    nonblocking = 1;
    if (ioctlsocket(pImpl_->sockfd, FIONBIO, &nonblocking) != 0) {
      std::cerr << "Could not set non-blocking mode on socket!" << std::endl;
    }
  }

  int32_t flags = 0;

  int32_t bytes_sent = ::sendto(pImpl_->sockfd, (char*)&data[0], data.size(), flags, a, a_len);
  if (bytes_sent == -1) {
    int e = WSAGetLastError();
    std::cerr << "Error in sendto: " << e << std::endl;
    return 0;
  }

  if (!bBlock) {
    nonblocking = 0;
    if (ioctlsocket(pImpl_->sockfd, FIONBIO, &nonblocking) != 0) {
      std::cerr << "Could not set blocking mode on socket!" << std::endl;
    }
  }

  return bytes_sent;
}

uint32_t UdpSocket::recvfrom(Ipv4SocketAddress& addr, std::vector<uint8_t>& data, uint32_t maxSize, bool bBlock, bool bPeek, bool bWaitAll) {
  if (!isOpen())
    return 0;

  // helper variables
  struct sockaddr*  a     = NULL;
  socklen_t         a_len;
  u_short*          port = NULL;

  a     = (struct sockaddr*) &addr.pImpl_->ipv4address;
  addr.pImpl_->ipv4address.sin_family = AF_INET;
  a_len = sizeof(sockaddr_in);
  port     = &addr.pImpl_->ipv4address.sin_port;

  int32_t flags = 0;

  u_long nonblocking = 0;
  if (!bBlock) {
    nonblocking = 1;
    if (ioctlsocket(pImpl_->sockfd, FIONBIO, &nonblocking) != 0) {
      std::cerr << "Could not set non-blocking mode on socket!" << std::endl;
    }
  }

  if (bPeek)
    flags |= MSG_PEEK;

  uint32_t toread = maxSize ? maxSize : data.capacity();
  if (maxSize > data.capacity()) {
    std::cerr << "Data to read over buffer capacity!" << std::endl;
    return 0;
  }

  int32_t read_bytes = ::recvfrom(pImpl_->sockfd, (char*)&data[0], toread, flags, a, &a_len);
  int32_t e = WSAGetLastError();
  if (read_bytes == -1 || read_bytes == 0) {
    std::cerr << "Error in recvfrom: " << e << std::endl;
    data.clear();
    return 0;
  }

  if (!bBlock) {
    nonblocking = 0;
    if (ioctlsocket(pImpl_->sockfd, FIONBIO, &nonblocking) != 0) {
      std::cerr << "Could not set blocking mode on socket!" << std::endl;
    }
  }

  addr.setPort(ntohs(*port));

  data.resize(std::max(read_bytes, 0));

  return read_bytes;
}

bool UdpSocket::close() {
  if (!isOpen())
    return false;

  if (::closesocket(pImpl_->sockfd) == -1) {
    int e = WSAGetLastError();
    std::cerr << "Error in closing socket: " << e << std::endl;
  }
  pImpl_->isOpen = false;
  return true;
}

}
