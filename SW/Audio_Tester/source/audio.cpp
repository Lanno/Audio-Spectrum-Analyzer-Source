/*
 * audio.cpp
 *
 *  Created on: Mar 31, 2018
 *      Author: noluc_000
 */

#include <cmath>

#include <audio.hpp>

#define CORE_CLOCK_FREQ 100000000 // Hz
#define SAMPLE_RATE         48000 // samples / second
#define LONGEST_RECORD          5 // seconds

namespace nluckett {
    Audio::Audio(void)
    : data()
    {

        // Initialize the I2S structures.

        i2s_base.base       = XPAR_LOGII2S_BASEADDR;

        i2s_tx.base         = XPAR_LOGII2S_BASEADDR + LOGII2S_INST_OFFSET;
        i2s_tx.clock_freq   = 4000000;
        i2s_tx.fifo_size    = 512;
        i2s_tx.almost_full  = 170;
        i2s_tx.almost_empty = 85;

        i2s_rx.base         = XPAR_LOGII2S_BASEADDR + 2 * LOGII2S_INST_OFFSET;
        i2s_rx.clock_freq   = 4000000;
        i2s_rx.fifo_size    = 512;
        i2s_rx.almost_full  = 170;
        i2s_rx.almost_empty = 85;

        recording = false;
        playing   = false;

        std::cout << "LogiCore I2S Version: " << logii2s_port_get_version(&i2s_base) << std::endl;
        std::cout << "Instance 0 Direction: " << logii2s_port_direction(&i2s_rx)      << std::endl;
        std::cout << "Instance 1 Direction: " << logii2s_port_direction(&i2s_rx)      << std::endl;

        u32 actual_fs_0 = logii2s_port_init_clock(&i2s_tx, CORE_CLOCK_FREQ, SAMPLE_RATE);
        u32 actual_fs_1 = logii2s_port_init_clock(&i2s_rx, CORE_CLOCK_FREQ, SAMPLE_RATE);

        if(actual_fs_0 == 0) throw std::runtime_error("Instance 0's control register is not configured to be a clock and word select master.");

        if(actual_fs_1 == 0) throw std::runtime_error("Instance 1's control register is not configured to be a clock and word select master.");

        std::cout << "The requested sampling rate is: "       << SAMPLE_RATE << std::endl;
        std::cout << "Instance 0's actual sampling rate is: " << actual_fs_0 << std::endl;
        std::cout << "Instance 1's actual sampling rate is: " << actual_fs_1 << std::endl;

        // Initialize the mute

        int status = XGpio_Initialize(mute_ptr, XPAR_MUTE_DEVICE_ID);

        if(status != XST_SUCCESS) throw std::runtime_error("The Mute GPIO could not be initialized.");

        // I2C Stuff

        IicConfig(&IIC, XPAR_PS7_I2C_0_DEVICE_ID);

        AudioPllConfig(&IIC);
    }

    void Audio::enable_recording(void) {
		recording = true;

		logii2s_port_clear_isr(&i2s_rx, LOGII2S_INT_ACK_ALL);

		logii2s_port_unmask_int(&i2s_rx, LOGII2S_INT_FAF);

		logii2s_port_enable_xfer(&i2s_rx);

    }

    void Audio::disable_recording(void) {
		recording = false;

		logii2s_port_disable_xfer(&i2s_rx);

		logii2s_port_mask_int(&i2s_rx, LOGII2S_INT_FAF);

		logii2s_port_clear_isr(&i2s_rx, LOGII2S_INT_ACK_ALL);

	}

    void Audio::enable_playback(void) {
		playing = true;

		logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);

		logii2s_port_unmask_int(&i2s_tx, LOGII2S_INT_FAE);

		logii2s_port_enable_xfer(&i2s_tx);

    }

    void Audio::disable_playback(void) {
		playing = false;

		logii2s_port_disable_xfer(&i2s_tx);

		logii2s_port_mask_int(&i2s_tx, LOGII2S_INT_FAE);

		logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);

	}

    bool Audio::recording_enabled(void) {
    	return recording;
    }

    bool Audio::playback_enabled(void) {
		return playing;
	}

    void Audio::mute(void) {
    	XGpio_DiscreteWrite(mute_ptr, 1, 0x00000000);
    }

    void Audio::unmute(void) {
    	XGpio_DiscreteWrite(mute_ptr, 1, 0xFFFFFFFF);
    }

    logii2s_port Audio::get_tx(void) {
    	return i2s_tx;
    }

    logii2s_port Audio::get_rx(void) {
		return i2s_rx;
	}

    void Audio::record(void) {
    	logii2s_port_clear_isr(&i2s_rx, LOGII2S_INT_ACK_ALL);

    	for(u32 idx = 0; idx < i2s_rx.almost_full; idx++) {
			u32 sample = logii2s_port_read_fifo_word(&i2s_rx);

			if(data.size() < (SAMPLE_RATE * LONGEST_RECORD)) {
				data.push(sample);
				std::cout << std::hex << sample << ", ";
			}
    	}
    }

    void Audio::playback(void) {
		for(u32 idx = 0; idx < (i2s_tx.fifo_size - i2s_tx.almost_empty); idx++) {
			if(data.size() > 0) {
				u32 sample = data.front();

				data.pop();

				logii2s_port_write_fifo_word(&i2s_tx, sample);

			} else {
				logii2s_port_write_fifo_word(&i2s_tx, 0);
			}
		}

		logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);
	}

    void Audio::play_reference(void) {
    	static float phase = 0;

    	const float phase_step = 6.28 / 1e6;

		for(u32 idx = 0; idx < (i2s_tx.fifo_size - i2s_tx.almost_empty); idx++) {
			double value = sin(phase);

			u32 sample = u32(value * (1 << 30));

			if(value > 0)
				sample += (1 << 31);

			//u32 sample = (half_sample << 12) | half_sample;

			//std::cout << std::hex << sample << ", ";

			logii2s_port_write_fifo_word(&i2s_tx, sample);

			phase += phase_step;
		}

		logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);
	}

    void Audio::clear_interrupts(void) {
    	logii2s_port_clear_isr(&i2s_rx, LOGII2S_INT_ACK_ALL);

    	logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);
    }

    u32 Audio::size(void) {
    	return data.size();
    }

	void audio_handler(void *audio_instance) {
		Audio* audio = (Audio*)audio_instance;

		if(audio->recording_enabled()) {
			audio->record();

		} else if(audio->playback_enabled()) {
			audio->play_reference();

		} else {
			std::cout << "Bad audio interrupt." << std::endl;

			audio->clear_interrupts();
		}

	}

	void init_audio_interrupts(Audio& audio_instance, XScuGic& gic) {
		// Connect device interrupt to handler

		u32 status = XScuGic_Connect(&gic,
								 XPAR_FABRIC_LOGII2S_INTERRUPT_INTR,
								 (Xil_ExceptionHandler) audio_handler,
								 (void*) &audio_instance);

		if(status != XST_SUCCESS) throw std::runtime_error("The audio_handler could not be connected.");

		// Enable LOGII2S interrupts in the controller

		XScuGic_Enable(&gic, XPAR_FABRIC_LOGII2S_INTERRUPT_INTR);
	}
}



