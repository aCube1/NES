#ifndef _NES_BUS_HPP_
#define _NES_BUS_HPP_

#include "CPU.hpp"
#include "types.hpp"

#include <array>
#include <vector>

#define NES_RAM_SIZE 2048

namespace nes {
	class Bus {
		public:
			Bus() = default;
			Bus(const Bus&) = delete;
			Bus& operator=(const Bus&) = delete;

			// TODO: Proper mapper.
			// Emulate Mapper0 just for testing purposes.
			inline void load(const std::vector<u8>& program) {
				std::copy(program.begin(), program.end(), m_cpu_ram.begin() + 0x8000);
				m_cpu_ram.at(0xfffc) = 0x00;
				m_cpu_ram.at(0xfffd) = 0x80;
			}

			void power();
			void reset();
			void clock();

			[[nodiscard]] u8 cpuRead(u16 addr, bool ro = false) const;
			[[nodiscard]] u16 cpuRead16(u16 addr, bool ro = false) const;
			void cpuWrite(u16 addr, u8 data);

		private:
			CPU m_cpu;

			// Count how many clocks have passed.
			u32 m_clock { 0 };

			// Memory
			std::array<u8, NES_RAM_SIZE> m_cpu_ram {};
			// std::array<u8, NES_RAM_SIZE> m_ppu_ram {};
	};
} // namespace nes

#endif // _NES_BUS_HPP_
