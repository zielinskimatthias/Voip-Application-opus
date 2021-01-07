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
};

#endif /* VOIP_AUDIODECODER_H */
