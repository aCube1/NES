#include "Bus.hpp"

#include <cstdlib>
#include <curses.h>
#include <spdlog/spdlog.h>

namespace nes {
	void runDebug(Bus& bus) {
		auto& cpu { bus.getCPU() };
		(void)cpu;

		// initscr();
		// raw();
		// while (true) {
		// 	bus.clock();
		// 	printw("%s", cpu.getDebugString().c_str());
		// 	refresh();
		// 	getch();
		// }
		// endwin();
	}
} // namespace nes

int main() {
	try {
		auto bus { nes::Bus() };
		bus.load({
			0xa9, 0xf0,       // LDA #$f0
			0xaa,             // TAX
			0x85, 0x00, 0x20, // STA $2000
			0x69, 0x11,       // ADC #$11
			0xe9, 0x02,       // SBC #$01
			0x00,             // BRK
		});
		bus.power();

		nes::runDebug(bus);
	} catch (const std::exception& e) {
		spdlog::error("{}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
