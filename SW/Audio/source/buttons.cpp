#include <buttons.hpp>

#define BUTTONS_INTERRUPT 1

bool RECORD   = false;
bool PLAYBACK = false;
bool ANALYZE  = false;

void button_handler(void *InstancePtr) {
	// Disable GPIO interrupts
	XGpio_InterruptDisable((XGpio*) InstancePtr, BUTTONS_INTERRUPT);

	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus((XGpio*) InstancePtr) & BUTTONS_INTERRUPT) != BUTTONS_INTERRUPT)
		return;

	u32 button_value = XGpio_DiscreteRead((XGpio*) InstancePtr, 1);

	if     ((button_value & 0x1) == 0x1 && RECORD == false) RECORD = true;
	else if((button_value & 0x1) == 0x0 && RECORD == true ) RECORD = false;

	if     ((button_value & 0x2) == 0x2 && PLAYBACK == false) PLAYBACK = true;
	else if((button_value & 0x2) == 0x0 && PLAYBACK == true ) PLAYBACK = false;

	if     ((button_value & 0x4) == 0x4 && ANALYZE == false) ANALYZE = true;
	else if((button_value & 0x4) == 0x0 && ANALYZE == true ) ANALYZE = false;

    XGpio_InterruptClear((XGpio*) InstancePtr, BUTTONS_INTERRUPT);

    // Enable GPIO interrupts
    XGpio_InterruptEnable((XGpio*) InstancePtr, BUTTONS_INTERRUPT);
}

void init_gpio(XScuGic& gic) {
	// Setup the buttons with their interrupts

	static XGpio buttons;

	int status = XGpio_Initialize(&buttons, XPAR_BUTTONS_DEVICE_ID);

	if(status != XST_SUCCESS)
		throw_exception("The button GPIO could not be initialized.");


	XGpio_SetDataDirection(&buttons, 1, 0xF);

	// Connect GPIO interrupt to handler

	status = XScuGic_Connect(&gic,
							 XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR,
							 (Xil_ExceptionHandler) button_handler,
							 (void*) &buttons);

	if(status != XST_SUCCESS)
		throw_exception("The button GPIO handler could not be connected.");

	// Enable GPIO interrupts in the controller

	XGpio_InterruptClear(&buttons, BUTTONS_INTERRUPT);

	XGpio_InterruptEnable(&buttons, BUTTONS_INTERRUPT);

	XGpio_InterruptGlobalEnable(&buttons);

	XScuGic_Enable(&gic, XPAR_FABRIC_BUTTONS_IP2INTC_IRPT_INTR);
}
