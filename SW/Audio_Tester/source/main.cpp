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

#include <audio.hpp>


int main() {
	std::cout << "Entered program." << std::endl;

    nluckett::Audio audio;

	audio.record();

	audio.playback();

	return 0;
}
