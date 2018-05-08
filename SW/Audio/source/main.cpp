/////////////////////////////////////////////////////////////////////
// The audio tester project will initialize the Zybo's audio codec
// via the audio interface IP, and record and play back audio automatically
// every 5 seconds.
//
// Author: Nolan Luckett
// Organization: CSULB
// Class: CECS561
// Project: Final Project, Basic System

#include <sstream>
#include <vector>

#include <xparameters.h>
#include <xgpio.h>
#include <xscugic.h>
#include <xil_exception.h>
#include <json.hpp>

#include <audio.hpp>
#include <serial_comm.hpp>

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

	std::stringstream payload;

	payload << "Record: " << RECORD << "Playback: " << PLAYBACK;

	//send_message(payload);

    XGpio_InterruptClear((XGpio*) InstancePtr, BUTTONS_INTERRUPT);

    // Enable GPIO interrupts
    XGpio_InterruptEnable((XGpio*) InstancePtr, BUTTONS_INTERRUPT);
}

void init_gic(XScuGic& gic) {
	XScuGic_Config *gic_config;

	gic_config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);

	u32 status = XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress);

	if(status != XST_SUCCESS)
		throw_exception("The GIC configuration could not be initialized.");


	// Enable GPIO interrupts interrupt

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
								(Xil_ExceptionHandler) XScuGic_InterruptHandler,
								 &gic);

	Xil_ExceptionEnable();
}

void init_gpio(XScuGic& gic) {
	// Setup the buttons with their interrupts

	static XGpio buttons;

	int status = XGpio_Initialize(&buttons, XPAR_BUTTONS_DEVICE_ID);

	if(status != XST_SUCCESS)
		throw_exception("The button GPIO could not be initialized.");


	XGpio_SetDataDirection(&buttons, 1, 0xF);

	// Connect GPIO interrupt to handler

	status = XScuGic_Connect(&gic,
							 XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR,
							 (Xil_ExceptionHandler) button_handler,
							 (void*) &buttons);

	if(status != XST_SUCCESS)
		throw_exception("The button GPIO could not be initialized.");

	// Enable GPIO interrupts in the controller

	XGpio_InterruptClear(&buttons, BUTTONS_INTERRUPT);

	XGpio_InterruptEnable(&buttons, BUTTONS_INTERRUPT);

	XGpio_InterruptGlobalEnable(&buttons);

	XScuGic_Enable(&gic, XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR);
}


int main() {
    send_message("Beginning audio program...");

    nluckett::Audio audio;

	send_message("I2S initialization complete.");

    audio.unmute();

    XScuGic gic;

    init_gic(gic);

    send_message("GIC initialization complete.");

    init_gpio(gic);

    send_message("GPIO initialization complete.");

    init_audio_interrupts(audio, gic);

    send_message("Audio interrupt initialization complete.");

    while(true) {
    	if(RECORD) {
    		audio.disable_playback();

    		audio.enable_recording();

    	} else if(PLAYBACK) {
    		audio.disable_recording();

    		audio.enable_playback();

    	} else {
    		audio.disable_recording();

    		audio.disable_playback();

    	}
    }

    return 0;
}