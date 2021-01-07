/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_RTP_DEPACKER_H
#define VOIP_RTP_DEPACKER_H

#include <vector>

class RtpDepacker 
{
public:
	RtpDepacker();
	bool validatePacket(std::vector<uint8_t>* packet, uint8_t expectedPayloadType);
	std::vector<uint8_t> unpack(std::vector<uint8_t>* packet);

private:
	bool validateVersion(std::vector<uint8_t>* packet);
	bool validatePayloadType(std::vector<uint8_t>* packet, uint8_t expectedPayloadType);
	bool validateSequenceNumber(std::vector<uint8_t>* packet);
	bool validateSSRC(std::vector<uint8_t>* packet);
	uint16_t getSequenceNumber(std::vector<uint8_t>* packet);
	uint8_t getPayloadType(std::vector<uint8_t>* packet);
	uint8_t getVersion(std::vector<uint8_t>* packet);
	uint32_t getSSRC(std::vector<uint8_t>* packet);
	bool isSequenceNumberSet;
	uint16_t lastSequenceNumber;
	bool isSSRCSet;
	uint32_t expectedSSRC;
};

#endif /* VOIP_RTP_DEPACKER_H */
