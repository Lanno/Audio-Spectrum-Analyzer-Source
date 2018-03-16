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
#include <xscugic.h>
#include <xil_exception.h>

#include <audio.hpp>

#define BUTTONS_INTERRUPT 1

bool RECORD   = false;
bool PLAYBACK = false;

void button_handler(void *InstancePtr) {
	// Disable GPIO interrupts
	XGpio_InterruptDisable((XGpio*) InstancePtr, BUTTONS_INTERRUPT);

	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus((XGpio*) InstancePtr) & BUTTONS_INTERRUPT) != BUTTONS_INTERRUPT)
		return;

	u32 button_value = XGpio_DiscreteRead((XGpio*) InstancePtr, 1);

	if     ((button_value & 0x1) == 0x1 && RECORD == false) RECORD = true;
	else if((button_value & 0x1) == 0x0 && RECORD == true ) RECORD = false;

	if     ((button_value & 0x2) == 0x2 && PLAYBACK == false) PLAYBACK = true;
	else if((button_value & 0x2) == 0x0 && PLAYBACK == true ) PLAYBACK = false;

	std::cout << "Button state: " << button_value << std::endl;

	std::cout << "Record: " << RECORD << " Playback: " << PLAYBACK << std::endl;

    XGpio_InterruptClear((XGpio*) InstancePtr, BUTTONS_INTERRUPT);

    // Enable GPIO interrupts
    XGpio_InterruptEnable((XGpio*) InstancePtr, BUTTONS_INTERRUPT);
}


void init_gpio(void) {
	// Setup the buttons with their interrupts

	XGpio BUTTONS;

	int status = XGpio_Initialize(&BUTTONS, XPAR_BUTTONS_DEVICE_ID);

	if(status != XST_SUCCESS) throw std::runtime_error("The Mute GPIO could not be initialized.");

	// Interrupt controller initialization
	XScuGic gic;

	XScuGic_Config *gic_config;

	gic_config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);

	status = XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress);

	if(status != XST_SUCCESS) throw std::runtime_error("The Mute GPIO could not be initialized.");

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
								(Xil_ExceptionHandler) XScuGic_InterruptHandler,
								 &gic);

	Xil_ExceptionEnable();

	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(&gic,
							 XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR,
							 (Xil_ExceptionHandler) button_handler,
							 (void *) &BUTTONS);

	if(status != XST_SUCCESS) throw std::runtime_error("The Mute GPIO could not be initialized.");

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(&BUTTONS, BUTTONS_INTERRUPT);

	XGpio_InterruptGlobalEnable(&BUTTONS);

	// Enable GPIO interrupts in the controller
	XScuGic_Enable(&gic, XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR);
}


int main() {
    std::cout << "Beginning audio program..." << std::endl;

    nluckett::Audio audio;

    std::cout << "I2S Initialization Complete." << std::endl;

    audio.unmute();

    std::cout << "Initializing GPIO with interrupts." << std::endl;

    init_gpio();

    std::cout << "GPIO initialization complete." << std::endl;

    while(true) {}
//    	std::vector<u32> data;
//
//    	u32 buttons_state = XGpio_DiscreteRead(&BUTTONS, 1);
//
//    	if(buttons_state != 0) {
//    		std::cout << "Recording audio data." << std::endl;
//
//    		for(u32 rec_counter = 0; rec_counter < 2000; rec_counter++) {
//    		    //buttons_state = XGpio_DiscreteRead(&BUTTONS, 1);
//
//    		    audio.record(data);
//    		}
//
//    		std::cout << "Recorded " << data.size() << " samples." << std::endl;
//
//    	}
//
//    	buttons_state = XGpio_DiscreteRead(&BUTTONS, 1);
//
//    	if(buttons_state == 0 && data.size() > 0) {
//    		std::cout << "Playing back audio data." << std::endl;
//
//			audio.playback(data);
//
//			std::cout << "Play back complete. " << data.size() << " samples remain in the buffer." <<  std::endl;
//    	}
//    }

    return 0;
}
