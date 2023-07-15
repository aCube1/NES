#include "Bus.hpp"

#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>

namespace nes {
	void runDebug(Bus& bus) {
		auto& cpu { bus.getCPU() };
		cpu.setPC(0x8000);

		do {
			bus.clock();

			if (cpu.getCycles() == 0) {
				spdlog::info("{}", cpu.getDebugString());
			}
		} while (true);
	}
} // namespace nes

int main() {
	spdlog::set_pattern("%X %^[%L]%$ %v");

	try {
		auto bus { nes::Bus() };
		auto cartridge { nes::Cartridge::loadFile("test/nestest.nes") };
		bus.insert(cartridge.value());
		bus.power();

		// TODO: Create engine.
		nes::runDebug(bus);
	} catch (const std::exception& e) {
		spdlog::error("{}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
