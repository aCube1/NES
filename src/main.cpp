#include "CPU.hpp"

#include <cstdlib>
#include <spdlog/spdlog.h>

int main() {
	try {
		auto cpu { nes::CPU() };

		cpu.load({
			0xa9, 0xfe, // LDA $fe
			0xaa,		// TAX
			0xe8,		// INX
			0xe8,		// INX
			0x00,		// BRK
		});

		// TODO: Better emulation loop.
		while (((cpu.getStatus() & (1 << 4)) | (cpu.getStatus() & (1 << 2))) == 0) {
			cpu.clock();

			auto debug_str { cpu.getDebugString() };
			if (!debug_str.empty()) {
				spdlog::info(debug_str);
			}
		}
	} catch (const std::exception& e) {
		spdlog::error("{}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
