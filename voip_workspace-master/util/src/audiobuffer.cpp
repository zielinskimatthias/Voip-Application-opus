/******************************************************************************/
/*
 *  Project: Voice-over-IP
 *  Author: Matthias Tessmann <matthias.tessmann@th-nuernberg.de
 *  Date: October, 5th 2015
 *
 *  Contents: Very simple audio buffer class (wrapper around std::vector).
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

#include "audiobuffer.h"

#include <iostream>

namespace util {

AudioBuffer::AudioBuffer()
  : frameSize_(0),
    nChannels_(0),
    sampleRate_(0),
    format_(UNKNOWN),
    data_()
{}


AudioBuffer::AudioBuffer(uint32_t fs, uint32_t nch,
                         uint32_t sr,
                         AudioBuffer::SampleFormat fmt)
  : frameSize_(fs),
    nChannels_(nch),
    sampleRate_(sr),
    format_(fmt),
    data_(fs*nch*(int)fmt/8, 0)
{}

// Note: this may be inefficient
AudioBuffer::AudioBuffer(AudioBuffer const& rhs)
  : frameSize_(rhs.frameSize_),
    nChannels_(rhs.nChannels_),
    sampleRate_(rhs.sampleRate_),
    format_(rhs.format_),
    data_(rhs.data_)
{
}

// Note: this may be inefficient
AudioBuffer& AudioBuffer::operator=(AudioBuffer const& rhs) {
  if (this == &rhs) {
    return *this;
  }
  frameSize_  = rhs.frameSize_;
  nChannels_  = rhs.nChannels_;
  sampleRate_ = rhs.sampleRate_;
  format_     = rhs.format_;
  data_       = rhs.data_;

  return *this;
}

void AudioBuffer::setSamplerate(uint32_t sr) {
  sampleRate_ = sr;
}

void AudioBuffer::setFsChFmt(uint32_t fs, uint32_t nch, AudioBuffer::SampleFormat fmt) {
  data_.clear();
  frameSize_ = fs;
  nChannels_ = nch;
  format_    = fmt;

  uint32_t size = frameSize_ * nChannels_ * (format_ / 8);
  data_.resize(size);
}

}
