#include "CPU.hpp"

#include <cstdlib>
#include <spdlog/spdlog.h>

int main() {
	try {
		auto cpu { nes::CPU() };

		// TODO: Add cartridge loading.
		cpu.load({
			0xa9, 0xf0, // LDA #$f0
			0xaa,		// TAX
			0xe8,		// INX
			0x18,		// CLC
			0xe8,		// INX
			0x69, 0x11, // ADC #$11
			0xe9, 0x02, // SBC #$01
			0x08,		// PHP
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
