/******************************************************************************/
/*
 *  Project: Voice-over-IP
 *  Author: Matthias Tessmann <matthias.tessmann@th-nuernberg.de
 *  Date: October, 5th 2015
 *
 *  Contents: Application wrapper class skeleton.
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

#include "voip_comm.h"
#include "soundcard.h"

#include "receiver.h"

#include "tclap/CmdLine.h"

VoIPComm::VoIPComm() : isOpus(false), isUDP(false), isLoopback(false), isInitialized(false), soundcard(this), jitterBuffer(), packer(), encoder() {}
VoIPComm::~VoIPComm() 
{
	if (isUDP)
	{
		delete(jitterBuffer);
	}
}

int VoIPComm::process(util::AudioBuffer& output, util::AudioBuffer const& input) 
{
	// two modes this application can be run in
	if (isLoopback) { processLoopback(output, input); }
	else if (isUDP) { processUDP(output, input); }
	return 0;
}

void VoIPComm::processLoopback(util::AudioBuffer& output, util::AudioBuffer const& input)
{
	output = input;
}

void VoIPComm::processUDP(util::AudioBuffer& output, util::AudioBuffer const& input)
{
	/* PACKING & UNPACKING */
	if (isOpus)
	{
		/* IF OPUS ENCODE/DECODE */
		// allocate the data with the max encoded data size
		std::vector <uint8_t>encodedData;
		encodedData.reserve(OPUS_MAX_SIZE);
		encoder.encode(&std::vector<uint8_t>(input.data(), input.data() + input.size()), &encodedData);
	
		sender.send(&packer.pack(&encodedData));
	}
	else
	{
		sender.send(&packer.pack(&std::vector<uint8_t>(input.data(), input.data() + input.size())));
	}
	
	/* READ OUT JITTER BUFFER */
	if (jitterBuffer->hasData())
	{
		jitterBuffer->getFrontData(output.data());
	}
	else
	{
		std::cout << "Buffer is empty" << std::endl;
	}
}

int VoIPComm::exec(int argc, char* argv[]) {

	if (!init(argc, argv)) {
		std::cerr << "Error initializing!" << std::endl;
		return -1;
	}
	

	if (isInitialized)
	{
		if (isUDP)
		{
			receiver.start();
		}

		if (soundcard.start())
		{
			std::cout << "Playback started, press any key to stop the playback.";
		}
	}
	else
	{
		std::cerr << "Initialize a mode first! E.g. call initLoopback(...);" << std::endl;
		exit(-1);
	}

	char input;
	std::cin.get(input);
	

	soundcard.stop();
	
	if (isUDP)
	{
		receiver.stop();
		sender.stop();
	}
	
	return 0;
}

