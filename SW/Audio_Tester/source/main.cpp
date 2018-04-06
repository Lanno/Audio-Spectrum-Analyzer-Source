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
    	std::vector<u32> rec_data;

    	u32 buttons_state = XGpio_DiscreteRead(&buttons, 1);

    	if(buttons_state != 0) {
    		std::cout << "Recording audio data." << std::endl;

    		while(buttons_state != 0) {
    		    buttons_state = XGpio_DiscreteRead(&buttons, 1);

    		    audio.record(rec_data);
    		}

    		std::cout << "Recorded " << rec_data.size() << " samples." << std::endl;

    	}

    	buttons_state = XGpio_DiscreteRead(&buttons, 1);

    	if(buttons_state == 0) {
    		while(rec_data.size() >= 500) {
				std::vector<u32> pbl_data;

				for(u32 idx = 0; idx < 500; idx++) {
					pbl_data.push_back(rec_data.back());

					rec_data.pop_back();
				}

				std::reverse(pbl_data.begin(), pbl_data.end());

				audio.playback(pbl_data);
    		}
    	}
    }

    return 0;
}
