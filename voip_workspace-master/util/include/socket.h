/******************************************************************************/
/*
 *  Project: Voice-over-IP
 *  Author: Matthias Tessmann <matthias.tessmann@th-nuernberg.de
 *  Date: October, 5th 2015
 *
 *  Contents: Very simple socket wrapper (UDP & IPv4 only).
 *            Intended for educational purposes.
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

#ifndef VOIP_UTIL_SOCKET_H
#define VOIP_UTIL_SOCKET_H

#include <string>
#include <vector>
#include <stdint.h>

namespace util {

/******************************************************************************/
/* class Ipv4SocketAddress */
/******************************************************************************/
class Ipv4SocketAddress {
public:
  explicit Ipv4SocketAddress(uint16_t port = 0);
  Ipv4SocketAddress(std::string const& addr, uint16_t port = 0);
  Ipv4SocketAddress(Ipv4SocketAddress const& rhs);
  ~Ipv4SocketAddress();

  Ipv4SocketAddress& operator=(Ipv4SocketAddress const& rhs);

  std::string   toString(bool wPort = false) const;

  uint16_t      port() const;
  void          setPort(uint16_t port);
  std::string   address() const;
  void          setAddress(std::string const& addr);
  bool          isValid() const;
  bool          isWildcardAdress() const;

  friend class UdpSocket;

private:
  struct Ipv4SocketAddressImpl;
  Ipv4SocketAddressImpl *pImpl_;
};

/******************************************************************************/
/* Class UdpSocket */
/******************************************************************************/
class UdpSocket {
public:
  UdpSocket();
  UdpSocket(UdpSocket const&)            = delete;
  UdpSocket(UdpSocket&&)                 = delete;
  UdpSocket& operator=(UdpSocket const&) = delete;
  UdpSocket& operator=(UdpSocket&&)      = delete;
  ~UdpSocket();

  bool open();
  bool isOpen();
  bool bind(Ipv4SocketAddress const& addr);

  uint32_t sendto(Ipv4SocketAddress const& addr, std::vector<uint8_t> const& data, bool bBlock = true);
  uint32_t recvfrom(Ipv4SocketAddress& addr, std::vector<uint8_t>& data, uint32_t maxSize = 0, bool bBlock = true, bool bPeek = false, bool bWaitAll = false);

  bool close();

private:
  class UdpSocketImpl;
  UdpSocketImpl *pImpl_;
};

}

#endif /* VOIP_UTIL_SOCKET_H */
