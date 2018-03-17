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

	static XGpio buttons;

	int status = XGpio_Initialize(&buttons, XPAR_BUTTONS_DEVICE_ID);

	if(status != XST_SUCCESS) throw std::runtime_error("The button GPIO could not be initialized.");

	XGpio_SetDataDirection(&buttons, 1, 0xF);

	// Interrupt controller initialization

	static XScuGic gic;

	XScuGic_Config *gic_config;

	gic_config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);

	status = XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress);

	if(status != XST_SUCCESS) throw std::runtime_error("The GIC configuration could not be initialized.");

	// Enable GPIO interrupts interrupt

	XGpio_InterruptEnable(&buttons, BUTTONS_INTERRUPT);

	XGpio_InterruptGlobalEnable(&buttons);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
								(Xil_ExceptionHandler) XScuGic_InterruptHandler,
								 &gic);

	Xil_ExceptionEnable();

	// Connect GPIO interrupt to handler

	status = XScuGic_Connect(&gic,
							 XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR,
							 (Xil_ExceptionHandler) button_handler,
							 (void*) &buttons);

	if(status != XST_SUCCESS) throw std::runtime_error("The button_handler could not be connected.");

	// Enable GPIO interrupts in the controller

	XGpio_InterruptClear(&buttons, BUTTONS_INTERRUPT);

	XGpio_InterruptEnable(&buttons, BUTTONS_INTERRUPT);

	XGpio_InterruptGlobalEnable(&buttons);

	XScuGic_Enable(&gic, XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR);
}


int main() {
    std::cout << "Beginning audio program..." << std::endl;

    nluckett::Audio audio;

    std::cout << "I2S initialization complete." << std::endl;

    audio.unmute();

    init_gpio();

    std::cout << "GPIO initialization complete." << std::endl;

    init_audio_interrupts(audio);

    std::cout << "Audio interrupt initialization complete." << std::endl;

    while(true) {
    	if(RECORD) {
    		std::cout << "Enabling audio recording..." << std::endl;

    		audio.disable_playback();

    		audio.enable_recording();

    		std::cout << "Recording enabled." << std::endl;

    	} else if(PLAYBACK) {
    		std::cout << "Enabling audio playback..." << std::endl;

    		audio.disable_recording();

    		audio.enable_playback();

    		std::cout << "Playback enabled." << std::endl;

    	} else {
    		audio.disable_recording();

    		audio.disable_playback();

    	}
    }

    return 0;
}
