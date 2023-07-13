#include "Bus.hpp"

namespace nes {
	void Bus::powerUp() {
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

		if (addr >= 0x0000 && addr <= 0x1fff) {
			// System RAM Address range, mirrorred every 2048
			data = m_cpu_ram.at(addr & 0x07ff);
		} else if (addr >= 0x2000 && addr <= 0x3fff) {
			// TODO: Implement PPU!
		}

		return data;
	}

	u16 Bus::cpuRead16(u16 addr, bool ro) const {
		auto l { cpuRead(addr, ro) };
		auto h { cpuRead(addr + 1, ro) };

		return (h << 8) | l;
	}

	void Bus::cpuWrite(u16 addr, u8 data) {
		if (addr >= 0x0000 && addr <= 0x1fff) {
			// PPU Address range, mirrored every 8
			m_cpu_ram.at(addr & 0x07ff) = data;
		} else if (addr >= 0x2000 && addr <= 0x3fff) {
			// TODO: Implement PPU!
		}
	}

	void Bus::cpuWrite16(u16 addr, u16 data) {
		auto l { static_cast<u8>(data >> 8) };
		auto h { static_cast<u8>(data & 0xff) };

		cpuWrite(addr, l);
		cpuWrite(addr + 1, h);
	}
} // namespace nes
