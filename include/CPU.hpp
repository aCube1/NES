#ifndef _NES_CPU_HPP_
#define _NES_CPU_HPP_

#include "types.hpp"

#include <ios>
#include <vector>

namespace nes {
	class CPU {
		public:
			CPU() = default;
			~CPU() = default;

			void interpret(const std::vector<u8>& program);

			[[nodiscard]] u8 getStatus() const;
			[[nodiscard]] u8 getA() const;
			[[nodiscard]] u8 getX() const;
			[[nodiscard]] u8 getY() const;

		private:
			u16 m_pc { 0x0000 };  // Program Counter
			u8 m_status { 0x00 }; // Status Register
			u8 m_reg_a { 0x00 };  // Accumulator Register
			u8 m_reg_x { 0x00 };  // X Register
			u8 m_reg_y { 0x00 };  // Y Register

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
	};
} // namespace nes

#endif // _NES_CPU_HPP_
