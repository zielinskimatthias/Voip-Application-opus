/******************************************************************************/
/*
 *  Project: Voice-over-IP
 *  Author: Matthias Tessmann <matthias.tessmann@th-nuernberg.de>
 *  Date: October, 5th 2015
 *
 *  Contents: Sound card wrapper class.
 *            Intended for educational purposes.
 *
 *  Additoinal notes:
 *         - There is very little error handling.
 *         - The implementation may contain bugs of any kind!
 *
 * The MIT License (MIT)
 *
 *  Copyright (c) 2015 Matthias Teßmann <matthias.tessmann@th-nuernberg.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/******************************************************************************/

#ifndef VOIP_SOUNDCARD_H
#define VOIP_SOUNDCARD_H

#include "audiobuffer.h"

namespace util {

class AudioIO {
public:
  virtual ~AudioIO() = 0;
  virtual int process(util::AudioBuffer& output, util::AudioBuffer const& input) = 0;
};

class SoundCard {
public:
  static void listDevices();

public:
  SoundCard(AudioIO *audioIo);
  ~SoundCard();
  SoundCard(SoundCard const&) = delete;
  SoundCard(SoundCard const&&) = delete;
  SoundCard& operator=(SoundCard const&) = delete;
  SoundCard& operator=(SoundCard const&&) = delete;


  bool init(int inDev, int outDev, unsigned int inCh, unsigned int outCh, unsigned int sr,
            unsigned int fs, util::AudioBuffer::SampleFormat sampleFormat);
  bool start();
  bool stop();

private:
  struct SoundCardImpl;
  SoundCardImpl *impl_;
};

}

#endif /* VOIP_SOUNDCARD_H */
