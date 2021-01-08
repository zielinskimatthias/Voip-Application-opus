/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_RECEIVER_H
#define VOIP_RECEIVER_H

#include <thread>
#include <iostream>
#include "socket.h"
#include "simplejb.h"
#include "rtp_depacker.h"
#include "audiodecoder.h"
#include "opus_settings.h"

class Receiver {
public:
	Receiver();
	~Receiver();

	void init(uint32_t port, SimpleJB* jitterBuffer, uint8_t payloadType);
	void start();
	void stop();

private:
	void receive();

	std::thread self_;
	bool        running_;

	util::UdpSocket socket;
	SimpleJB* jitterBuffer;
	uint32_t port;
	uint8_t payloadType;
	AudioDecoder decoder;

};

#endif /* VOIP_RECEIVER_H */
