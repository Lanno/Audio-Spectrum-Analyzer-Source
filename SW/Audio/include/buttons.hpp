#ifndef INCLUDE_BUTTONS_HPP_
#define INCLUDE_BUTTONS_HPP_

#include <sstream>
#include <iostream>

#include <xparameters.h>
#include <xgpio.h>
#include <xscugic.h>

#include <serial_comm.hpp>

void button_handler(void *InstancePtr);

void init_gpio(XScuGic& gic);

#endif
