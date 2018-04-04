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

		i2s.base         = XPAR_LOGII2S_BASEADDR;
		i2s.clock_freq   = 24;
		i2s.fifo_size    = 512;
		i2s.almost_full  = 170;
		i2s.almost_empty = 85;

		std::cout << "LogiCore I2S Version: " << logii2s_port_get_version(&i2s) << std::endl;

		//u32 actual_fs = logii2s_port_init_clock(&i2s, CORE_CLOCK_FREQ, SAMPLE_RATE);

		//std::cout << "The requested sampling rate is: " << SAMPLE_RATE << std::endl;
		//std::cout << "The actual sampling rate is: "    << actual_fs   << std::endl;
	}

	u32 Audio::record(void) {
		return 0;
	}

	u32 Audio::playback(void) {
		return 0;
	}
}




