#ifndef INCLUDE_SERIAL_COMM_HPP_
#define INCLUDE_SERIAL_COMM_HPP_


#include <iostream>
#include <string>
#include <deque>

#include <xil_types.h>

#include <json.hpp>

void send_message(std::string payload);

void send_data(std::deque<u32> payload);

void throw_exception(std::string payload);

#endif
