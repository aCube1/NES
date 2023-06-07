#include "CPU.hpp"

#include <spdlog/spdlog.h>

namespace nes {
	CPU::CPU() {
		m_optable = {
			{ 0x69, Opcode { "ADC", AddressingMode::IMM, &CPU::ADC, 2, 0 } },
			{ 0x65, Opcode { "ADC", AddressingMode::ZP0, &CPU::ADC, 3, 0 } },
			{ 0x75, Opcode { "ADC", AddressingMode::ZPX, &CPU::ADC, 4, 0 } },
			{ 0x6d, Opcode { "ADC", AddressingMode::ABS, &CPU::ADC, 4, 0 } },
			{ 0x7d, Opcode { "ADC", AddressingMode::ABX, &CPU::ADC, 4, 1 } },
			{ 0x79, Opcode { "ADC", AddressingMode::ABY, &CPU::ADC, 4, 1 } },
			{ 0x61, Opcode { "ADC", AddressingMode::IZX, &CPU::ADC, 6, 0 } },
			{ 0x71, Opcode { "ADC", AddressingMode::IZY, &CPU::ADC, 5, 1 } },

			{ 0x00, Opcode { "BRK", AddressingMode::IMP, &CPU::BRK, 7, 0 } },

			{ 0xa9, Opcode { "LDA", AddressingMode::IMM, &CPU::LDA, 2, 0 } },
			{ 0xa5, Opcode { "LDA", AddressingMode::ZP0, &CPU::LDA, 3, 0 } },
			{ 0xb5, Opcode { "LDA", AddressingMode::ZPX, &CPU::LDA, 4, 0 } },
			{ 0xad, Opcode { "LDA", AddressingMode::ABS, &CPU::LDA, 4, 0 } },
			{ 0xbd, Opcode { "LDA", AddressingMode::ABX, &CPU::LDA, 4, 1 } },
			{ 0xb9, Opcode { "LDA", AddressingMode::ABY, &CPU::LDA, 4, 1 } },
			{ 0xa1, Opcode { "LDA", AddressingMode::IZX, &CPU::LDA, 6, 0 } },
			{ 0xb1, Opcode { "LDA", AddressingMode::IZY, &CPU::LDA, 5, 1 } },

			{ 0xa2, Opcode { "LDX", AddressingMode::IMM, &CPU::LDX, 2, 0 } },
			{ 0xa6, Opcode { "LDX", AddressingMode::ZP0, &CPU::LDX, 3, 0 } },
			{ 0xb6, Opcode { "LDX", AddressingMode::ZPY, &CPU::LDX, 4, 0 } },
			{ 0xae, Opcode { "LDX", AddressingMode::ABS, &CPU::LDX, 4, 0 } },
			{ 0xbe, Opcode { "LDX", AddressingMode::ABY, &CPU::LDX, 4, 1 } },

			{ 0xa0, Opcode { "LDY", AddressingMode::IMM, &CPU::LDY, 2, 0 } },
			{ 0xa4, Opcode { "LDY", AddressingMode::ZP0, &CPU::LDY, 3, 0 } },
			{ 0xb4, Opcode { "LDY", AddressingMode::ZPX, &CPU::LDY, 4, 0 } },
			{ 0xac, Opcode { "LDY", AddressingMode::ABS, &CPU::LDY, 4, 0 } },
			{ 0xbc, Opcode { "LDY", AddressingMode::ABX, &CPU::LDY, 4, 1 } },

			{ 0x08, Opcode { "PHP", AddressingMode::IMP, &CPU::PHP, 3, 0 } },

			{ 0xaa, Opcode { "TAX", AddressingMode::IMP, &CPU::TAX, 2, 0 } },
			{ 0xa8, Opcode { "TAY", AddressingMode::IMP, &CPU::TAY, 2, 0 } },
			{ 0xba, Opcode { "TSX", AddressingMode::IMP, &CPU::TSX, 2, 0 } },
			{ 0x8a, Opcode { "TXA", AddressingMode::IMP, &CPU::TXA, 2, 0 } },
			{ 0x9a, Opcode { "TXS", AddressingMode::IMP, &CPU::TXS, 2, 0 } },
			{ 0x98, Opcode { "TYA", AddressingMode::IMP, &CPU::TYA, 2, 0 } },
		};
	}

