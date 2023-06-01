#ifndef _NES_CPU_HPP_
#define _NES_CPU_HPP_

#include "types.hpp"

#include <array>
#include <vector>

namespace nes {
	class CPU {
		public:
			CPU() = default;
			CPU(const CPU&) = delete;
			CPU& operator=(const CPU&) = delete;
			~CPU() = default;

			void load(const std::vector<u8>& program); // TODO: Proper mapper.
			void run();
			void reset();

			// TODO: Move functions to a Bus.
			void memWrite(u16 addr, u8 data);
			[[nodiscard]] u16 memRead(u16 addr) const;

			void memWrite16(u16 addr, u16 data);
			[[nodiscard]] u16 memRead16(u16 addr) const;

			// NOTE: Maybe this is unnecessary.
			[[nodiscard]] u8 getStatus() const;
			[[nodiscard]] u8 getA() const;
			[[nodiscard]] u8 getX() const;
			[[nodiscard]] u8 getY() const;

		private:
			enum Flags : u8 {
				C = (1 << 0), // Carry bit.
				Z = (1 << 1), // Zero
				I = (1 << 2), // Disable Interrupts
				D = (1 << 3), // Decimal Mode
				B = (1 << 4), // Break
				U = (1 << 5), // Unused
				V = (1 << 6), // Overflow
				N = (1 << 7), // Negative
			};

			[[nodiscard]] u8 getFlag(Flags flag) const;
			void setFlag(Flags flag, bool v);

			u16 m_pc { 0x0000 };  // Program Counter
			u8 m_status { 0x00 }; // Status Register
			u8 m_reg_a { 0x00 };  // Accumulator Register
			u8 m_reg_x { 0x00 };  // X Register
			u8 m_reg_y { 0x00 };  // Y Register

			std::array<u8, 0xffff> m_ram {}; // RAM memory array.
	};
} // namespace nes

#endif // _NES_CPU_HPP_
