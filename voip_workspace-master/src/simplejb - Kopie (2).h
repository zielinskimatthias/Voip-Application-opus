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
#include <iostream>
#include <mutex>
#include <list>

class SimpleJB {
public:
    SimpleJB(uint32_t samplerate);
    ~SimpleJB() {};

    void get(uint8_t* target);
    void addPacket(std::vector<uint8_t>& packet);

private:
    std::list<std::vector<uint8_t>> rtpPackets;
    float maxSize;
    int milliSec;
    std::mutex mutex;
};

#endif /* VOIP_SIMPLEJB_H */
