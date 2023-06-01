#include "CPU.hpp"

#include <cstdlib>
#include <spdlog/spdlog.h>

int main() {
	try {
		auto cpu { nes::CPU() };

		cpu.load({ 0xa9, 0xfe, 0xaa, 0xe8, 0x00 });
		cpu.run();

		spdlog::info("Status : {:#0b}", cpu.getStatus());
		spdlog::info("Reg | A: {:#0x}", cpu.getA());
		spdlog::info("Reg | X: {:#0x}", cpu.getX());
		spdlog::info("Reg | Y: {:#0x}", cpu.getY());
	} catch (const std::exception& e) {
		spdlog::error("{}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
