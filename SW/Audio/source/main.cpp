/////////////////////////////////////////////////////////////////////
// The audio tester project will initialize the Zybo's audio codec
// via the audio interface IP, and record and play back audio automatically
// every 5 seconds.
//
// Author: Nolan Luckett
// Organization: CSULB
// Class: CECS561
// Project: Final Project, Basic System

#include <vector>

#include <xparameters.h>
#include <xgpio.h>
#include <xscugic.h>
#include <xil_exception.h>
#include <json.hpp>

#include <audio.hpp>
#include <buttons.hpp>
#include <serial_comm.hpp>

extern bool RECORD;
extern bool PLAYBACK;
extern bool ANALYZE;

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


int main(void) {
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

    	} else if(ANALYZE) {
    		audio.send_serial();

    	} else {
    		audio.disable_recording();

    		audio.disable_playback();

    	}
    }

    return 0;
}
