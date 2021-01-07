/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "rtp_packer.h"

RtpPacker::RtpPacker() : sampleRate(), framesize(), payloadType()
{ 
	seqNumber = getRandom16BitNumber();
	timestamp = getRandom32BitNumber();
	ssrc = getRandom32BitNumber();
}

void RtpPacker::init(uint32_t sampleRate, uint32_t framesize, uint8_t payloadType)
{
	this->sampleRate = sampleRate;
	this->framesize = framesize;
	this->payloadType = payloadType;
	
	float_t frameDuration = (float_t) framesize * (1 / (float_t) sampleRate);
	uint32_t samplesPerSecond = 1 / frameDuration;
	timestampIncrease = sampleRate / samplesPerSecond;
}

std::vector<uint8_t> RtpPacker::pack(std::vector<uint8_t>* data)
{
	std::vector<uint8_t> header(12, 0);
	setFirstByte(&header);
	setSecondByte(&header);
	setSequenceNumber(&header);
	setTimestamp(&header);
	setSSRC(&header);

	header.insert(header.end(), data->begin(), data->end());

	return header;
}

void RtpPacker::setFirstByte(std::vector<uint8_t>* header)
{
	header->at(0) = 0x00;
	header->at(0) |= 0x80;
}

void RtpPacker::setSecondByte(std::vector<uint8_t>* header)
{
	header->at(1) = payloadType;
}

void RtpPacker::setSequenceNumber(std::vector<uint8_t>* header)
{
	header->at(2) = (seqNumber & 0xFF00) >> 8;
	header->at(3) = seqNumber & 0x00FF;
	seqNumber = (seqNumber + 1) % std::numeric_limits<uint16_t>::max();
}

void RtpPacker::setTimestamp(std::vector<uint8_t>* header)
{
	if (payloadType == 10 || payloadType == 11)
	{
		timestamp = (timestamp + 512) % std::numeric_limits<uint32_t>::max();
	}
	else if (payloadType == 100)
	{
		timestamp = (timestamp + 480) % std::numeric_limits<uint32_t>::max();
	}
	
	header->at(4) = (timestamp & 0xFF000000) >> 24;
	header->at(5) = (timestamp & 0x00FF0000) >> 16;
	header->at(6) = (timestamp & 0x0000FF00) >> 8;
	header->at(7) = (timestamp & 0x000000FF);
}

void RtpPacker::setSSRC(std::vector<uint8_t>* header)
{
	header->at(8) = (ssrc & 0xFF000000) >> 24;
	header->at(9) = (ssrc & 0x00FF0000) >> 16;
	header->at(10) = (ssrc & 0x0000FF00) >> 8;
	header->at(11) = (ssrc & 0x000000FF);
}

uint16_t RtpPacker::getRandom16BitNumber()
{
	return (uint16_t) 1234;
}

uint32_t RtpPacker::getRandom32BitNumber()
{
	return (uint32_t) 56789;
}