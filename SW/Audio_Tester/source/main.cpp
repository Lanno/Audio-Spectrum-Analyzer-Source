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
#include <algorithm>

#include <xparameters.h>
#include <xgpio.h>

#include <audio.hpp>


int main() {
    std::cout << "Beginning audio program..." << std::endl;

    nluckett::Audio audio;

    std::cout << "I2S Initialization Complete." << std::endl;

    audio.unmute();

    XGpio buttons;

    int status = XGpio_Initialize(&buttons, XPAR_BUTTONS_DEVICE_ID);

    if(status != XST_SUCCESS) throw("The Mute GPIO could not be initialized.");

    while(true) {
    	std::vector<u32> data;

    	u32 buttons_state = XGpio_DiscreteRead(&buttons, 1);

    	if(buttons_state != 0) {
    		std::cout << "Recording audio data." << std::endl;

    		for(u32 rec_counter = 0; rec_counter < 2000; rec_counter++) {
    		    //buttons_state = XGpio_DiscreteRead(&buttons, 1);

    		    audio.record(data);
    		}

    		std::cout << "Recorded " << data.size() << " samples." << std::endl;

    	}

    	buttons_state = XGpio_DiscreteRead(&buttons, 1);

    	if(buttons_state == 0 && data.size() > 0) {
    		std::cout << "Playing back audio data." << std::endl;

			audio.playback(data);

			std::cout << "Play back complete. " << data.size() << " samples remain in the buffer." <<  std::endl;
    	}
    }

    return 0;
}