	void CPU::load(const std::vector<u8>& program) {
		std::copy(program.begin(), program.end(), m_ram.begin() + 0x8000);
		m_pc = 0x8000;
	}

	void CPU::clock() {
		// Verify if there is remaining cycles.
		if (m_cycles == 0) {
			step();
		}
		m_cycles -= 1;
	}

	void CPU::step() {
		m_opcode = memRead(m_pc);
		m_pc += 1;

		Opcode op {};
		try {
			op = m_optable.at(m_opcode);
		} catch (std::out_of_range& e) {
			// FIX: Emulation crash if opcode doesn't exist!
			spdlog::warn("Opcode {0:#0x} is not implemented! {1:}", m_opcode, e.what());
			throw;
		}

		u16 addr { getOperandAddress(op.addressing) };

		(this->*op.operation)(addr);
		m_cycles += op.cycles;

		if (m_page_crossed) {
			m_cycles += op.page_cycles;
		}

		// TODO: Debug only, remove later.
		spdlog::info("Opcode {:#0x}", m_opcode);
		spdlog::info("Status : {:#0b}", m_status);
		spdlog::info("Reg | A: {:#0x}", m_reg_a);
		spdlog::info("Reg | X: {:#0x}", m_reg_x);
		spdlog::info("Reg | Y: {:#0x}", m_reg_y);
	}

	void CPU::reset() {
		m_status = 0 | U; // NOTE: Unused bit is always 1.
		m_sp = 0xfd;
		m_pc = memRead16(0xfffc);

		m_reg_a = 0x00;
		m_reg_x = 0x00;
		m_reg_y = 0x00;

		// Reset takes time.
		m_cycles = 8;
	}

	void CPU::irq() {
		// Is interrupt is allowed
		if (getFlag(I) == 1) {
			return;
		}

		stackPush16(m_pc);

		setFlag(B, false);
		setFlag(U, true);
		setFlag(I, true);
		stackPush(m_status);

		m_pc = memRead(0xfffe);

		m_cycles = 7; // IRQs take time.
	}

	void CPU::nmi() {
		stackPush16(m_pc);

		setFlag(B, false);
		setFlag(U, true);
		setFlag(I, true);
		stackPush(m_status);

		m_pc = memRead(0xfffa);

		m_cycles = 8; // NMIs take time.
	}

	void CPU::memWrite(u16 addr, u8 data) {
		m_ram.at(addr) = data;
	}

	u8 CPU::memRead(u16 addr) const {
		u8 data {};

		data = m_ram.at(addr);

		return data;
	}

	void CPU::memWrite16(u16 addr, u16 data) {
		auto l { static_cast<u8>(data >> 8) };
		auto h { static_cast<u8>(data & 0xff) };

		memWrite(addr, l);
		memWrite(addr + 1, h);
	}

	u16 CPU::memRead16(u16 addr) const {
		auto l { memRead(addr) };
		auto h { memRead(addr + 1) };

		return (h << 8) | l;
	}

	void CPU::stackPush(u8 data) {
		memWrite(0x0100 + m_sp, data);
		m_sp -= 1;
	}

	u8 CPU::stackPop() {
		m_sp += 1;
		return memRead(0x0100 + m_sp);
	}

	void CPU::stackPush16(u16 data) {
		memWrite16(0x0100 + m_sp, data);
		m_sp -= 2;
	}

	u16 CPU::stackPop16() {
		auto l { stackPop() };
		auto h { stackPop() };

		return (h << 8) | l;
	}

	// Check if page crossed.
	void CPU::isPageCrossed(u16 a, u16 b) {
		m_page_crossed = (a & 0xff00) != (b & 0xff00);
	}

