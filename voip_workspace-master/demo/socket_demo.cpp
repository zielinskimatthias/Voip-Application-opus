/******************************************************************************/
/*
 *  Project: Voice-over-IP
 *  Author: Matthias Tessmann <matthias.tessmann@th-nuernberg.de
 *  Date: October, 5th 2015
 *
 *  Contents: Simple socket demo.
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

#include "socket.h"
#include "tclap/CmdLine.h"

#include <string.h>

int main(int argc, char *argv[]) {

  /* Set up command line arguments */
  try {

    TCLAP::CmdLine cmd("Socket demo", ' ', "0.1");
    TCLAP::ValueArg<unsigned int> port("", "port", "Local Port (default: 1976)", false, 1976, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> rport("", "rport", "Remote Port (default: 1976)", false, 1976, "unsigned int", cmd);

    TCLAP::UnlabeledValueArg<std::string> destIp("destIp", "Destination IP address", false, "", "std::string", cmd);
    TCLAP::SwitchArg receive("r", "receive", "Act as receiver.", cmd);

    cmd.parse(argc, argv);

    if (!receive.getValue()) {
      if (destIp.getValue() == "") {
        TCLAP::StdOutput().usage(cmd);
        exit(-1);
      }

      util::Ipv4SocketAddress raddr(destIp.getValue(), rport.getValue());

      util::UdpSocket s;
      s.open();

      if (s.isOpen()) {
        std::cerr << "Socket is open. Sending 'hello' to " << raddr.toString(true) << std::endl;
      }

      const char* hellostr = "hello";
      std::vector<uint8_t> data(hellostr, hellostr + strlen(hellostr));
      uint32_t send_cnt = 0, i = 0;

      while (true) {
        send_cnt = s.sendto(raddr, data);
        std::cerr << "Sent " << send_cnt << " bytes." << std::endl;
      }
      s.close();

    } else {
      util::Ipv4SocketAddress addr("", port.getValue());
      util::UdpSocket s;
      s.open();

      if (s.isOpen()) {
        std::cerr << "Socket is open. Listening... " << std::endl;
      }

      if (!s.bind(addr)) {
        std::cerr << "Error binding socket!" << std::endl;
        s.close();
        exit(-1);
      }

      while (true) {
        std::vector<uint8_t> data(128, 0);
        util::Ipv4SocketAddress from;
        s.recvfrom(from, data, 128);

        std::cerr << "Received " << data.size() << " bytes from " << from.toString(true) << std::endl;
        std::string msg(reinterpret_cast<const char*>(&data[0]), data.size());
        std::cerr << "Message: " << msg << std::endl;
      }
      s.close();

    }
  } catch (TCLAP::ArgException& argEx) {
    std::cerr << "Error parsing command line arguments: " << argEx.error() << " for argument " << argEx.argId() << std::endl;
  }

  return 0;
}
