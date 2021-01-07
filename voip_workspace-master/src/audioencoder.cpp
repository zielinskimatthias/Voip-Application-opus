/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "audioencoder.h"


AudioEncoder::AudioEncoder() : encoder(NULL), framesize(0), rate(0) { }

AudioEncoder::~AudioEncoder()
{
	opus_encoder_destroy(encoder);
}

void AudioEncoder::init(opus_int32 samplingRate, opus_int32 framesize, int channels)
{
	int error;

	this->framesize = framesize;

	encoder = opus_encoder_create(samplingRate, channels, OPUS_APPLICATION_VOIP, &error);
	if (error == OPUS_OK)
	{
		std::cout << "[OPUS] Opus encoder created successfully" << std::endl;
	}
	else
	{
		throw std::invalid_argument("[OPUS] Could not init opus encoder.");
	}
}

void AudioEncoder::encode(std::vector<uint8_t>* inputData, std::vector<uint8_t>* outputData)
{
	opus_int16 in[(480 * 2)];
	uint8_t out[1276];

	for (int i = 0; i < (2 * 480); i++)
		in[i] = inputData->at(2 * i + 1) << 8 | inputData->at(2 * i);

	opus_int32 encodedSize = opus_encode(encoder, in, framesize, out, 1276);
	std::cout << "RESULT: " << encodedSize << std::endl;
	
	outputData->insert(outputData->begin(), out, out + encodedSize);

}