bool VoIPComm::init(int argc, char* argv[]) {


	/* Set up command line arguments */
	try {

		TCLAP::CmdLine cmd("VoIP Real-Time Communication Project", ' ', "0.1");

		// This is the only command line argument currently used
		TCLAP::SwitchArg listDevices("l", "list-devices", "List audio devices", cmd, false);
		TCLAP::SwitchArg loopBack("b", "loopback", "Execution in loopback mode", cmd, false);

		// These here show you what you might need
		TCLAP::ValueArg<int>          inDev("i", "input-device", "Select input device", false, -1, "int", cmd);
		TCLAP::ValueArg<int>          outDev("o", "output-device", "Select output device", false, -1, "int", cmd);
		TCLAP::ValueArg<unsigned int> inCh("", "inCh", "Number of input channels (default: 1)", false, 1, "unsigned int", cmd);
		TCLAP::ValueArg<unsigned int> outCh("", "outCh", "Number of output channels (default 1)", false, 1, "unsigned int", cmd);
		TCLAP::ValueArg<unsigned int> fs("f", "Framesize", "Framesize (default: 512)", false, 512, "unsigned int", cmd);
		TCLAP::ValueArg<unsigned int> s("s", "samplerate", "Samplerate (default: 44100)", false, 44100, "unsigned int", cmd);
		TCLAP::ValueArg<unsigned int> rPort("", "rPort", "Remote Port (default: 1976)", false, 1976, "unsigned int", cmd);
		TCLAP::ValueArg<unsigned int> lPort("", "lPort", "Local Port (default: 1976)", false, 1976, "unsigned int", cmd);
		TCLAP::ValueArg<unsigned int> payloadType("", "pt", "Payload type (ignores sample rate and inCh setting) (10, 11, 100 [opus])", false, 10, "unsigned int", cmd);

		TCLAP::UnlabeledValueArg<std::string> destIp("destIp", "Destination IP address", false, "", "std::string", cmd);

		cmd.parse(argc, argv);

		/* Add argument processing here */
		if (listDevices.getValue()) {
			this->listDevices();
			exit(0);
		}

		/****** loopback can not be used in combination with UDP or opus ******/
		if (loopBack.getValue() && destIp.getValue() != "")
		{
			std::cout << "Using loopback mode in combination with or UDP (IP passed as argument) is not allowed." << std::endl;
			exit(-1);
		}

		/****** mode selection ******/
		if (loopBack.getValue())
		{
			/* LOOPBACK */
			isLoopback = true;
			if(!initLoopback(inDev.getValue(), outDev.getValue(), inCh.getValue(), outCh.getValue(), s.getValue(), fs.getValue()))
			{
				exit(-1);
			}
		}
		else if (destIp.getValue() != "")
		{
			/* UDP */
			uint32_t sampleRate;
			uint32_t channels;
			uint32_t frameSize;

			// configure payload type parameters
			switch (payloadType.getValue())
			{
			case 10:
				channels = 2;
				sampleRate = 44100;
				frameSize = 512;
				break;
			case 11:
				channels = 1;
				sampleRate = 44100;
				frameSize = 512;
				break;
			case OPUS_PAYLOADTYPE:
				channels = OPUS_CHANNELS;
				sampleRate = OPUS_SAMPLERATE;
				frameSize = OPUS_FRAMESIZE;
				break;
			default:
				std::cerr << "Invalid payload type chosen." << std::endl;
				TCLAP::StdOutput().usage(cmd);
				exit(-1);
				break;
			}

			isUDP = true;
			if (!initUDP(destIp.getValue(), lPort.getValue(), rPort.getValue(), inDev.getValue(), outDev.getValue(), channels, channels, sampleRate, frameSize, 
				payloadType.getValue()))
			{
				exit(-1);
			}

			/* UDP + OPUS */
			if (payloadType.getValue() == OPUS_PAYLOADTYPE)
			{
				initOpus(sampleRate, frameSize, channels);
				isOpus = true;
			}
		}
		else
		{
			TCLAP::StdOutput().usage(cmd);
			exit(-1);
		}

	}
	catch (TCLAP::ArgException& argEx) {
		std::cerr << "Error parsing command line arguments: " << argEx.error() << " for argument " << argEx.argId() << std::endl;
		return false;
	}

	return true;
}

void VoIPComm::listDevices() {
	util::SoundCard::listDevices();
}

bool VoIPComm::initLoopback(int inputDeviceID, int outputDeviceID, uint32_t inputChannels, uint32_t outputChannels, uint32_t sampleRate, uint32_t framesize)
{
	// intialization method for loopback
	if (isInitialized) { return false; }
	if (soundcard.init(inputDeviceID, outputDeviceID, inputChannels, outputChannels, sampleRate, framesize, util::AudioBuffer::INT16))
	{
		std::cout << "[Loopback] soundcard successfully initialized!" << std::endl;
		isInitialized = true;
		return true;
	}
	std::cerr << "Error when initializing soundcard!" << std::endl;
	return false;
}

bool VoIPComm::initUDP(std::string ipAddress, uint32_t lPort, uint32_t rPort, int inputDeviceID, int outputDeviceID, uint32_t inputChannels, 
	uint32_t outputChannels, uint32_t sampleRate, uint32_t framesize, uint8_t payloadType)
{
	//intialization method for UDP
	if (isInitialized) { return false; }
	
	if (soundcard.init(inputDeviceID, outputDeviceID, inputChannels, outputChannels, sampleRate, framesize, util::AudioBuffer::INT16))
	{
		std::cout << "[UDP] soundcard successfully initialized!" << std::endl;

		packer.init(payloadType);
		jitterBuffer = new SimpleJB();
		receiver.init(lPort, jitterBuffer, payloadType);
		sender.init(ipAddress, rPort);
		isInitialized = true;
		return true;
	}
	std::cerr << "Error when initializing soundcard!" << std::endl;

	return false;
}

void VoIPComm::initOpus(uint32_t sampleRate, uint32_t framesize, uint32_t inputChannels)
{
	//intialization method for opus
	encoder.init(sampleRate, framesize, inputChannels);
	std::cout << "[OPUS] opus initialized!" << std::endl;
}
