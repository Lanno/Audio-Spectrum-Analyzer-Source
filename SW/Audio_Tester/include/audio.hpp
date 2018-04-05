/*
 * audio.hpp
 *
 *  Created on: Mar 31, 2018
 *      Author: noluc_000
 */

#ifndef INCLUDE_AUDIO_HPP_
#define INCLUDE_AUDIO_HPP_

#include <iostream>

#include <xparameters.h>

#include <logii2s.h>

namespace nluckett {
	class Audio {
		logii2s_port i2s_base;

		logii2s_port i2s_0;

		logii2s_port i2s_1;

	public:
		Audio(void);

		u32 record(void);

		u32 playback(void);
	};
}

#endif /* INCLUDE_AUDIO_HPP_ */
