#include "CPU.hpp"

#include <cstdlib>
#include <spdlog/spdlog.h>

int main() {
	try {
		auto cpu { nes::CPU() };

		cpu.load({ 0xa9, 0xfe, 0xaa, 0xe8, 0x00 });

		// TODO: Better emulation loop.
		while ((cpu.getStatus() & (1 << 4)) == 0) {
			cpu.clock();
		}
	} catch (const std::exception& e) {
		spdlog::error("{}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
