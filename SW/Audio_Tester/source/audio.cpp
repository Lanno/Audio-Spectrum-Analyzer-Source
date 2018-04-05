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

		// Initialize the I2S structures.

		i2s_base.base         = XPAR_LOGII2S_BASEADDR;

		i2s_0.base         = XPAR_LOGII2S_BASEADDR + LOGII2S_INST_OFFSET;
		i2s_0.clock_freq   = 10000000;
		i2s_0.fifo_size    = 512;
		i2s_0.almost_full  = 170;
		i2s_0.almost_empty = 85;

		i2s_1.base         = XPAR_LOGII2S_BASEADDR + 2 * LOGII2S_INST_OFFSET;
		i2s_1.clock_freq   = 10000000;
		i2s_1.fifo_size    = 512;
		i2s_1.almost_full  = 170;
		i2s_1.almost_empty = 85;

		std::cout << "LogiCore I2S Version: " << logii2s_port_get_version(&i2s_base) << std::endl;
		std::cout << "Instance 0 Direction: " << logii2s_port_direction(&i2s_0)      << std::endl;
		std::cout << "Instance 1 Direction: " << logii2s_port_direction(&i2s_1)      << std::endl;

		u32 actual_fs_0 = logii2s_port_init_clock(&i2s_0, CORE_CLOCK_FREQ, SAMPLE_RATE);
		u32 actual_fs_1 = logii2s_port_init_clock(&i2s_1, CORE_CLOCK_FREQ, SAMPLE_RATE);

		if(actual_fs_0 == 0)
			throw("Instance 0's control register is not configured to be a clock and word select master.");

		if(actual_fs_1 == 0)
			throw("Instance 1's control register is not configured to be a clock and word select master.");

		std::cout << "The requested sampling rate is: "       << SAMPLE_RATE << std::endl;
		std::cout << "Instance 0's actual sampling rate is: " << actual_fs_0 << std::endl;
		std::cout << "Instance 1's actual sampling rate is: " << actual_fs_1 << std::endl;
	}

	u32 Audio::record(void) {
		return 0;
	}

	u32 Audio::playback(void) {
		return 0;
	}
}