	u16 CPU::getOperandAddress(AddressingMode mode) {
		u16 addr {};

		switch (mode) {
		case AddressingMode::IMM:
			addr = m_pc;
			m_pc += 1;
			break;
		case AddressingMode::REL:
			addr = static_cast<u16>(memRead(m_pc));
			m_pc += 1;

			if (addr & 0x80) {
				addr |= 0xff00;
			}
			break;
		case AddressingMode::ZP0:
			addr = static_cast<u16>(memRead(m_pc));
			m_pc += 1;
			break;
		case AddressingMode::ZPX:
			addr = static_cast<u16>(memRead(m_pc) + m_reg_x);
			m_pc += 1;
			break;
		case AddressingMode::ZPY:
			addr = static_cast<u16>(memRead(m_pc) + m_reg_y);
			m_pc += 1;
			break;
		case AddressingMode::ABS:
			addr = memRead16(m_pc);
			m_pc += 2;
			break;
		case AddressingMode::ABX:
			addr = memRead16(m_pc) + m_reg_x;
			m_pc += 2;
			isPageCrossed(addr - m_reg_x, addr);
			break;
		case AddressingMode::ABY:
			addr = memRead16(m_pc) + m_reg_y;
			m_pc += 2;
			isPageCrossed(addr - m_reg_y, addr);
			break;
		case AddressingMode::IND: {
			auto ptr { memRead16(m_pc) };
			m_pc += 2;

			if ((ptr & 0x00ff) == 0x00ff) {
				// HACK: Simulate page boundary hardware bug.
				addr = (memRead(ptr & 0xff00) << 8) | memRead(ptr);
			} else {
				addr = (memRead(ptr + 1) << 8 | memRead(ptr));
			}
		} break;
		case AddressingMode::IZX: {
			u16 temp { memRead(m_pc) };
			m_pc += 1;

			u8 l { memRead((temp + m_reg_x) & 0x00ff) };
			u8 h { memRead((temp + m_reg_x + 1) & 0x00ff) };

			addr = (h << 8) | l;
		} break;
		case AddressingMode::IZY: {
			u16 temp { memRead(m_pc) };
			m_pc += 1;

			u8 l { memRead(temp & 0x00ff) };
			u8 h { memRead((temp + 1) & 0x00ff) };

			addr = (h << 8) | l;
			addr += m_reg_y;
			isPageCrossed(addr - m_reg_y, addr);
		} break;
		default:
			break;
		}

		return addr;
	}

	u8 CPU::getFlag(CPU::Flags flag) const {
		return (m_status & flag);
	}

	void CPU::setFlag(CPU::Flags flag, bool v) {
		if (v) {
			m_status |= flag;
		} else {
			m_status &= (~flag);
		}
	}

