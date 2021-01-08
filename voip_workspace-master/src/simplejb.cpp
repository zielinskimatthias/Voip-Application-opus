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
	std::cout << "[JB] Jitterbuffer with capacity for " << BUFFER_MAX_PACKETS << " packets (without header) created." << std::endl;
}

SimpleJB::~SimpleJB() { }

void SimpleJB::add(std::vector<uint8_t> data)
{
	// when the buffer reaches it max size, clear it before adding the new packet to avoid too long delay.
	if (dataBuffer.size() == BUFFER_MAX_PACKETS)
	{
		std::cerr << "[JB] Buffer full! Resetting the buffer to avoid too long delay" << std::endl;
		dataBuffer.clear();
	}

	// The class lock_guard is a mutex wrapper that provides a convenient RAII-style mechanism for owning a mutex for the duration of a scoped block.
	// source: https://en.cppreference.com/w/cpp/thread/lock_guard
	const std::lock_guard<std::mutex> lock(mutex);

	dataBuffer.push_back(data);
	//std::cout << "current buffer fill: " << dataBuffer.size() << " data packets (without header)." << std::endl;
}

void SimpleJB::getFrontData(uint8_t* outputPtr)
{
	if (dataBuffer.empty()) { return; }

	// The class lock_guard is a mutex wrapper that provides a convenient RAII-style mechanism for owning a mutex for the duration of a scoped block.
	// source: https://en.cppreference.com/w/cpp/thread/lock_guard
	const std::lock_guard<std::mutex> lock(mutex);

	// copy first packet to the outputPtr (should be the playout buffer)
	std::copy(dataBuffer.front().begin(), dataBuffer.front().end(), outputPtr);

	// pop the packet from the front
	dataBuffer.pop_front();
}

bool SimpleJB::hasData()
{
	return !dataBuffer.empty();
}




