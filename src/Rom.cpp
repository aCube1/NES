#include "Rom.hpp"

#include "spdlog/spdlog.h"

#include <fstream>

namespace {
	// iNES format header:
	// 	name - Constant $4E $45 $53 $1A (ASCII "NES" followed by MS-DOS end-of-file).
	// 	prg_banks - Size of PRG ROM in 16KB units.
	// 	chr_banks -  Size of CHR ROM in 8KB units (value 0 means the board uses CHR RAM).
	// 	flag6 - Mapper, mirroring, battery, trainer
	// 	flag7 - Mapper, VS/Playchoice, NES 2.0
	// 	flag8 - PRG-RAM size (rarely used extension)
	// 	flag9 - TV system (rarely used extension)
	// 	flag10 - TV system, PRG-RAM presence (unofficial, rarely used extension)
	// 	padding - Unused padding (should be filled with zero, but some rippers put their
	// 	name across bytes 7-15)
	struct INESHeader {
			u32 name;
			u8 prg_banks;
			u8 chr_banks;
			u8 flag6;
			u8 flag7;
			u8 flag8;
			u8 flag9;
			u8 flag10;
			u8 padding[5];
	};

	constexpr u32 constant_name { 0x1a54454e };
} // namespace

// NOTE: I think some machines will be unable to run cartridges, but...
static_assert(sizeof(INESHeader) == 16, "The Header is not 16 bytes!");

namespace nes {
	std::optional<Rom> Rom::loadFile(std::string_view path) {
		std::ifstream file(path.data(), std::ifstream::in | std::ifstream::binary);
		if (!file) {
			spdlog::error("Cannot open the nes file from {}", path);
			return {};
		}

		spdlog::info("Reading NES file...");

		INESHeader header {};
		file.read(reinterpret_cast<char *>(&header), sizeof(INESHeader));
		if (!file) {
			spdlog::error("Reading the NES file header failed!");
			return {};
		}

		if (header.name != constant_name) {
			spdlog::error(
				"Not a valid .nes file: the constant is {}, expect {}", header.name,
				constant_name
			);
			return {};
		}

		// Get mapper number.
		u8 mapper_lo = (header.flag6 >> 4) & 0x0F;
		u8 mapper_hi = (header.flag7 >> 4) & 0x0F;
		u8 mapper_id = (mapper_hi << 4) | mapper_lo;
		spdlog::info("Cartridge Mapper number is {}.", mapper_id);

		// Get mirroring type.
		// 0: horizontal (vertical arrangement) (CIRAM A10 = PPU A11)
		// 1: vertical (horizontal arrangement) (CIRAM A10 = PPU A10)
		Mirroring mirroring_type { header.flag6 & 0x01 ? VERTICAL : HORIZONTAL };
		spdlog::info(
			"Cartridge mirroring type is {}.",
			header.flag6 & 0x01 ? "Vertical" : "Horizontal"
		);

		// Persistent memory.
		if ((header.flag6 & 0x02)) {
			spdlog::info("Cartridge contains battery-backed PRG RAM ($6000-7fff) or "
			             "other Persistent memory.");
		}

		// Check if "trainer" is present.
		if (header.flag6 & 0x04) {
			spdlog::info("Cartridge have trainer! Skipping...");
			file.seekg(512, std::ios_base::cur);
		}

		// Read PRG data.
		std::vector<u8> prg_data(header.prg_banks * 0x4000); // PRG_BANKS * 16384
		file.read(reinterpret_cast<char *>(prg_data.data()), prg_data.size());
		if (!file) {
			spdlog::info("Failed to read PRG ROM data!");
			return {};
		}
		spdlog::info("Cartridge PRG ROM size is {} KB.", prg_data.size() / 1024);

		// Read CHR data.
		std::vector<u8> chr_data(header.chr_banks * 0x2000); // CHR_BANKS * 8192
		file.read(reinterpret_cast<char *>(chr_data.data()), chr_data.size());
		if (!file) {
			spdlog::info("Failed to read CHR ROM data!");
			return {};
		}
		spdlog::info("Cartridge CHR ROM size is {} KB.", chr_data.size() / 1024);

		file.close();
		return Rom {
			header.prg_banks, header.chr_banks,    mapper_id,
			mirroring_type,   std::move(prg_data), std::move(chr_data),
		};
	}
} // namespace nes
