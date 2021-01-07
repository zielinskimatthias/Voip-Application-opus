/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#ifndef VOIP_SIMPLEJB_H
#define VOIP_SIMPLEJB_H

#include <vector>
#include <deque>
#include <iostream>
#include <mutex>

#define BUFFER_MAX_PACKETS 10

class SimpleJB 
{
public:
	SimpleJB();
	~SimpleJB();
	void add(std::vector<uint8_t> data);
	void getFrontData(uint8_t* outputPtr);
	bool hasData();

private:
	std::deque<std::vector<uint8_t>> dataBuffer;
	std::mutex mutex;
};

#endif /* VOIP_SIMPLEJB_H */
