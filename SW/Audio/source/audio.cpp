/*
 * audio.cpp
 *
 *  Created on: Mar 31, 2018
 *      Author: noluc_000
 */

#include <cmath>

#include <json.hpp>

#include <audio.hpp>
#include <serial_comm.hpp>

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

        // Initialize the mute

        int status = XGpio_Initialize(&mute_gpio, XPAR_MUTE_DEVICE_ID);

        if(status != XST_SUCCESS)
        	throw_exception("The Mute GPIO could not be initialized.");

        send_message("Mute GPIO initialized.");

        // I2C Stuff

        IicConfig(&IIC, XPAR_PS7_I2C_0_DEVICE_ID);

        AudioPllConfig(&IIC);
    }

    void Audio::enable_recording(void) {
		if(recording == false) {
			recording = true;

			logii2s_port_clear_isr(&i2s_rx, LOGII2S_INT_ACK_ALL);

			logii2s_port_unmask_int(&i2s_rx, LOGII2S_INT_FAF);

			logii2s_port_enable_xfer(&i2s_rx);

			send_message("Recording...");
		}
    }

    void Audio::disable_recording(void) {
    	if(recording == true) {
			recording = false;

			logii2s_port_disable_xfer(&i2s_rx);

			logii2s_port_mask_int(&i2s_rx, LOGII2S_INT_FAF);

			logii2s_port_clear_isr(&i2s_rx, LOGII2S_INT_ACK_ALL);

			send_message("Finished recording.");
    	}
	}

    void Audio::enable_playback(void) {
    	if(playing == false) {
			playing = true;

			logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);

			logii2s_port_unmask_int(&i2s_tx, LOGII2S_INT_FAE);

			logii2s_port_enable_xfer(&i2s_tx);

			send_message("Playing...");
    	}
    }

    void Audio::disable_playback(void) {
    	if(playing == true) {
			playing = false;

			logii2s_port_disable_xfer(&i2s_tx);

			logii2s_port_mask_int(&i2s_tx, LOGII2S_INT_FAE);

			logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);

			send_message("Finished playing.");
    	}
	}

    bool Audio::recording_enabled(void) {
    	return recording;
    }

    bool Audio::playback_enabled(void) {
		return playing;
	}

    void Audio::mute(void) {
    	XGpio_DiscreteWrite(&mute_gpio, 1, 0x00000000);
    }

    void Audio::unmute(void) {
    	XGpio_DiscreteWrite(&mute_gpio, 1, 0xFFFFFFFF);
    }

    logii2s_port Audio::get_tx(void) {
    	return i2s_tx;
    }

    logii2s_port Audio::get_rx(void) {
		return i2s_rx;
	}

    void Audio::record(void) {
    	for(u32 idx = 0; idx < i2s_rx.almost_full; idx++) {
			u32 sample = logii2s_port_read_fifo_word(&i2s_rx);

			if(data.size() < (SAMPLE_RATE * LONGEST_RECORD)) {
				data.push_back(sample);
			}
    	}

    	new_data = true;

    	// Clearing the interrupt after processing is critical.

    	logii2s_port_clear_isr(&i2s_rx, LOGII2S_INT_ACK_ALL);
    }

    void Audio::playback(void) {
		for(u32 idx = 0; idx < (i2s_tx.fifo_size - i2s_tx.almost_empty); idx++) {
			if(data.size() > 0) {
				u32 sample = data.front();

				data.pop_front();

				logii2s_port_write_fifo_word(&i2s_tx, sample);

			} else {
				logii2s_port_write_fifo_word(&i2s_tx, 0);
			}
		}

		// Clearing the interrupt after processing is critical.

		logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);
	}

    void Audio::play_reference(void) {
    	static float phase = 0;

    	static u32 sample = 0;

    	const float phase_step = 6.28 / 48;

		for(u32 idx = 0; idx < (i2s_tx.fifo_size - i2s_tx.almost_empty); idx++) {
			double value = sin(phase);

			u16 half_sample = u16(abs(value * (1 << 14)));

			if(value < 0)
				half_sample = ~half_sample + 0x0001;

			sample = (half_sample << 16) | half_sample;

			phase += phase_step;

			logii2s_port_write_fifo_word(&i2s_tx, sample);
		}

		// Clearing the interrupt after processing is critical.

		logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);
	}

    void Audio::clear_interrupts(void) {
    	logii2s_port_clear_isr(&i2s_rx, LOGII2S_INT_ACK_ALL);

    	logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);
    }

    u32 Audio::size(void) {
    	return data.size();
    }

    void Audio::send_serial(void) {
    	if(new_data == true) {
    		std::deque<u32> decimated;

    		for(u32 idx = 0; idx < data.size(); idx += 10){
    			decimated.push_back(data[idx]);
    		}

			send_data(decimated);

			new_data = false;
		}
    }

	void audio_handler(void *audio_instance) {
		Audio* audio = (Audio*)audio_instance;

		if(audio->recording_enabled()) {
			audio->record();

		} else if(audio->playback_enabled()) {
			audio->playback();

		} else {
			send_message("Bad audio interrupt.");

			audio->clear_interrupts();
		}

	}

	void init_audio_interrupts(Audio& audio_instance, XScuGic& gic) {
		// Connect device interrupt to handler

		u32 status = XScuGic_Connect(&gic,
								 XPAR_FABRIC_LOGII2S_INTERRUPT_INTR,
								 (Xil_ExceptionHandler) audio_handler,
								 (void*) &audio_instance);

		if(status != XST_SUCCESS)
			throw_exception("The audio_handler could not be connected.");

		// Enable LOGII2S interrupts in the controller

		XScuGic_Enable(&gic, XPAR_FABRIC_LOGII2S_INTERRUPT_INTR);
	}
}



