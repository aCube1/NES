#include "Bus.hpp"

#include <cassert>
#include <utility>

namespace nes {
	void Bus::insert(Cartridge cartridge) {
		m_cartridge = std::move(cartridge);
	}

	void Bus::power() {
		m_cpu.connectBus(this);

		reset();
	}

	void Bus::reset() {
		m_cpu.reset();

		m_clock = 0;
	}

	void Bus::clock() {
		if (m_clock % 3 == 0) {
			m_cpu.clock();
		}

		m_clock += 1;
	}

	u8 Bus::cpuRead(u16 addr, bool /* ro */) const {
		u8 data { 0x00 };

		if (addr >= 0x0000 && addr < 0x2000) {
			// System RAM Address range, mirrorred every 2048
			data = m_cpu_ram.at(addr & 0x07ff);
		} else if (addr >= 0x2000 && addr < 0x4000) {
			// TODO: Implement PPU!
		} else if (addr >= 0x4000 && addr < 0x4018) {
			// TODO: Implement APU and Joysticks!
		} else if (addr >= 0x4018 && addr < 0x4020) {
			// APU and I/0 functionality
			assert(0); // But it's normally disabled
		} else {
			// TODO: Implement mappers!
			data = m_cartridge.prg_data.at(addr & 0x3fff);
		}

		return data;
	}

	u16 Bus::cpuRead16(u16 addr, bool ro) const {
		auto l { cpuRead(addr, ro) };
		auto h { cpuRead(addr + 1, ro) };

		return (h << 8) | l;
	}

	void Bus::cpuWrite(u16 addr, u8 data) {
		if (addr >= 0x0000 && addr < 0x2000) {
			// PPU Address range, mirrored every 8
			m_cpu_ram.at(addr & 0x07ff) = data;
		} else if (addr >= 0x2000 && addr < 0x4000) {
			// TODO: Implement PPU!
		} else if (addr >= 0x4000 && addr < 0x4018) {
			// TODO: Implement APU and Joysticks!
		} else if (addr >= 0x4018 && addr < 0x4020) {
			// APU and I/0 functionality
			assert(0); // But it's normally disabled
		} else {
			// TODO: Implement mappers!
		}
	}
} // namespace nes
