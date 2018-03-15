/*
 * audio.hpp
 *
 *  Created on: Mar 31, 2018
 *      Author: noluc_000
 */

#ifndef INCLUDE_AUDIO_HPP_
#define INCLUDE_AUDIO_HPP_

#include <iostream>
#include <vector>

#include <xparameters.h>
#include <xgpio.h>

#include <logii2s.h>
#include <i2c.h>

namespace nluckett {
	class Audio {
		logii2s_port i2s_base;

		logii2s_port i2s_tx;

		logii2s_port i2s_rx;

		XGpio *mute_ptr;

		XIicPs IIC;

	public:
		Audio(void);

		void record(std::vector<u32> &data);

		void playback(std::vector<u32> &data);

		void mute(void);

		void unmute(void);
	};
}

#endif /* INCLUDE_AUDIO_HPP_ */
