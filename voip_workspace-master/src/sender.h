/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_SENDER_H
#define VOIP_SENDER_H

#include <iostream>
#include <socket.h>

class Sender 
{
public:
	Sender();
	~Sender();
	void init(std::string ipAddress, unsigned int port);
	void send(std::vector<uint8_t>* data);
	void stop();
private:
	util::Ipv4SocketAddress* socketAddress;
	util::UdpSocket socket;
	uint32_t sentPackets;
};

#endif /* VOIP_SENDER_H */
