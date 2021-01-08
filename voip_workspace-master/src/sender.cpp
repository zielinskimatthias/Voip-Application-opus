/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "sender.h"

Sender::Sender() : socketAddress(NULL), sentPackets(0) { }

Sender::~Sender() 
{
	delete(socketAddress);
}

void Sender::init(std::string ipAddress, unsigned int port)
{
	socketAddress = new util::Ipv4SocketAddress(ipAddress, port);

	socket.open();
	if (!socket.isOpen()) {
		std::cerr << "Could not open socket..." << std::endl;
	}
}

void Sender::send(std::vector<uint8_t>* data)
{
	// just for information purposes and so that the console does not get spammed. 99 is arbitrary and can be changed as wished.
	if (sentPackets++ % 99 == 0)
	{
		std::cout << "[INFO] Sent " << sentPackets << " packets during this session already. " << std::endl;
	}
	socket.sendto(*socketAddress, *data);
}

void Sender::stop()
{
	socket.close();
}