	// Instruction: Add with Carry In
	// Result     : A = A + M + C
	// Flags      : C, V, N, Z
	void CPU::ADC(u16 addr) {
		auto m { memRead(addr) };
		auto result { static_cast<u16>(m_reg_a + m + getFlag(C)) };

		setFlag(C, result > 0xff);
		setFlag(V, (m_reg_a ^ result) & (m ^ result) & 0x80 != 0x00);

		m_reg_a = result & 0x00ff;

		setFlag(Z, (m_reg_a & 0xFF) == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	void CPU::AND(u16 /*unused*/) {}

	void CPU::ASL(u16 /*unused*/) {}

	void CPU::BCC(u16 /*unused*/) {}

	void CPU::BCS(u16 /*unused*/) {}

	void CPU::BEQ(u16 /*unused*/) {}

	void CPU::BIT(u16 /*unused*/) {}

	void CPU::BMI(u16 /*unused*/) {}

	void CPU::BNE(u16 /*unused*/) {}

	void CPU::BPL(u16 /*unused*/) {}

	// Instruction: Break
	// Result     : Program sourced interrupt
	void CPU::BRK(u16 /*unused*/) {
		setFlag(I, true);
		stackPush16(m_pc);

		stackPush(m_status | B);

		m_pc = memRead16(0xfffe);
	}

	void CPU::BVC(u16 /*unused*/) {}

	void CPU::BVS(u16 /*unused*/) {}

	void CPU::CLC(u16 /*unused*/) {}

	void CPU::CLD(u16 /*unused*/) {}

	void CPU::CLI(u16 /*unused*/) {}

	void CPU::CLV(u16 /*unused*/) {}

	void CPU::CMP(u16 /*unused*/) {}

	void CPU::CPX(u16 /*unused*/) {}

	void CPU::CPY(u16 /*unused*/) {}

	void CPU::DEC(u16 /*unused*/) {}

	void CPU::DEX(u16 /*unused*/) {}

	void CPU::DEY(u16 /*unused*/) {}

	void CPU::EOR(u16 /*unused*/) {}

	void CPU::INC(u16 /*unused*/) {}

	void CPU::INX(u16 /*unused*/) {}

	void CPU::INY(u16 /*unused*/) {}

	void CPU::JMP(u16 /*unused*/) {}

	void CPU::JSR(u16 /*unused*/) {}

	// Instruction: Load the accumulator
	// Result     : A = M
	// Flags      : Z, N
	void CPU::LDA(u16 addr) {
		m_reg_a = memRead(addr);
		setFlag(Z, m_reg_a == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	// Instruction: Load the X register
	// Result     : X = M
	// Flags      : Z, N
	void CPU::LDX(u16 addr) {
		m_reg_x = memRead(addr);
		setFlag(Z, m_reg_x == 0x00);
		setFlag(N, m_reg_x & 0x80);
	}

	// Instruction: Load the Y register
	// Result     : Y = M
	// Flags      : Z, N
	void CPU::LDY(u16 addr) {
		m_reg_y = memRead(addr);
		setFlag(Z, m_reg_y == 0x00);
		setFlag(N, m_reg_y & 0x80);
	}

	void CPU::LSR(u16 /*unused*/) {}

	void CPU::NOP(u16 /*unused*/) {}

	void CPU::ORA(u16 /*unused*/) {}

	void CPU::PHA(u16 /*unused*/) {}

	// Instruction: Push status register to the stack
	// Result     : Status -> Stack
	// Note       : Break flag is set before push.
	void CPU::PHP(u16 /*unused*/) {
		stackPush(m_status | B | U); // NOTE: Always set Unused bit to 1.
		setFlag(B, false);
	}

	void CPU::PLA(u16 /*unused*/) {}

	void CPU::PLP(u16 /*unused*/) {}

	void CPU::ROL(u16 /*unused*/) {}

	void CPU::ROR(u16 /*unused*/) {}

	void CPU::RTI(u16 /*unused*/) {}

	void CPU::RTS(u16 /*unused*/) {}

	// Instruction: Subtract with Borrow In
	// Result     : A = A - M - (1 - C)
	// Flags      : C, V, N, Z
	void CPU::SBC(u16 addr) {
		auto m { memRead(addr) };
		auto result { static_cast<u16>(m_reg_a - m - (1 - getFlag(C))) };

		setFlag(C, result > 0xff);
		setFlag(V, (m_reg_a ^ result) & (~m ^ result) & 0x80 != 0x00);

		m_reg_a = result & 0x00ff;

		setFlag(Z, (m_reg_a & 0xff) == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	void CPU::SEC(u16 /*unused*/) {}

	void CPU::SED(u16 /*unused*/) {}

	void CPU::SEI(u16 /*unused*/) {}

	void CPU::STA(u16 /*unused*/) {}

	void CPU::STX(u16 /*unused*/) {}

	void CPU::STY(u16 /*unused*/) {}

	// Instruction: Copy contents from Accumulator to X register.
	// Result     : X = A
	// Flags      : Z, N
	void CPU::TAX(u16 /*unused*/) {
		m_reg_x = m_reg_a;
		setFlag(Z, (m_reg_x & 0xff) == 0x00);
		setFlag(N, m_reg_x & 0x80);
	}

	// Instruction: Copy contents from Accumulator to Y register.
	// Result     : Y = A
	// Flags      : Z, N
	void CPU::TAY(u16 /*unused*/) {
		m_reg_y = m_reg_a;
		setFlag(Z, (m_reg_y & 0xff) == 0x00);
		setFlag(N, m_reg_y & 0x80);
	}

	// Instruction: Transfer Stack Pointer to X register.
	// Result     : X = StackPointer
	// Flags      : Z, N
	void CPU::TSX(u16 /*unused*/) {
		m_reg_x = m_sp;
		setFlag(Z, (m_reg_x & 0xff) == 0x00);
		setFlag(N, m_reg_x & 0x80);
	}

	// Instruction: Copy contents from X register to Accumulator.
	// Result     : A = X
	// Flags      : Z, N
	void CPU::TXA(u16 /*unused*/) {
		m_reg_a = m_reg_x;
		setFlag(Z, (m_reg_a & 0xff) == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	// Instruction: Transfer X register to Stack Pointer.
	// Result     : StackPointer = X
	void CPU::TXS(u16 /*unused*/) {
		m_sp = m_reg_x;
	}

	// Instruction: Copy contents from Y register to Accumulator.
	// Result     : A = Y
	// Flags      : Z, N
	void CPU::TYA(u16 /*unused*/) {
		m_reg_a = m_reg_y;
		setFlag(Z, (m_reg_a & 0xff) == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}
} // namespace nes
