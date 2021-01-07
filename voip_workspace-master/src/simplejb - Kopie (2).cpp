/*
 * This file and maybe an associated implementation
 * are provided as a means to get you started with
 * the project. You can adapt and use the provided
 * structure or start from scratch and create your
 * own implementation.
 */

#include "simplejb.h"

SimpleJB::SimpleJB(uint32_t samplerate) {
    milliSec = 200;
    float sampLength = 1 / (float)samplerate;
    maxSize = milliSec / sampLength;
    std::cout << "maxsize Jitterbuffer" << maxSize << std::endl;
}


void SimpleJB::get(uint8_t* target) {
    if (!rtpPackets.empty())
    {
        const std::lock_guard<std::mutex> lock(mutex);
        std::vector<uint8_t> packet = rtpPackets.front();
        rtpPackets.pop_front();
        std::copy(packet.begin(), packet.end(), target); 
        std::cout << "Copied jitterbuffer" << rtpPackets.size() << std::endl;
    }
    else 
    {
        std::cout << "Jitterbuffer is empty" << rtpPackets.size() << std::endl;
    }
}

void SimpleJB::addPacket(std::vector<uint8_t>& packet) {
    if (rtpPackets.size() < maxSize) 
    {
        const std::lock_guard<std::mutex> lock(mutex);
        rtpPackets.push_back(packet);
        std::cout << "Jitterbuffer Size: " << rtpPackets.size() << std::endl;
    }
    else 
    {
        std::cout << "Jitterbuffer full" <<std::endl;
    }
}




