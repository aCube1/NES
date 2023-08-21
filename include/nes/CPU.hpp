#ifndef _NES_CPU_HPP_
#define _NES_CPU_HPP_

#include "common/BitField.hpp"
#include "common/types.hpp"

#include <array>
#include <string>
#include <tuple>

namespace nes {
	class Bus;

	class CPU {
	public:
		CPU();
		CPU(const CPU&) = delete;
		CPU& operator=(const CPU&) = delete;

		void connectBus(Bus *bus);

		void clock();
		void step();
		void reset();

		void irq();
		void nmi();

		inline void setPC(u16 pc) { m_reg.pc = pc; }

		[[nodiscard]] inline u8 getCycles() const { return m_cycles; }

		[[nodiscard]] std::string getDebugString() const;

	private:
		enum AddressingMode {
			IMP, // Implied       : No operand
			ACC, // Accumulator   : No operand
			IMM, // Immediate     : #VALUE
			REL, // Relative      : $ADDR8 used with Branch Instructions
			ZP0, // ZeroPage      : $ADDR8
			ZPX, // ZeroPage, X   : $ADDR8 + X
			ZPY, // ZeroPage, Y   : $ADDR8 + Y
			ABS, // Absolute      : $ADDR16
			ABX, // Absolute, X   : $ADDR16 + X
			ABY, // Absolute, Y   : $ADDR16 + Y
			IND, // Indirect      : ($ADDR16) used only with JMP
			IZX, // (Indirect, X) : ($ADDR16 + X)
			IZY, // (Indirect), Y : ($ADDR16) + Y
		};

		struct Opcode {
			std::string name;
			AddressingMode addressing;
			void (CPU::*operation)(u16) = nullptr;
			u8 cycles { 0 };
			u8 page_cycles { 0 };
		};

		[[nodiscard]] u8 memRead(u16 addr, bool ro = false) const;
		[[nodiscard]] u16 memRead16(u16 addr, bool ro = false) const;
		void memWrite(u16 addr, u8 data);

		void stackPush(u8 data);
		void stackPush16(u16 data);
		[[nodiscard]] u8 stackPop();
		[[nodiscard]] u16 stackPop16();

		[[nodiscard]] std::tuple<u16, bool> getOperandAddress(AddressingMode mode);

		// clang-format off
			void ADC(u16 addr); void AND(u16 addr); void ASL(u16 addr); void BCC(u16 addr);
			void BCS(u16 addr); void BEQ(u16 addr); void BIT(u16 addr); void BMI(u16 addr);
			void BNE(u16 addr); void BPL(u16 addr); void BRK(u16 addr); void BVC(u16 addr);
			void BVS(u16 addr); void CLC(u16 addr); void CLD(u16 addr); void CLI(u16 addr);
			void CLV(u16 addr); void CMP(u16 addr); void CPX(u16 addr); void CPY(u16 addr);
			void DEC(u16 addr); void DEX(u16 addr); void DEY(u16 addr); void EOR(u16 addr);
			void INC(u16 addr); void INX(u16 addr); void INY(u16 addr); void JMP(u16 addr);
			void JSR(u16 addr); void LDA(u16 addr); void LDX(u16 addr); void LDY(u16 addr);
			void LSR(u16 addr); void NOP(u16 addr); void ORA(u16 addr); void PHA(u16 addr);
			void PHP(u16 addr); void PLA(u16 addr); void PLP(u16 addr); void ROL(u16 addr);
			void ROR(u16 addr); void RTI(u16 addr); void RTS(u16 addr); void SBC(u16 addr);
			void SEC(u16 addr); void SED(u16 addr); void SEI(u16 addr); void STA(u16 addr);
			void STX(u16 addr); void STY(u16 addr); void TAX(u16 addr); void TAY(u16 addr);
			void TSX(u16 addr); void TXA(u16 addr); void TXS(u16 addr); void TYA(u16 addr);

			void NIL(u16 addr); // Catch all "illegal" opcodes

		// clang-format on

		// Registers
		struct {
			u16 pc { 0x0000 }; // Program Counter
			u8 sp { 0xfd };    // Stack Pointer

			u8 a { 0x00 }; // Accumulator Register
			u8 x { 0x00 }; // X Register
			u8 y { 0x00 }; // Y Register

			union { // CPU status
				u8 raw;
				BitField<7> n; // Negative
				BitField<6> v; // Overflow
				BitField<5> u; // Unused
				BitField<4> b; // Break
				BitField<3> d; // Decimal Mode
				BitField<2> i; // Disable Interrupts
				BitField<1> z; // Zero
				BitField<0> c; // Carry bit.
			} p;
		} m_reg;

		u8 m_cycles { 8 };

		Bus *m_bus { nullptr };

		// Convenience variables.
		u8 m_opcode {};
		Opcode m_instruction {};

		// Lookup table with all opcodes.
		std::array<Opcode, 256> m_optable;
	};
} // namespace nes

#endif // _NES_CPU_HPP_
