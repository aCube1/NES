#ifndef _NES_MAPPER_NROM_HPP_
#define _NES_MAPPER_NROM_HPP_

#include "nes/Mapper.hpp"

namespace nes::mapper {
	class NROM : public Mapper {
		public:
			using Mapper::Mapper;

			u8 cpuRead(u16 addr) override;
			void cpuWrite(u16 addr, u8 data) override;

			u8 ppuRead(u16 addr) override;
			void ppuWrite(u16 addr, u8 data) override;
	};
} // namespace nes::mapper

#endif // _NES_MAPPER_NROM_HPP_
