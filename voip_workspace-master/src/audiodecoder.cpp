/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "audiodecoder.h"
#include <iostream>

AudioDecoder::AudioDecoder() : decoder(NULL), framesize(0), rate(0) { }

AudioDecoder::~AudioDecoder()
{
	opus_decoder_destroy(decoder);
}

void AudioDecoder::init(opus_int32 samplingRate, opus_int32 framesize, int channels)
{
	int error;

	this->framesize = framesize;

	decoder = opus_decoder_create(samplingRate, channels, &error);
	if (error == OPUS_OK)
	{
		std::cout << "[OPUS] Opus decoder created successfully" << std::endl;
	}
	else
	{
		throw std::invalid_argument("[OPUS] Could not init opus decoder.");
	}
}

void AudioDecoder::decode(std::vector<uint8_t>* inputData, std::vector<uint8_t>* outputData)
{
	opus_int16 out[(6 * 480)];
	uint8_t outUint8[(6 * 480) * 2 * 2];

	int decodedSize = opus_decode(decoder, &inputData->at(0), (opus_int32)inputData->size(), out, (6 * 480), 0);
	std::cout << "RESULT DEC: " << decodedSize << std::endl;

	for (int i = 0; i < 2 * 480; i++)
	{
		outUint8[2 * i] = out[i] & 0xFF;
		outUint8[2 * i + 1] = (out[i] >> 8) & 0xFF;
	}

	if (decodedSize > 0)
	{
		outputData->insert(outputData->begin(), outUint8, outUint8 + (4 *480));
	}
}