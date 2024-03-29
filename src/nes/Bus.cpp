#include "nes/Bus.hpp"

#include <cassert>
#include <stdexcept>
#include <utility>

namespace nes {
	void Bus::insert(Cartridge cartridge) {
		m_mapper = Mapper::create(std::move(cartridge));
		if (m_mapper == nullptr) {
			throw std::runtime_error("No supported cartridge!");
		}
	}

	void Bus::power() {
		m_cpu.connectBus(this);

		reset();
	}

	void Bus::reset() {
		m_cpu.reset();

		m_clock = 0;
	}

	u32 Bus::clock() {
		if (m_clock % 3 == 0) {
			m_cpu.clock();
		}

		m_clock += 1;
		return m_clock;
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
			// But it's normally disabled
		} else {
			// Cartridge space: PRG ROM, PRG RAM, and mapper registers
			data = m_mapper->cpuRead(addr);
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
			// But it's normally disabled
		} else {
			// Save RAM and PRG ROM that stored in cartridge
			m_mapper->cpuWrite(addr, data);
		}
	}
} // namespace nes
