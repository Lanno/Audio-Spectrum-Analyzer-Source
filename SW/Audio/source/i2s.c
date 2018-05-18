/*
 * audio.c
 *
 *  Contains all of the functions related to audio codec setup.
 */
 
#include <i2c.h>
#include <sleep.h>
#include <serial_comm.hpp>

/* ---------------------------------------------------------------------------- *
 * 									IicConfig()									*
 * ---------------------------------------------------------------------------- *
 * Initialises the IIC driver by looking up the configuration in the config
 * table and then initialising it. Also sets the IIC serial clock rate.
 * ---------------------------------------------------------------------------- */
unsigned char IicConfig(XIicPs *Iic, unsigned int DeviceIdPS)
{
	XIicPs_Config *Config;
	int Status;

	/* Initialise the IIC driver so that it's ready to use */

	// Look up the configuration in the config table
	Config = XIicPs_LookupConfig(DeviceIdPS);
	if(NULL == Config) {
		return XST_FAILURE;
	}

	// Initialise the IIC driver configuration
	Status = XIicPs_CfgInitialize(Iic, Config, Config->BaseAddress);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(Iic);
	if (Status != XST_SUCCESS) {
		send_message("IIC FAILED");
		return XST_FAILURE;
	}
	send_message("IIC PASSED");

	//Set the IIC serial clock rate.
	Status = XIicPs_SetSClk(Iic, IIC_SCLK_RATE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/* ---------------------------------------------------------------------------- *
 * 								AudioPllConfig()								*
 * ---------------------------------------------------------------------------- *
 * Reset CODEC Software and power-up. Set default ADC and DAC volumes and enable
 * the correct signal path to the DSP unit. Set a 48 kHz sampling rate and
 * activate the SSM CODEC.
 * ---------------------------------------------------------------------------- */
void AudioPllConfig(XIicPs *Iic) {


	AudioWriteToReg(Iic, R15_SOFTWARE_RESET, 				0b000000000); //Perform Reset
	usleep(75000);
	AudioWriteToReg(Iic, R6_POWER_MANAGEMENT, 				0b000110000); //Power Up
	AudioWriteToReg(Iic, R0_LEFT_CHANNEL_ADC_INPUT_VOLUME, 	0b000010111); //Default Volume
	AudioWriteToReg(Iic, R1_RIGHT_CHANNEL_ADC_INPUT_VOLUME, 0b000010111); //Default Volume
	AudioWriteToReg(Iic, R2_LEFT_CHANNEL_DAC_VOLUME, 		0b001111001);
	AudioWriteToReg(Iic, R3_RIGHT_CHANNEL_DAC_VOLUME, 		0b001111001);
	AudioWriteToReg(Iic, R4_ANALOG_AUDIO_PATH, 				0b000010010); //Allow Mixed DAC, Mute MIC
	AudioWriteToReg(Iic, R5_DIGITAL_AUDIO_PATH, 			0b000000111); //48 kHz Sampling Rate emphasis, no high pass
	AudioWriteToReg(Iic, R7_DIGITAL_AUDIO_I_F, 				0b001000010); //Master, I2S Mode, 16 bits
	AudioWriteToReg(Iic, R8_SAMPLING_RATE, 					0b000000000);
	usleep(75000);
	AudioWriteToReg(Iic, R9_ACTIVE, 						0b000000001); //Activate digital core
	AudioWriteToReg(Iic, R6_POWER_MANAGEMENT, 				0b000100010); //Output Power Up

}



/* ---------------------------------------------------------------------------- *
 * 								AudioWriteToReg									*
 * ---------------------------------------------------------------------------- *
 * Function to write to one of the registers from the audio
 * controller.
 * ---------------------------------------------------------------------------- */
void AudioWriteToReg(XIicPs *Iic, u8 u8RegAddr, u16 u16Data) {

	unsigned char u8TxData[2];

	u8TxData[0] = u8RegAddr << 1;
	u8TxData[0] = u8TxData[0] | ((u16Data >> 8) & 0b1);

	u8TxData[1] = u16Data & 0xFF;

	XIicPs_MasterSendPolled(Iic, u8TxData, 2, IIC_SLAVE_ADDR);
	while(XIicPs_BusIsBusy(Iic));
}

