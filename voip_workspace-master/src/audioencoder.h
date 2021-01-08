/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_AUDIOENCODER_H
#define VOIP_AUDIOENCODER_H

#include "opus.h"
#include "opus_settings.h"
#include <vector>
#include <iostream>

class AudioEncoder {

public:
	AudioEncoder();
	~AudioEncoder();
	void init(opus_int32 samplingRate, opus_int32 framesize, int channels);
	void encode(std::vector<uint8_t>* inputData, std::vector<uint8_t>* outputData);

private:
	OpusEncoder* encoder;
	opus_int32 rate;
	opus_int32 framesize;
	opus_int16 in[(OPUS_FRAMESIZE * OPUS_CHANNELS)];
	uint8_t out[OPUS_MAX_SIZE];
};

#endif /* VOIP_AUDIOENCODER_H */
