/******************************************************************************/
/*
 *  Project: Voice-over-IP
 *  Author: Matthias Tessmann <matthias.tessmann@th-nuernberg.de
 *  Date: October, 5th 2015
 *
 *  Contents: Sound card wrapper class skeleton.
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

#include <iostream>
#include <cstring>
#include "RtAudio.h"

#include "soundcard.h"

namespace util {

// Pure viruals need a destructor implementation
AudioIO::~AudioIO() {}

// Helper functions
static std::string supportedSampleTypes(RtAudio::DeviceInfo const& info) {
  std::stringstream s;
  s << ((info.nativeFormats & RTAUDIO_SINT8)   ? "SINT8 "  :"");
  s << ((info.nativeFormats & RTAUDIO_SINT16)  ? "SINT16 " :"");
  s << ((info.nativeFormats & RTAUDIO_SINT24)  ? "SINT24 " :"");
  s << ((info.nativeFormats & RTAUDIO_SINT32)  ? "SINT32 " :"");
  s << ((info.nativeFormats & RTAUDIO_FLOAT32) ? "FLOAT32 ":"");
  s << ((info.nativeFormats & RTAUDIO_FLOAT64) ? "FLOAT64 ":"");
  return s.str();
}

// Internal implementation
struct SoundCard::SoundCardImpl {
  SoundCardImpl(AudioIO *audioIo);
  ~SoundCardImpl();

  int callback(void *outBuf, void *inBuf, unsigned int nFrames, double streamTime, RtAudioStreamStatus status);

  AudioIO*                                audioIO_;
  RtAudio                                 audio_;
  RtAudio::StreamParameters               inputParameters_;
  RtAudio::StreamParameters               outputParameters_;
  unsigned int                            sr_;
  unsigned int                            fs_;
  util::AudioBuffer::SampleFormat         fmt_;
  util::AudioBuffer                       inputBuffer_;
  util::AudioBuffer                       outputBuffer_;
  bool                                    initialized_;
  bool                                    running_;

  static int rtaudio_callback(void *outBuf, void *inBuf, unsigned int nFrames,
                              double streamTime, RtAudioStreamStatus status, void *userData);
};

SoundCard::SoundCardImpl::SoundCardImpl(AudioIO *audioIo)
  : audioIO_(audioIo),
    audio_(),
    inputParameters_(),
    outputParameters_(),
    sr_(0),
    fs_(0),
    fmt_(util::AudioBuffer::UNKNOWN),
    inputBuffer_(),
    outputBuffer_(),
    initialized_(false),
    running_(false)
{
}

SoundCard::SoundCardImpl::~SoundCardImpl() {
}

int SoundCard::SoundCardImpl::callback(void *outBuf, void *inBuf, unsigned int nFrames,
                                       double streamTime, RtAudioStreamStatus status)
{
  if (nFrames != fs_) {
    std::cerr << "Warning: need to adapt framesize in callback!" << std::endl;
    fs_ = nFrames;
    inputBuffer_.setFsChFmt(fs_, inputParameters_.nChannels, fmt_);
    outputBuffer_.setFsChFmt(fs_, outputParameters_.nChannels, fmt_);
  }

  ::memcpy(inputBuffer_.data(), inBuf, inputBuffer_.size());
  int retval = audioIO_->process(outputBuffer_, inputBuffer_);
  ::memcpy(outBuf, outputBuffer_.data(), outputBuffer_.size());

  return retval;
}

int SoundCard::SoundCardImpl::rtaudio_callback(void *outBuf, void *inBuf, unsigned int nFrames,
                                               double streamTime, RtAudioStreamStatus status, void *userData)
{
  SoundCardImpl *instance = static_cast<SoundCardImpl*>(userData);
  return instance->callback(outBuf, inBuf, nFrames, streamTime, status);
}

// Static SoundCard methods
void SoundCard::listDevices() {
    RtAudio audio;

    // Disable display of warning messages
    audio.showWarnings(false);

    const unsigned int nDevs = audio.getDeviceCount();

    RtAudio::DeviceInfo info;
    for (unsigned int i = 0; i < nDevs; ++i) {
      info = audio.getDeviceInfo(i);
      if (info.probed) {
        std::cerr << std::endl;
        std::cerr << "************************************************************" << std::endl;
        std::cerr << "Device ID = " << i << std::endl;
        std::cerr << "Device Name = " << info.name << std::endl;
        std::cerr << "Supported Channels (in/out/duplex) = ";
        std::cerr << info.inputChannels << "/" << info.outputChannels << "/" << info.duplexChannels << std::endl;
        std::cerr << "Native sample rates = ";
        for (unsigned int i = 0; i < info.sampleRates.size(); ++i) {
          std::cerr << info.sampleRates[i] << "Hz" << (i == info.sampleRates.size()-1 ? " ":", ");
        }
        std::cerr << std::endl;
        std::cerr << "Native sample types = " << supportedSampleTypes(info) << std::endl;

        std::cerr << "Is default input/output device = " << (info.isDefaultInput ? "YES":"NO") << " / ";
        std::cerr << (info.isDefaultOutput ? "YES":"NO") << std::endl;
      }
    }
    std::cerr << "************************************************************" << std::endl << std::endl;
}

// SoundCard implementation
SoundCard::SoundCard(AudioIO *audioIo)
  : impl_(new SoundCardImpl(audioIo))
{}

SoundCard::~SoundCard() {
  delete impl_;
}

bool SoundCard::init(int inDev, int outDev, unsigned int inCh,
                     unsigned int outCh, unsigned int sr,
                     unsigned int fs, util::AudioBuffer::SampleFormat sampleFormat)
{
  if (inCh == 0 || // Ususally you could signal non-duplex mode by setting nCh to 0
      outCh == 0 ||
      sr == 0 ||
      fs == 0 ||
      sampleFormat == util::AudioBuffer::UNKNOWN)
    return false;

    if (inDev < 0)
      inDev = impl_->audio_.getDefaultInputDevice();

    if (outDev < 0)
      outDev = impl_->audio_.getDefaultOutputDevice();

    impl_->inputParameters_.deviceId     = inDev;
    impl_->inputParameters_.nChannels    = inCh;
    impl_->inputParameters_.firstChannel = 0;

    impl_->outputParameters_.deviceId     = outDev;
    impl_->outputParameters_.nChannels    = outCh;
    impl_->outputParameters_.firstChannel = 0;

    impl_->sr_  = sr;
    impl_->fs_  = fs;
    impl_->fmt_ = sampleFormat;

    impl_->inputBuffer_.setSamplerate(impl_->sr_);
    impl_->outputBuffer_.setSamplerate(impl_->sr_);

    impl_->inputBuffer_.setFsChFmt(impl_->fs_, inCh, impl_->fmt_);
    impl_->outputBuffer_.setFsChFmt(impl_->fs_, outCh, impl_->fmt_);

    std::cerr << "SC init - buffer size in bytes: " << impl_->outputBuffer_.size() << std::endl;

    RtAudioFormat rtaudiofmt;

    switch (impl_->fmt_) {
      case util::AudioBuffer::INT16:
        rtaudiofmt = RTAUDIO_SINT16;
      break;
      case util::AudioBuffer::FLOAT32:
        rtaudiofmt = RTAUDIO_FLOAT32;
      break;
      default:
        std::cerr << "Unsupported sample format!" << std::endl;
        return false;
      break;
    }

    unsigned int expectedFramesize = impl_->fs_;

    try {
      impl_->audio_.openStream(&(impl_->outputParameters_), &(impl_->inputParameters_), rtaudiofmt,
                        impl_->sr_, &expectedFramesize, &SoundCard::SoundCardImpl::rtaudio_callback,
                        static_cast<void*>(impl_));
    } catch (RtAudioError& e) {
      std::cerr << "Error initializing!" << std::endl;
      e.printMessage();
      return false;
    }

    if (expectedFramesize != impl_->fs_) {
      std::cerr << "Warning: requested framesize differs from actual framesize ";
      std::cerr << "(" << impl_->fs_ << " vs. " << expectedFramesize << ")!" << std::endl;
      std::cerr << "Adjusting to " << expectedFramesize << std::endl;

      impl_->fs_ = expectedFramesize;
      impl_->inputBuffer_.setFsChFmt(impl_->fs_, inCh, impl_->fmt_);
      impl_->outputBuffer_.setFsChFmt(impl_->fs_, outCh, impl_->fmt_);
    }

    impl_->initialized_ = true;
    return impl_->initialized_;
}

bool SoundCard::start() {
  if (!impl_->initialized_)
    return false;

  try {
    impl_->audio_.startStream();
  } catch (RtAudioError& e) {
    std::cerr << "Error starting stream!" << std::endl;
    e.printMessage();
    return false;
  }

  impl_->running_ = true;
  return impl_->running_;
}

bool SoundCard::stop() {
  if (!impl_->running_)
    return false;

  try {
    impl_->audio_.stopStream();
    if (impl_->audio_.isStreamOpen()) {
      impl_->audio_.closeStream();
      impl_->initialized_ = false;
    }
  } catch (RtAudioError& e) {
    std::cerr << "Error starting stream!" << std::endl;
    e.printMessage();
  }

  impl_->running_ = false;
  return true;
}

}
