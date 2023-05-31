#include "CPU.hpp"

#include <cstdlib>
#include <spdlog/spdlog.h>

int main() {
	auto cpu { nes::CPU() };

	cpu.interpret(std::vector<uint8_t> { 0xa9, 0xfe, 0xaa, 0xe8, 0x00 });

	spdlog::info("Status : {:#08b}", cpu.getStatus());
	spdlog::info("Reg | A: {:#0x}", cpu.getA());
	spdlog::info("Reg | X: {:#0x}", cpu.getX());
	spdlog::info("Reg | Y: {:#0x}", cpu.getY());

	return EXIT_SUCCESS;
}
