/////////////////////////////////////////////////////////////////////
// The audio tester project will initialize the Zybo's audio codec
// via the audio interface IP, and record and play back audio automatically
// every 5 seconds.
//
// Author: Nolan Luckett
// Organization: CSULB
// Class: CECS561
// Project: Final Project, Basic System

#include <iostream>
#include <vector>
#include <cstdlib>

#include <audio.hpp>


int main() {
    std::cout << "Beginning audio program..." << std::endl;

    nluckett::Audio audio;

    std::cout << "I2S Initialization Complete." << std::endl;

    audio.unmute();

	// audio.record();

    std::vector<u32> data;

    for(int idx = 0; idx < 512; idx++)
        data.push_back(std::rand() % (1 << 24));

    while(true)
    	audio.playback(data);

    return 0;
}
