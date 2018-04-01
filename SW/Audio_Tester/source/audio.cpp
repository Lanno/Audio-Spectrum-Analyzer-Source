/*
 * audio.cpp
 *
 *  Created on: Mar 31, 2018
 *      Author: noluc_000
 */

#include <audio.hpp>

#define CORE_CLOCK_FREQ 100000000
#define SAMPLE_RATE         44100

namespace nluckett {
	Audio::Audio(void) {

		// Initialize the I2S structure.

		i2s.base         = XPAR_AUDIO_INTERFACE_BASEADDR;
		i2s.clock_freq   = 24;
		i2s.fifo_size    = 512;
		i2s.almost_full  = 170;
		i2s.almost_empty = 85;

		//

		u32 actual_fs = logii2s_port_init_clock(&i2s, CORE_CLOCK_FREQ, SAMPLE_RATE);

		xil_printf("The requested sampling rate is: %d", SAMPLE_RATE);
		xil_printf("The actual sampling rate is: %d", actual_fs);
	}

	u32 Audio::record(void) {
		return 0;
	}

	u32 Audio::playback(void) {
		return 0;
	}
}




