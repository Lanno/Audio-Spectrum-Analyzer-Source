/*
 * audio.cpp
 *
 *  Created on: Mar 31, 2018
 *      Author: noluc_000
 */

#include <audio.hpp>

#include <xparameters.h>
#include <xgpio.h>
#include <xscugic.h>
#include <xil_exception.h>

#define CORE_CLOCK_FREQ 100000000
#define SAMPLE_RATE         48000

namespace nluckett {
    Audio::Audio(void) {

        // Initialize the I2S structures.

        i2s_base.base         = XPAR_LOGII2S_BASEADDR;

        i2s_tx.base         = XPAR_LOGII2S_BASEADDR + LOGII2S_INST_OFFSET;
        i2s_tx.clock_freq   = 12288000;
        i2s_tx.fifo_size    = 512;
        i2s_tx.almost_full  = 170;
        i2s_tx.almost_empty = 85;

        i2s_rx.base         = XPAR_LOGII2S_BASEADDR + 2 * LOGII2S_INST_OFFSET;
        i2s_rx.clock_freq   = 12288000;
        i2s_rx.fifo_size    = 512;
        i2s_rx.almost_full  = 170;
        i2s_rx.almost_empty = 85;

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

    void Audio::record(std::vector<u32> &data) {
		logii2s_port_clear_isr(&i2s_rx, LOGII2S_INT_ACK_ALL);

		logii2s_port_unmask_int(&i2s_rx, LOGII2S_INT_FAF);

		logii2s_port_enable_xfer(&i2s_rx);

    }

    void Audio::playback(std::vector<u32> &data) {
		logii2s_port_clear_isr(&i2s_tx, LOGII2S_INT_ACK_ALL);

		logii2s_port_unmask_int(&i2s_tx, LOGII2S_INT_FAE);

		logii2s_port_enable_xfer(&i2s_tx);
    }

    void Audio::mute(void) {
    	XGpio_DiscreteWrite(mute_ptr, 1, 0x00000000);
    }

    void Audio::unmute(void) {
    	XGpio_DiscreteWrite(mute_ptr, 1, 0xFFFFFFFF);
    }
}

//void audio_interrupt_handler(void *InstancePtr) {
//
//}
//
//void init_audio_interrupts(void) {
//	// Interrupt controller initialization
//	XScuGic gic;
//
//	XScuGic_Config *gic_config;
//
//	gic_config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
//
//	int status = XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress);
//
//	if(status != XST_SUCCESS) throw std::runtime_error("The Mute GPIO could not be initialized.");
//
//	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
//								(Xil_ExceptionHandler) XScuGic_InterruptHandler,
//								 &gic);
//
//	Xil_ExceptionEnable();
//
//	// Connect device interrupt to handler
//	status = XScuGic_Connect(&gic,
//							 XPAR_FABRIC_LOGII2S_INTERRUPT_INTR,
//							 (Xil_ExceptionHandler) audio_interrupt_handler,
//							 (void *) &BUTTONS);
//
//	if(status != XST_SUCCESS) throw std::runtime_error("The Mute GPIO could not be initialized.");
//
//	// Enable GPIO interrupts interrupt
//	XGpio_InterruptEnable(&BUTTONS, BUTTONS_INTERRUPT);
//
//	XGpio_InterruptGlobalEnable(&BUTTONS);
//
//	// Enable GPIO interrupts in the controller
//	XScuGic_Enable(&gic, XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR);
//}



