/*
 * audio.hpp
 *
 *  Created on: Mar 31, 2018
 *      Author: noluc_000
 */

#ifndef INCLUDE_AUDIO_HPP_
#define INCLUDE_AUDIO_HPP_

#include <iostream>
#include <queue>
#include <stdexcept>

#include <xparameters.h>
#include <xgpio.h>
#include <xscugic.h>
#include <xil_exception.h>

#include <logii2s.h>
#include <i2c.h>

namespace nluckett {
	class Audio {
		logii2s_port i2s_base;

		logii2s_port i2s_tx;

		logii2s_port i2s_rx;

		XGpio *mute_ptr;

		XIicPs IIC;

		bool recording;

		bool playing;

		std::queue<u32> data;

	public:
		Audio(void);

		void enable_recording(void);

		void disable_recording(void);

		void enable_playback(void);

		void disable_playback(void);

		bool recording_enabled(void);

		bool playback_enabled(void);

		void record(void);

		void playback(void);

		void mute(void);

		void unmute(void);

		logii2s_port get_tx(void);

		logii2s_port get_rx(void);

	};
}

//void init_audio_interrupts(void);

#endif /* INCLUDE_AUDIO_HPP_ */
