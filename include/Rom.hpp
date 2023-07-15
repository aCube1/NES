#ifndef _NES_ROM_HPP_
#define _NES_ROM_HPP_

#include "types.hpp"

#include <optional>
#include <string_view>
#include <vector>

namespace nes {
	// Rom represents a NES cartridge.
	// It carried banks of ROM memory: PRG ROM for code and CHR ROM for visual graphics.
	struct Rom {
		public:
			static std::optional<Rom> loadFile(std::string_view path);

			enum Mirroring {
				HORIZONTAL,
				VERTICAL,
				ONE_SCREEN_LO,
				ONE_SCREEN_HI,
			};

			u8 prg_banks;
			u8 chr_banks;
			u8 mapper_id;

			Mirroring mirroring;

			std::vector<u8> prg_data;
			std::vector<u8> chr_data;
	};

} // namespace nes

#endif // _NES_ROM_HPP_
