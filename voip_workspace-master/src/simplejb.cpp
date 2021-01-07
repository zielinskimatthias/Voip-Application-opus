/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "simplejb.h"

SimpleJB::SimpleJB()
{
	std::cout << "Jitterbuffer with capacity for " << BUFFER_MAX_PACKETS << " packets (without header) created." << std::endl;
}

SimpleJB::~SimpleJB() { }

void SimpleJB::add(std::vector<uint8_t> data)
{
	if (dataBuffer.size() == BUFFER_MAX_PACKETS)
	{
		std::cerr << "Buffer full! Resetting the buffer to avoid too long delay" << std::endl;
		dataBuffer.clear();
		//return;
	}
	const std::lock_guard<std::mutex> lock(mutex);

	dataBuffer.push_back(data);
	//std::cout << "current buffer fill: " << dataBuffer.size() << " data packets (without header)." << std::endl;
}

void SimpleJB::getFrontData(uint8_t* outputPtr)
{
	if (dataBuffer.empty()) { return; }
	const std::lock_guard<std::mutex> lock(mutex);

	std::copy(dataBuffer.front().begin(), dataBuffer.front().end(), outputPtr);

	dataBuffer.pop_front();
}

bool SimpleJB::hasData()
{
	return !dataBuffer.empty();
}




