/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_RTP_PACKER_H
#define VOIP_RTP_PACKER_H

#include <vector>
#include <random>
#include <iostream>
#include "opus_settings.h"

class RtpPacker 
{
public:
	RtpPacker();
	void init(uint8_t payloadType);
	std::vector<uint8_t> pack(std::vector<uint8_t>* data);

private:
	void setFirstByte(std::vector<uint8_t>* header);
	void setSecondByte(std::vector<uint8_t>* header);
	void setSequenceNumber(std::vector<uint8_t>* header);
	void setTimestamp(std::vector<uint8_t>* header);
	void setSSRC(std::vector<uint8_t>* header);
	uint16_t getRandom16BitNumber();
	uint32_t getRandom32BitNumber();

	uint16_t seqNumber;
	uint32_t timestamp;
	uint32_t ssrc;
	uint8_t payloadType;
};

#endif /* VOIP_RTP_PACKER_H */
