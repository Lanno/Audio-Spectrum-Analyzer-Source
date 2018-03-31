/*
 * audio.hpp
 *
 *  Created on: Mar 31, 2018
 *      Author: noluc_000
 */

#ifndef INCLUDE_AUDIO_HPP_
#define INCLUDE_AUDIO_HPP_

namespace nluckett {
	class Audio {
	public:
		Audio(void);

		unsigned int record(void);

		unsigned int playback(void);
	};
}

#endif /* INCLUDE_AUDIO_HPP_ */
