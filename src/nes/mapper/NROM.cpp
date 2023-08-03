#include "nes/mapper/NROM.hpp"

#include "spdlog/spdlog.h"

namespace nes::mapper {
	u8 NROM::cpuRead(u16 addr) {
		u32 mapped_addr {};

		if (addr >= 0x8000 && addr <= 0xffff) {
			mapped_addr = addr & (prgBanks() > 1 ? 0x7fff : 0x3fff);
		}

		return m_cartridge.prg_data.at(mapped_addr);
	}

	void NROM::cpuWrite(u16 addr, u8 data) {
		spdlog::warn("ROM memory write attempt at: {:#06x} to set {:#04x}", addr, data);
	}

	u8 NROM::ppuRead(u16 addr) {
		// There is no mapping required for PPU
		if (addr >= 0x0000 && addr < 0x2000) {
			return m_cartridge.chr_data.at(addr);
		}
		return 0x00;
	}

	void NROM::ppuWrite(u16 addr, u8 data) {
		if (addr >= 0x0000 && addr < 0x2000) {
			if (m_cartridge.chr_banks == 0) {
				m_cartridge.chr_data.at(addr) = data;
			}
		}
	}
} // namespace nes::mapper
