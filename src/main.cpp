#include "Bus.hpp"

#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>

namespace nes {
	void runDebug(Bus& bus) {
		auto& cpu { bus.getCPU() };
		cpu.setPC(0xc000);

		do {
			bus.clock();

			if (cpu.getCycles() == 0) {
				spdlog::info("{}", cpu.getDebugString());
				std::cin.get();
			}
		} while (true);
	}
} // namespace nes

int main(int argc, char *argv[]) {
	spdlog::set_pattern("%X %^[%L]%$ %v");

	if (argc != 2) {
		spdlog::error("Usage: {} <rom>", argv[0]);
		return EXIT_FAILURE;
	}

	try {
		auto bus { nes::Bus() };
		auto cartridge { nes::Cartridge::loadFile(argv[1]) };
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
