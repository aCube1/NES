#ifndef _NES_MAPPER_HPP_
#define _NES_MAPPER_HPP_

#include "Cartridge.hpp"

#include <memory>

namespace nes {
	class Mapper {
		public:
			static std::unique_ptr<Mapper> create(Cartridge cartridge);

			explicit Mapper(Cartridge cartridge);
			virtual ~Mapper() = default;

			virtual u8 cpuRead(u16 addr) = 0;
			virtual void cpuWrite(u16 addr, u8 data) = 0;

			virtual u8 ppuRead(u16 addr) = 0;
			virtual void ppuWrite(u16 addr, u8 data) = 0;

		protected:
			[[nodiscard]] u8 prgBanks() const;
			[[nodiscard]] u8 chrBanks() const;
			[[nodiscard]] Cartridge::Mirroring mirroringType() const;

			Cartridge m_cartridge;
	};
} // namespace nes

#endif // _NES_MAPPER_HPP_
