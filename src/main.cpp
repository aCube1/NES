#include "Bus.hpp"

#include <cstdlib>
#include <spdlog/spdlog.h>

int main() {
	try {

	} catch (const std::exception& e) {
		spdlog::error("{}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
