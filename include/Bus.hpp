#ifndef _NES_BUS_HPP_
#define _NES_BUS_HPP_

#include "CPU.hpp"
#include "Cartridge.hpp"
#include "types.hpp"

#include <array>
#include <memory>
#include <vector>

#define NES_RAM_SIZE 2048

namespace nes {
	class Bus {
		public:
			Bus() = default;
			Bus(const Bus&) = delete;
			Bus& operator=(const Bus&) = delete;

			void insert(Cartridge cartridge);

			void power();
			void reset();
			void clock();

			[[nodiscard]] u8 cpuRead(u16 addr, bool ro = false) const;
			[[nodiscard]] u16 cpuRead16(u16 addr, bool ro = false) const;
			void cpuWrite(u16 addr, u8 data);

			[[nodiscard]] u8 ppuRead(u16 addr) const;

			[[nodiscard]] inline CPU& getCPU() { return m_cpu; }

		private:
			Cartridge m_cartridge; // TODO: Move cartridge to a mapper.
			CPU m_cpu;

			// Count how many clocks have passed.
			u32 m_clock { 0 };

			// Memory
			std::array<u8, NES_RAM_SIZE> m_cpu_ram {};
	};
} // namespace nes

#endif // _NES_BUS_HPP_
