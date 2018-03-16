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


XGpio buttons;


void button_handler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&buttons, BUTTONS_INTERRUPT);

	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&buttons) & BUTTONS_INTERRUPT) != BUTTONS_INTERRUPT) {
		return;
	}

	u32 btn_value = XGpio_DiscreteRead(&buttons, 1);
	// Increment counter based on button value

	std::cout << "Button status: " << btn_value << std::endl;

    (void)XGpio_InterruptClear(&buttons, BUTTONS_INTERRUPT);

    // Enable GPIO interrupts
    XGpio_InterruptEnable(&buttons, BUTTONS_INTERRUPT);
}


void init_gpio(void) {
	// Setup the buttons with their interrupts

	int status = XGpio_Initialize(&buttons, XPAR_BUTTONS_DEVICE_ID);

	if(status != XST_SUCCESS) throw std::runtime_error("The Mute GPIO could not be initialized.");

	std::cout << "GPIO initialized." << std::endl;

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
							 (void *) &buttons);

	if(status != XST_SUCCESS) throw std::runtime_error("The Mute GPIO could not be initialized.");

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(&buttons, BUTTONS_INTERRUPT);

	XGpio_InterruptGlobalEnable(&buttons);

	// Enable GPIO interrupts in the controller
	XScuGic_Enable(&gic, XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR);

	std::cout << "Interrupt controller initialized." << std::endl;
}


int main() {
    std::cout << "Beginning audio program..." << std::endl;

    nluckett::Audio audio;

    std::cout << "I2S Initialization Complete." << std::endl;

    audio.unmute();

    init_gpio();

//    while(true) {
//    	std::vector<u32> data;
//
//    	u32 buttons_state = XGpio_DiscreteRead(&buttons, 1);
//
//    	if(buttons_state != 0) {
//    		std::cout << "Recording audio data." << std::endl;
//
//    		for(u32 rec_counter = 0; rec_counter < 2000; rec_counter++) {
//    		    //buttons_state = XGpio_DiscreteRead(&buttons, 1);
//
//    		    audio.record(data);
//    		}
//
//    		std::cout << "Recorded " << data.size() << " samples." << std::endl;
//
//    	}
//
//    	buttons_state = XGpio_DiscreteRead(&buttons, 1);
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
