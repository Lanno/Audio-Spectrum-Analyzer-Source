#include <serial_comm.hpp>

void send_message(std::string payload) {
	nlohmann::json message = {
		{"message", payload}
	};

	std::cout << message << std::endl;
}

void throw_exception(std::string payload) {
	nlohmann::json exception = {
		{"exception", payload}
	};

	throw std::runtime_error(payload);
}
