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

// the maximum amout of packets the jitter buffer will store.
// my idea was to avoid too long delay and just start with more current packets
// when the buffer gets too filled. So the buffer is emptied when the maximum size + 1 is reached!
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
	// i chose to use a deque because it allows fast insertion and deletion at both its beginning and its end
	// insertion and removal at beginning or end in O(1)
	std::deque<std::vector<uint8_t>> dataBuffer;
	std::mutex mutex;
};

#endif /* VOIP_SIMPLEJB_H */
