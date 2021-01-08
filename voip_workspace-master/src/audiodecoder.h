/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_AUDIODECODER_H
#define VOIP_AUDIODECODER_H

#include "opus.h"
#include "opus_settings.h"
#include <vector>

class AudioDecoder {
public:
	AudioDecoder();
	~AudioDecoder();
	void init(opus_int32 samplingRate, opus_int32 framesize, int channels);
	void decode(std::vector<uint8_t>* inputData, std::vector<uint8_t>* outputData);

private:
	OpusDecoder* decoder;
	opus_int32 rate;
	opus_int32 framesize;
	opus_int16 out[(OPUS_CHANNELS * OPUS_FRAMESIZE)];
	// times 2, because uint8_t is just 1 Byte and uint16_t is two, so the actual framesize in bytes is 2 * OPUS_FRAMESIZE
	uint8_t outUint8[(2 * OPUS_CHANNELS * OPUS_FRAMESIZE)];
};

#endif /* VOIP_AUDIODECODER_H */
