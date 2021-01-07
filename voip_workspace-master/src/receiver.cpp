/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "receiver.h"

Receiver::Receiver() : self_(), running_(true), port(), jitterBuffer(), decoder() { }

Receiver::~Receiver() {
	if (running_) {
		stop();
	}
}

void Receiver::init(uint32_t port, SimpleJB* jitterBuffer, uint8_t payloadType)
{
	if (payloadType == 100)
	{
		this->decoder.init(48000, 480, 2);
	}
	this->payloadType = payloadType;
	this->port = port;
	this->jitterBuffer = jitterBuffer;
}

void Receiver::start() {
	self_ = std::thread([=] { receive(); });
}

void Receiver::stop() {
	running_ = false;
	self_.join();
	if(socket.isOpen()) { socket.close(); }
}

void Receiver::receive() {
	util::Ipv4SocketAddress socketAddress("", port);
	util::UdpSocket socket;

	socket.open();

	if (socket.isOpen()) {
		std::cerr << "Socket is open. Listening... " << std::endl;
	}

	if (!socket.bind(socketAddress)) {
		std::cerr << "Error binding socket!" << std::endl;
		socket.close();
		exit(-1);
	}

	std::vector<uint8_t> packet(2060);
	util::Ipv4SocketAddress from;

	RtpDepacker depacker;

	uint32_t receivedPacketCount = 0;

	while (running_) {
		socket.recvfrom(from, packet, 2060);
		std::cout << "Received " << packet.size() << " bytes from " << from.toString(true) << std::endl;

		if(receivedPacketCount++ % 99 == 0)
		{
			std::cout << "[INFO] Received " << receivedPacketCount << " packets during this session already. " << std::endl;
		}

		if (depacker.validatePacket(&packet, payloadType))
		{
			if (payloadType == 100)
			{
				std::vector<uint8_t> unpackedData = depacker.unpack(&packet);
				std::vector<uint8_t> decodedData;
				decodedData.reserve(960 * 2);
				decoder.decode(&unpackedData, &decodedData);
				std::cout << "SIZE: " << decodedData.size() << std::endl;
				jitterBuffer->add(decodedData);
			}
			else

			{
				jitterBuffer->add(depacker.unpack(&packet));
			}
		}
		else
		{
			std::cout << "Received invalid packet." << std::endl;
		}
	}
	socket.close();
}
