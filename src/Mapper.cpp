#include "Mapper.hpp"

#include "mapper/MapperNROM.hpp"
#include "spdlog/spdlog.h"

namespace nes {
	std::unique_ptr<Mapper> Mapper::create(Cartridge cartridge) {
		switch (cartridge.mapper_id) {
		case 0:
			return std::make_unique<MapperNROM>(std::move(cartridge));
		default:
			spdlog::error("No mapper available!");
			return {};
		}
	}

	Mapper::Mapper(Cartridge cartridge)
		: m_cartridge(std::move(cartridge)) {}

	u8 Mapper::prgBanks() const {
		return m_cartridge.prg_banks;
	}

	u8 Mapper::chrBanks() const {
		return m_cartridge.chr_banks;
	}

	Cartridge::Mirroring Mapper::mirroringType() const {
		return m_cartridge.mirroring;
	}
} // namespace nes
