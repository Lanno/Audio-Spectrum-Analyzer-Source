#include <serial_comm.hpp>

void send_message(std::string payload) {
	nlohmann::json message = {
		{"message", payload}
	};

	std::cout << message << std::endl;
}

void send_data(std::deque<u32> payload) {
	nlohmann::json message;

	message["data"] = payload;

	std::cout << message << std::endl;
}

void throw_exception(std::string payload) {
	nlohmann::json exception = {
		{"error", payload}
	};

	throw std::runtime_error(exception.dump());
}
