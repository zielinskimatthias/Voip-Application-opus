/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "rtp_depacker.h"
#include <iostream>

RtpDepacker::RtpDepacker() : isSequenceNumberSet(false), isSSRCSet(false), lastSequenceNumber(0), expectedSSRC(0) { }

bool RtpDepacker::validatePacket(std::vector<uint8_t>* packet, uint8_t expectedPayloadType)
{

	if (!validateVersion(packet)) 
	{ 
		std::cerr << "Invalid packet version: " << static_cast<unsigned>(getVersion(packet)) << std::endl;
		return false; 
	}
	if (!validatePayloadType(packet, expectedPayloadType)) 
	{ 
		std::cerr << "Payload Type is: " << static_cast<unsigned>(getPayloadType(packet)) << ". Expected: " << expectedPayloadType <<  std::endl;
		return false; 
	}

	if (isSequenceNumberSet) 
	{ 
		if (!validateSequenceNumber(packet)) 
		{
			std::cerr << "Sequence number is: " << getSequenceNumber(packet) << ". Expected: " << lastSequenceNumber + 1 << std::endl;
			return false; 
		}
	}

	if (isSSRCSet) 
	{ 
		if (!validateSSRC(packet)) 
		{
			std::cerr << "SSRC changed! SSRC is: " << getSSRC(packet) << ". Expected: " << expectedSSRC << std::endl;
			return false; 
		}
	}

	return true;
}

std::vector<uint8_t> RtpDepacker::unpack(std::vector<uint8_t>* packet)
{
	if (!isSequenceNumberSet)
	{
		lastSequenceNumber = getSequenceNumber(packet);
		isSequenceNumberSet = true;
	}

	if (!isSSRCSet)
	{
		expectedSSRC = getSSRC(packet);
		isSSRCSet = true;
	}

	uint8_t payloadType = packet->at(1) & 0x7F;

	return std::vector<uint8_t>(packet->begin() + 12, packet->end());
}

bool RtpDepacker::validateVersion(std::vector<uint8_t>* packet)
{
	return getVersion(packet) == 2;
}

bool RtpDepacker::validatePayloadType(std::vector<uint8_t>* packet, uint8_t expectedPayloadType)
{
	return getPayloadType(packet) == expectedPayloadType;
}

bool RtpDepacker::validateSequenceNumber(std::vector<uint8_t>* packet)
{
	uint16_t packetSeqNumber = getSequenceNumber(packet);

	bool result = (packetSeqNumber == lastSequenceNumber + 1);
	lastSequenceNumber = packetSeqNumber;

	return result;
}

bool RtpDepacker::validateSSRC(std::vector<uint8_t>* packet)
{
	return getSSRC(packet) == expectedSSRC;
}

uint16_t RtpDepacker::getSequenceNumber(std::vector<uint8_t>* packet)
{
	uint16_t seqNumber = 0;
	seqNumber |= (packet->at(2) << 8);
	seqNumber |= packet->at(3);

	return seqNumber;
}

uint8_t RtpDepacker::getPayloadType(std::vector<uint8_t>* packet)
{
	return (packet->at(1) & 0x7f);
}

uint8_t RtpDepacker::getVersion(std::vector<uint8_t>* packet)
{
	return ((packet->at(0) & 0xC0) >> 6);
}

uint32_t RtpDepacker::getSSRC(std::vector<uint8_t>* packet)
{
	uint32_t ssrc = 0;
	ssrc |= (packet->at(8) << 24);
	ssrc |= (packet->at(9) << 16);
	ssrc |= (packet->at(10) << 8);
	ssrc |= packet->at(11);

	return ssrc;
}