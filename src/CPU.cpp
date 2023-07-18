#include "CPU.hpp"

#include "Bus.hpp"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace {
	[[nodiscard]] bool isPageCrossed(u16 a, u16 b) {
		return (a & 0xff00) != (b & 0xff00);
	}
} // namespace

namespace nes {
	CPU::CPU() {
		// NOTE: I don't know if I need to change this to a array or continue using a
		// unordered_map
		m_optable = {
			{ 0x69, Opcode { "ADC", AddressingMode::IMM, &CPU::ADC, 2, 0 } },
			{ 0x65, Opcode { "ADC", AddressingMode::ZP0, &CPU::ADC, 3, 0 } },
			{ 0x75, Opcode { "ADC", AddressingMode::ZPX, &CPU::ADC, 4, 0 } },
			{ 0x6d, Opcode { "ADC", AddressingMode::ABS, &CPU::ADC, 4, 0 } },
			{ 0x7d, Opcode { "ADC", AddressingMode::ABX, &CPU::ADC, 4, 1 } },
			{ 0x79, Opcode { "ADC", AddressingMode::ABY, &CPU::ADC, 4, 1 } },
			{ 0x61, Opcode { "ADC", AddressingMode::IZX, &CPU::ADC, 6, 0 } },
			{ 0x71, Opcode { "ADC", AddressingMode::IZY, &CPU::ADC, 5, 1 } },

			{ 0x29, Opcode { "AND", AddressingMode::IMM, &CPU::AND, 2, 0 } },
			{ 0x25, Opcode { "AND", AddressingMode::ZP0, &CPU::AND, 3, 0 } },
			{ 0x35, Opcode { "AND", AddressingMode::ZPX, &CPU::AND, 4, 0 } },
			{ 0x2d, Opcode { "AND", AddressingMode::ABS, &CPU::AND, 4, 0 } },
			{ 0x3d, Opcode { "AND", AddressingMode::ABX, &CPU::AND, 4, 1 } },
			{ 0x39, Opcode { "AND", AddressingMode::ABY, &CPU::AND, 4, 1 } },
			{ 0x21, Opcode { "AND", AddressingMode::IZX, &CPU::AND, 6, 0 } },
			{ 0x31, Opcode { "AND", AddressingMode::IZY, &CPU::AND, 5, 1 } },

			{ 0x0a, Opcode { "ASL", AddressingMode::ACC, &CPU::ASL, 2, 0 } },
			{ 0x06, Opcode { "ASL", AddressingMode::ZP0, &CPU::ASL, 5, 0 } },
			{ 0x16, Opcode { "ASL", AddressingMode::ZPX, &CPU::ASL, 6, 0 } },
			{ 0x0e, Opcode { "ASL", AddressingMode::ABS, &CPU::ASL, 6, 0 } },
			{ 0x1e, Opcode { "ASL", AddressingMode::ABX, &CPU::ASL, 7, 0 } },

			{ 0x90, Opcode { "BCC", AddressingMode::REL, &CPU::BCC, 2, 0 } },
			{ 0xb0, Opcode { "BCS", AddressingMode::REL, &CPU::BCS, 2, 0 } },
			{ 0xf0, Opcode { "BEQ", AddressingMode::REL, &CPU::BEQ, 2, 0 } },

			{ 0x24, Opcode { "BIT", AddressingMode::ZP0, &CPU::BIT, 3, 0 } },
			{ 0x2c, Opcode { "BIT", AddressingMode::ABS, &CPU::BIT, 4, 0 } },

			{ 0x30, Opcode { "BMI", AddressingMode::REL, &CPU::BMI, 2, 0 } },
			{ 0xd0, Opcode { "BNE", AddressingMode::REL, &CPU::BNE, 2, 0 } },
			{ 0x10, Opcode { "BPL", AddressingMode::REL, &CPU::BPL, 2, 0 } },

			{ 0x00, Opcode { "BRK", AddressingMode::IMP, &CPU::BRK, 7, 0 } },

			{ 0x50, Opcode { "BVC", AddressingMode::REL, &CPU::BVC, 2, 0 } },
			{ 0x70, Opcode { "BVS", AddressingMode::REL, &CPU::BVS, 2, 0 } },

			{ 0x18, Opcode { "CLC", AddressingMode::IMP, &CPU::CLC, 2, 0 } },
			{ 0xd8, Opcode { "CLD", AddressingMode::IMP, &CPU::CLD, 2, 0 } },
			{ 0x58, Opcode { "CLI", AddressingMode::IMP, &CPU::CLI, 2, 0 } },
			{ 0xb8, Opcode { "CLV", AddressingMode::IMP, &CPU::CLV, 2, 0 } },

			{ 0xc9, Opcode { "CMP", AddressingMode::IMM, &CPU::CMP, 2, 0 } },
			{ 0xc5, Opcode { "CMP", AddressingMode::ZP0, &CPU::CMP, 3, 0 } },
			{ 0xd5, Opcode { "CMP", AddressingMode::ZPX, &CPU::CMP, 4, 0 } },
			{ 0xcd, Opcode { "CMP", AddressingMode::ABS, &CPU::CMP, 4, 0 } },
			{ 0xdd, Opcode { "CMP", AddressingMode::ABX, &CPU::CMP, 4, 1 } },
			{ 0xd9, Opcode { "CMP", AddressingMode::ABY, &CPU::CMP, 4, 1 } },
			{ 0xc1, Opcode { "CMP", AddressingMode::IZX, &CPU::CMP, 6, 0 } },
			{ 0xd1, Opcode { "CMP", AddressingMode::IZY, &CPU::CMP, 5, 1 } },

			{ 0xe0, Opcode { "CPX", AddressingMode::IMM, &CPU::CPX, 2, 0 } },
			{ 0xe4, Opcode { "CPX", AddressingMode::ZP0, &CPU::CPX, 3, 0 } },
			{ 0xec, Opcode { "CPX", AddressingMode::ABS, &CPU::CPX, 4, 0 } },

			{ 0xc0, Opcode { "CPY", AddressingMode::IMM, &CPU::CPY, 2, 0 } },
			{ 0xc4, Opcode { "CPY", AddressingMode::ZP0, &CPU::CPY, 3, 0 } },
			{ 0xcc, Opcode { "CPY", AddressingMode::ABS, &CPU::CPY, 4, 0 } },

			{ 0xc6, Opcode { "DEC", AddressingMode::ZP0, &CPU::DEC, 5, 0 } },
			{ 0xd6, Opcode { "DEC", AddressingMode::ZPX, &CPU::DEC, 6, 0 } },
			{ 0xce, Opcode { "DEC", AddressingMode::ABS, &CPU::DEC, 6, 0 } },
			{ 0xde, Opcode { "DEC", AddressingMode::ABX, &CPU::DEC, 7, 0 } },

			{ 0xca, Opcode { "DEX", AddressingMode::IMP, &CPU::DEX, 2, 0 } },
			{ 0x88, Opcode { "DEY", AddressingMode::IMP, &CPU::DEY, 2, 0 } },

			{ 0x49, Opcode { "EOR", AddressingMode::IMM, &CPU::EOR, 2, 0 } },
			{ 0x45, Opcode { "EOR", AddressingMode::ZP0, &CPU::EOR, 3, 0 } },
			{ 0x55, Opcode { "EOR", AddressingMode::ZPX, &CPU::EOR, 4, 0 } },
			{ 0x4d, Opcode { "EOR", AddressingMode::ABS, &CPU::EOR, 4, 0 } },
			{ 0x5d, Opcode { "EOR", AddressingMode::ABX, &CPU::EOR, 4, 1 } },
			{ 0x59, Opcode { "EOR", AddressingMode::ABY, &CPU::EOR, 4, 1 } },
			{ 0x41, Opcode { "EOR", AddressingMode::IZX, &CPU::EOR, 6, 0 } },
			{ 0x51, Opcode { "EOR", AddressingMode::IZY, &CPU::EOR, 5, 1 } },

			{ 0xe6, Opcode { "INC", AddressingMode::ZP0, &CPU::INC, 5, 0 } },
			{ 0xf6, Opcode { "INC", AddressingMode::ZPX, &CPU::INC, 6, 0 } },
			{ 0xee, Opcode { "INC", AddressingMode::ABS, &CPU::INC, 6, 0 } },
			{ 0xfe, Opcode { "INC", AddressingMode::ABX, &CPU::INC, 7, 0 } },

			{ 0xe8, Opcode { "INX", AddressingMode::IMP, &CPU::INX, 2, 0 } },
			{ 0xc8, Opcode { "INY", AddressingMode::IMP, &CPU::INY, 2, 0 } },

			{ 0x4c, Opcode { "JMP", AddressingMode::ABS, &CPU::JMP, 3, 0 } },
			{ 0x6c, Opcode { "JMP", AddressingMode::IND, &CPU::JMP, 5, 0 } },

			{ 0x20, Opcode { "JSR", AddressingMode::ABS, &CPU::JSR, 6, 0 } },

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

			{ 0x4a, Opcode { "LSR", AddressingMode::ACC, &CPU::LSR, 2, 0 } },
			{ 0x46, Opcode { "LSR", AddressingMode::ZP0, &CPU::LSR, 5, 0 } },
			{ 0x56, Opcode { "LSR", AddressingMode::ZPX, &CPU::LSR, 6, 0 } },
			{ 0x4e, Opcode { "LSR", AddressingMode::ABS, &CPU::LSR, 6, 0 } },
			{ 0x5e, Opcode { "LSR", AddressingMode::ABX, &CPU::LSR, 7, 0 } },

			{ 0x1a, Opcode { "NOP", AddressingMode::IMP, &CPU::NOP, 2, 0 } },
			{ 0x3a, Opcode { "NOP", AddressingMode::IMP, &CPU::NOP, 2, 0 } },
			{ 0x5a, Opcode { "NOP", AddressingMode::IMP, &CPU::NOP, 2, 0 } },
			{ 0x7a, Opcode { "NOP", AddressingMode::IMP, &CPU::NOP, 2, 0 } },
			{ 0xda, Opcode { "NOP", AddressingMode::IMP, &CPU::NOP, 2, 0 } },
			{ 0xea, Opcode { "NOP", AddressingMode::IMP, &CPU::NOP, 2, 0 } },
			{ 0xfa, Opcode { "NOP", AddressingMode::IMP, &CPU::NOP, 2, 0 } },
			{ 0x80, Opcode { "NOP", AddressingMode::IMM, &CPU::NOP, 2, 0 } },
			{ 0x82, Opcode { "NOP", AddressingMode::IMM, &CPU::NOP, 2, 0 } },
			{ 0x89, Opcode { "NOP", AddressingMode::IMM, &CPU::NOP, 2, 0 } },
			{ 0xc2, Opcode { "NOP", AddressingMode::IMM, &CPU::NOP, 2, 0 } },
			{ 0xe2, Opcode { "NOP", AddressingMode::IMM, &CPU::NOP, 2, 0 } },
			{ 0x04, Opcode { "NOP", AddressingMode::ZP0, &CPU::NOP, 3, 0 } },
			{ 0x44, Opcode { "NOP", AddressingMode::ZP0, &CPU::NOP, 3, 0 } },
			{ 0x64, Opcode { "NOP", AddressingMode::ZP0, &CPU::NOP, 3, 0 } },
			{ 0x14, Opcode { "NOP", AddressingMode::ZPX, &CPU::NOP, 4, 0 } },
			{ 0x34, Opcode { "NOP", AddressingMode::ZPX, &CPU::NOP, 4, 0 } },
			{ 0x54, Opcode { "NOP", AddressingMode::ZPX, &CPU::NOP, 4, 0 } },
			{ 0x74, Opcode { "NOP", AddressingMode::ZPX, &CPU::NOP, 4, 0 } },
			{ 0xd4, Opcode { "NOP", AddressingMode::ZPX, &CPU::NOP, 4, 0 } },
			{ 0xf4, Opcode { "NOP", AddressingMode::ZPX, &CPU::NOP, 4, 0 } },
			{ 0x0c, Opcode { "NOP", AddressingMode::ABS, &CPU::NOP, 4, 0 } },
			{ 0x1c, Opcode { "NOP", AddressingMode::ABX, &CPU::NOP, 4, 1 } },
			{ 0x3c, Opcode { "NOP", AddressingMode::ABX, &CPU::NOP, 4, 1 } },
			{ 0x5c, Opcode { "NOP", AddressingMode::ABX, &CPU::NOP, 4, 1 } },
			{ 0x7c, Opcode { "NOP", AddressingMode::ABX, &CPU::NOP, 4, 1 } },
			{ 0xdc, Opcode { "NOP", AddressingMode::ABX, &CPU::NOP, 4, 1 } },
			{ 0xfc, Opcode { "NOP", AddressingMode::ABX, &CPU::NOP, 4, 1 } },

			{ 0x09, Opcode { "ORA", AddressingMode::IMM, &CPU::ORA, 2, 0 } },
			{ 0x05, Opcode { "ORA", AddressingMode::IMM, &CPU::ORA, 3, 0 } },
			{ 0x15, Opcode { "ORA", AddressingMode::IMM, &CPU::ORA, 4, 0 } },
			{ 0x0d, Opcode { "ORA", AddressingMode::IMM, &CPU::ORA, 4, 0 } },
			{ 0x1d, Opcode { "ORA", AddressingMode::IMM, &CPU::ORA, 4, 1 } },
			{ 0x19, Opcode { "ORA", AddressingMode::IMM, &CPU::ORA, 4, 1 } },
			{ 0x01, Opcode { "ORA", AddressingMode::IMM, &CPU::ORA, 6, 0 } },
			{ 0x11, Opcode { "ORA", AddressingMode::IMM, &CPU::ORA, 5, 0 } },

			{ 0x48, Opcode { "PHA", AddressingMode::IMP, &CPU::PHA, 3, 0 } },
			{ 0x08, Opcode { "PHP", AddressingMode::IMP, &CPU::PHP, 3, 0 } },
			{ 0x68, Opcode { "PLA", AddressingMode::IMP, &CPU::PLA, 4, 0 } },
			{ 0x28, Opcode { "PLP", AddressingMode::IMP, &CPU::PLP, 4, 0 } },

			{ 0x2a, Opcode { "ROL", AddressingMode::ACC, &CPU::ROL, 2, 0 } },
			{ 0x26, Opcode { "ROL", AddressingMode::ZP0, &CPU::ROL, 5, 0 } },
			{ 0x36, Opcode { "ROL", AddressingMode::ZPX, &CPU::ROL, 6, 0 } },
			{ 0x2e, Opcode { "ROL", AddressingMode::ABS, &CPU::ROL, 6, 0 } },
			{ 0x3e, Opcode { "ROL", AddressingMode::ABX, &CPU::ROL, 7, 0 } },

			{ 0x6a, Opcode { "ROR", AddressingMode::ACC, &CPU::ROR, 2, 0 } },
			{ 0x66, Opcode { "ROR", AddressingMode::ZP0, &CPU::ROR, 5, 0 } },
			{ 0x76, Opcode { "ROR", AddressingMode::ZPX, &CPU::ROR, 6, 0 } },
			{ 0x6e, Opcode { "ROR", AddressingMode::ABS, &CPU::ROR, 6, 0 } },
			{ 0x7e, Opcode { "ROR", AddressingMode::ABX, &CPU::ROR, 7, 0 } },

			{ 0xe9, Opcode { "SBC", AddressingMode::IMM, &CPU::SBC, 2, 0 } },
			{ 0xe5, Opcode { "SBC", AddressingMode::ZP0, &CPU::SBC, 3, 0 } },
			{ 0xf5, Opcode { "SBC", AddressingMode::ZPX, &CPU::SBC, 4, 0 } },
			{ 0xed, Opcode { "SBC", AddressingMode::ABS, &CPU::SBC, 4, 0 } },
			{ 0xfd, Opcode { "SBC", AddressingMode::ABX, &CPU::SBC, 4, 1 } },
			{ 0xf9, Opcode { "SBC", AddressingMode::ABY, &CPU::SBC, 4, 1 } },
			{ 0xe1, Opcode { "SBC", AddressingMode::IZX, &CPU::SBC, 6, 0 } },
			{ 0xf1, Opcode { "SBC", AddressingMode::IZY, &CPU::SBC, 5, 1 } },

			{ 0x38, Opcode { "SEC", AddressingMode::IMP, &CPU::SEC, 2, 0 } },
			{ 0xf8, Opcode { "SED", AddressingMode::IMP, &CPU::SED, 2, 0 } },
			{ 0x78, Opcode { "SEI", AddressingMode::IMP, &CPU::SEI, 2, 0 } },

			{ 0x85, Opcode { "STA", AddressingMode::ZP0, &CPU::STA, 3, 0 } },
			{ 0x95, Opcode { "STA", AddressingMode::ZPX, &CPU::STA, 4, 0 } },
			{ 0x8d, Opcode { "STA", AddressingMode::ABS, &CPU::STA, 4, 0 } },
			{ 0x9d, Opcode { "STA", AddressingMode::ABX, &CPU::STA, 5, 0 } },
			{ 0x99, Opcode { "STA", AddressingMode::ABY, &CPU::STA, 5, 0 } },
			{ 0x81, Opcode { "STA", AddressingMode::IZX, &CPU::STA, 6, 0 } },
			{ 0x91, Opcode { "STA", AddressingMode::IZY, &CPU::STA, 6, 0 } },

			{ 0x86, Opcode { "STX", AddressingMode::ZP0, &CPU::STX, 3, 0 } },
			{ 0x96, Opcode { "STX", AddressingMode::ZPY, &CPU::STX, 4, 0 } },
			{ 0x8e, Opcode { "STX", AddressingMode::ABS, &CPU::STX, 4, 0 } },

			{ 0x84, Opcode { "STY", AddressingMode::ZP0, &CPU::STY, 3, 0 } },
			{ 0x94, Opcode { "STY", AddressingMode::ZPX, &CPU::STY, 4, 0 } },
			{ 0x8c, Opcode { "STY", AddressingMode::ABS, &CPU::STY, 4, 0 } },

			{ 0xaa, Opcode { "TAX", AddressingMode::IMP, &CPU::TAX, 2, 0 } },
			{ 0xa8, Opcode { "TAY", AddressingMode::IMP, &CPU::TAY, 2, 0 } },
			{ 0xba, Opcode { "TSX", AddressingMode::IMP, &CPU::TSX, 2, 0 } },
			{ 0x8a, Opcode { "TXA", AddressingMode::IMP, &CPU::TXA, 2, 0 } },
			{ 0x9a, Opcode { "TXS", AddressingMode::IMP, &CPU::TXS, 2, 0 } },
			{ 0x98, Opcode { "TYA", AddressingMode::IMP, &CPU::TYA, 2, 0 } },
		};
	}

	void CPU::connectBus(Bus *bus) {
		m_bus = bus;
		assert(m_bus != nullptr);
	}

	void CPU::clock() {
		// Verify if there is remaining cycles.
		if (m_cycles == 0) {
			setFlag(U, true); // NOTE: Always set Unused bit to 1.
			step();
		}
		m_cycles -= 1;
	}

	void CPU::step() {
		m_opcode = memRead(m_pc);
		m_pc += 1;

		try {
			m_instruction = m_optable.at(m_opcode);
		} catch (std::out_of_range& e) {
			spdlog::warn("Opcode {:#04x} is not implemented! {}", m_opcode, e.what());
			// Just set current opcode to NOP if not doesn't exist.
			m_instruction = m_optable.at(0xea);
		}

		auto [addr, page_crossed] { getOperandAddress(m_instruction.addressing) };

		(this->*m_instruction.operation)(addr);
		m_cycles += m_instruction.cycles;

		if (page_crossed) {
			m_cycles += m_instruction.page_cycles;
		}
	}

	void CPU::reset() {
		m_pc = memRead16(0xfffc);
		m_status = 0x00 | U; // NOTE: Unused bit is always 1.
		m_sp = 0xfd;

		m_reg_a = 0x00;
		m_reg_x = 0x00;
		m_reg_y = 0x00;

		m_cycles = 8; // Reset takes time.
	}

	void CPU::irq() {
		// Is interrupt allowed
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

	u8 CPU::getFlag(CPU::Flags flag) const {
		return (m_status & flag);
	}

	void CPU::setFlag(CPU::Flags flag, bool active) {
		if (active) {
			m_status |= flag;
		} else {
			m_status &= (~flag);
		}
	}

	std::string CPU::getDebugString() const {
		// Check if some instruction is running.
		if (m_cycles > 0) {
			return "";
		}

		// Get next Opcode
		auto next_opcode { memRead(m_pc) };
		std::string opcode_name {};
		try {
			opcode_name = m_optable.at(next_opcode).name;
		} catch (std::out_of_range& e) {
			// Just set current opcode to XXX if not doesn't exist.
			opcode_name = "XXX";
		}

		return fmt::format(
			"{0:#06x} {1:#04x} {2}      A:{3:#04x} X:{4:#04x} Y:{5:#04x} ST:{6:#010b} "
			"SP:{7:#04x}",
			m_pc, next_opcode, opcode_name, m_reg_a, m_reg_x, m_reg_y, m_status, m_sp
		);
	}

	u8 CPU::memRead(u16 addr, bool ro) const {
		assert(m_bus != nullptr);
		return m_bus->cpuRead(addr, ro);
	}

	u16 CPU::memRead16(u16 addr, bool ro) const {
		assert(m_bus != nullptr);
		return m_bus->cpuRead16(addr, ro);
	}

	void CPU::memWrite(u16 addr, u8 data) {
		assert(m_bus != nullptr);
		m_bus->cpuWrite(addr, data);
	}

	void CPU::stackPush(u8 data) {
		memWrite(0x0100 + m_sp, data);
		m_sp -= 1;
	}

	u8 CPU::stackPop() {
		auto data { memRead(0x0100 + m_sp) };
		m_sp += 1;
		return data;
	}

	void CPU::stackPush16(u16 data) {
		stackPush((data >> 8) & 0x00ff);
		stackPush(data & 0x00ff);
	}

	u16 CPU::stackPop16() {
		return (stackPop() << 8) | stackPop();
	}

	std::tuple<u16, bool> CPU::getOperandAddress(AddressingMode mode) {
		u16 addr {};
		bool page_crossed { false };

		switch (mode) {
		case AddressingMode::IMP: // FALLTHROUGH
		case AddressingMode::ACC:
			break;
		case AddressingMode::IMM:
			addr = m_pc;
			m_pc += 1;

			break;
		case AddressingMode::REL:
			addr = memRead(m_pc);
			m_pc += 1;

			if (addr & 0x80) {
				addr |= 0xff00;
			}
			break;
		case AddressingMode::ZP0:
			addr = memRead(m_pc) & 0x00ff;
			m_pc += 1;
			break;
		case AddressingMode::ZPX:
			addr = (memRead(m_pc) + m_reg_x) & 0x00ff;
			m_pc += 1;
			break;
		case AddressingMode::ZPY:
			addr = (memRead(m_pc) + m_reg_y) & 0x00ff;
			m_pc += 1;
			break;
		case AddressingMode::ABS:
			addr = memRead16(m_pc);
			m_pc += 2;
			break;
		case AddressingMode::ABX:
			addr = memRead16(m_pc) + m_reg_x;
			m_pc += 2;
			page_crossed = isPageCrossed(addr - m_reg_x, addr);
			break;
		case AddressingMode::ABY:
			addr = memRead16(m_pc) + m_reg_y;
			m_pc += 2;
			page_crossed = isPageCrossed(addr - m_reg_y, addr);
			break;
		case AddressingMode::IND: {
			auto ptr { memRead16(m_pc) };
			m_pc += 2;

			if ((ptr & 0x00ff) == 0x00ff) {
				// HACK: Simulate page boundary hardware bug.
				addr = (memRead(ptr & 0xff00) << 8) | memRead(ptr);
			} else {
				addr = memRead16(ptr);
			}
		} break;
		case AddressingMode::IZX:
			addr = memRead16(memRead(m_pc) + m_reg_x);
			m_pc += 1;
			break;
		case AddressingMode::IZY:
			addr = memRead16(memRead(m_pc)) + m_reg_y;
			page_crossed = isPageCrossed(addr - m_reg_y, addr);
			m_pc += 1;
			break;
		default:
			break;
		}

		return std::make_tuple(addr, page_crossed);
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

		setFlag(Z, m_reg_a == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	// Instruction: Bitwise logical AND
	// Result     : A = A & M
	// Flags      : A, Z, N
	void CPU::AND(u16 addr) {
		m_reg_a &= memRead(addr);

		setFlag(Z, m_reg_a == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	// Instruction: Arithmetic Shift Left
	// Result     : A = A << 1 or M = M << 1
	// Flags      : N, Z, C
	void CPU::ASL(u16 addr) {
		if (m_instruction.addressing == AddressingMode::ACC) {
			setFlag(C, m_reg_a & 0x80);
			m_reg_a <<= 1;

			setFlag(Z, m_reg_a == 0x00);
			setFlag(N, m_reg_a & 0x80);
		} else {
			auto m { memRead(addr) };
			setFlag(C, m & 0x80);
			m <<= 1;
			memWrite(addr, m);

			setFlag(Z, m == 0x00);
			setFlag(N, m & 0x80);
		}
	}

	// Instruction: Branch if Carry Clear
	// Result     : if (C == 0) pc = addr
	void CPU::BCC(u16 addr) {
		if (getFlag(C) == 0x00) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Carry Set
	// Result     : if (C == 1) pc = addr
	void CPU::BCS(u16 addr) {
		if (getFlag(C) == 0x01) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Equal
	// Result     : if (Z == 1) pc = addr
	void CPU::BEQ(u16 addr) {
		if (getFlag(Z) == 0x01) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Use ANDed A and value in Memory to set or clear Zero flag. Bits 7
	//              and 6 of the value from memory are copied into the N and V flags
	// Flags      : A&M, N=M7, V=M6
	void CPU::BIT(u16 addr) {
		auto m { memRead(addr) };
		setFlag(Z, (m & m_reg_a) == 0x00);
		setFlag(V, (m & 0x40) & 0x01);
		setFlag(N, (m & 0x80) & 0x01);
	}

	// Instruction: Branch if Negative
	// Result     : if (N == 1) pc = addr
	void CPU::BMI(u16 addr) {
		if (getFlag(N) == 0x01) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Not Equal
	// Result     : if (Z == 0) pc = addr
	void CPU::BNE(u16 addr) {
		if (getFlag(Z) == 0x00) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Positive
	// Result     : if (N == 0) pc = addr
	void CPU::BPL(u16 addr) {
		if (getFlag(N) == 0x00) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Break
	// Result     : Program sourced interrupt
	void CPU::BRK(u16 /*unused*/) {
		setFlag(I, true);
		stackPush16(m_pc);

		stackPush(m_status | B);

		m_pc = memRead16(0xfffe);
	}

	// Instruction: Branch if Overflow Clear
	// Result     : if (V == 0) pc = addr
	void CPU::BVC(u16 addr) {
		if (getFlag(V) == 0x00) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Overflow Set
	// Result     : if (V == 1) pc = addr
	void CPU::BVS(u16 addr) {
		if (getFlag(V) == 0x01) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Clear Carry Flag
	// Result     : C = 0
	void CPU::CLC(u16 /*unused*/) {
		setFlag(C, false);
	}

	// Instruction: Clear Decimal Flag
	// Result     : D = 0
	void CPU::CLD(u16 /*unused*/) {
		setFlag(C, false);
	}

	// Instruction: Clear Interrupt Flag
	// Result     : I = 0
	void CPU::CLI(u16 /*unused*/) {
		setFlag(I, false);
	}

	// Instruction: Clear Overflow Flag
	// Result     : V = 0
	void CPU::CLV(u16 /*unused*/) {
		setFlag(V, false);
	}

	// Instruction: Compare accumulator
	// Result     : C <- A >= M    Z <- A == M
	// Flags      : C, Z, N
	void CPU::CMP(u16 addr) {
		auto m { memRead(addr) };

		setFlag(C, m_reg_a >= m);
		setFlag(Z, m_reg_a == m);
		setFlag(N, (m_reg_a - m) & 0x80);
	}

	// Instruction: Compare X register
	// Result     : C <- X >= M    Z <- X == M
	// Flags      : C, Z, N
	void CPU::CPX(u16 addr) {
		auto m { memRead(addr) };

		setFlag(C, m_reg_x >= m);
		setFlag(Z, m_reg_x == m);
		setFlag(N, (m_reg_x - m) & 0x80);
	}

	// Instruction: Compare Y register
	// Result     : C <- Y >= M    Z <- Y == M
	// Flags      : C, Z, N
	void CPU::CPY(u16 addr) {
		auto m { memRead(addr) };

		setFlag(C, m_reg_y >= m);
		setFlag(Z, m_reg_y == m);
		setFlag(N, (m_reg_y - m) & 0x80);
	}

	// Instruction: Decrement value at memory location
	// Result     : M = M - 1
	// Flags      : Z, N
	void CPU::DEC(u16 addr) {
		auto m { memRead(addr) };
		m -= 1;
		memWrite(addr, m);

		setFlag(Z, m == 0x00);
		setFlag(N, m & 0x80);
	}

	// Instruction: Decrement X register
	// Result     : X = X - 1
	// Flags      : Z, N
	void CPU::DEX(u16 /*unused*/) {
		m_reg_x -= 1;
		setFlag(Z, m_reg_x == 0x00);
		setFlag(N, m_reg_x & 0x80);
	}

	// Instruction: Decrement Y register
	// Result     : Y = Y - 1
	// Flags      : Z, N
	void CPU::DEY(u16 /*unused*/) {
		m_reg_y -= 1;
		setFlag(Z, m_reg_y == 0x00);
		setFlag(N, m_reg_y & 0x80);
	}

	// Instruction: Bitwise logic XOR
	// Result     : A = A xor M
	// Flags      : Z, N
	void CPU::EOR(u16 addr) {
		m_reg_a ^= memRead(addr);
		setFlag(Z, m_reg_a == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	// Instruction: Increment value at memory location
	// Result     : M = M + 1
	// Flags      : Z, N
	void CPU::INC(u16 addr) {
		auto m { memRead(addr) };
		m += 1;
		memWrite(addr, m);

		setFlag(Z, m == 0x00);
		setFlag(N, m & 0x80);
	}

	// Instruction: Increment X register by 1
	// Result     : X + 1
	// Flags      : Z, N
	void CPU::INX(u16 /*unused*/) {
		m_reg_x += 1;

		setFlag(Z, m_reg_x == 0x00);
		setFlag(N, m_reg_x & 0x80);
	}

	// Instruction: Increment Y register by 1
	// Result     : Y + 1
	// Flags      : Z, N
	void CPU::INY(u16 /*unused*/) {
		m_reg_y += 1;

		setFlag(Z, m_reg_y == 0x00);
		setFlag(N, m_reg_y & 0x80);
	}

	// Instruction: Jump to location
	// Result     : PC = addr
	void CPU::JMP(u16 addr) {
		m_pc = addr;
	}

	// Instruction: Jump to sub-routine
	// Result     : Push PC - 1; PC = addr
	void CPU::JSR(u16 addr) {
		stackPush16(m_pc - 1);
		m_pc = addr;
	}

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

	// Instruction: Arithmetic Shift Right
	// Result     : A = A >> 1 or M = M >> 1
	// Flags      : N, Z, C
	void CPU::LSR(u16 addr) {
		if (m_instruction.addressing == AddressingMode::ACC) {
			setFlag(C, m_reg_a & 0x01);
			m_reg_a >>= 1;

			setFlag(Z, m_reg_a == 0x00);
			setFlag(N, m_reg_a & 0x80);
		} else {
			auto m { memRead(addr) };
			setFlag(C, m & 0x01);
			m >>= 1;
			memWrite(addr, m);

			setFlag(Z, m == 0x00);
			setFlag(N, m & 0x80);
		}
	}

	// Instruction: Simply do nothing
	// Result     : Consume cycles
	void CPU::NOP(u16 /*unused*/) {
		// NOTE: DO NOTHING!
	}

	// Instruction: Bitwise logic OR
	// Result     : A = A | M
	// Flags      : N, Z
	void CPU::ORA(u16 addr) {
		m_reg_a |= memRead(addr);

		setFlag(Z, m_reg_a == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	// Instruction: Push accumulator to stack
	// Result     : A -> Stack
	void CPU::PHA(u16 /*unused*/) {
		stackPush(m_reg_a);
	}

	// Instruction: Push status register to stack
	// Result     : Status -> Stack
	void CPU::PHP(u16 /*unused*/) {
		stackPush(m_status | B | U); // NOTE: Always set Unused bit to 1.
		setFlag(B, false);
	}

	// Instruction: Pull accumulator off stack
	// Result     : A <- Stack
	void CPU::PLA(u16 /*unused*/) {
		m_reg_a = stackPop();

		setFlag(Z, m_reg_a == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	// Instruction: Pull status register off stack
	// Result     : Status <- Stack
	void CPU::PLP(u16 /*unused*/) {
		m_status = stackPop() | U;
	}

	// Instruction: Move bits left and fill 7th bit with old carry value
	// Result     : A = (A << 1) | OLD_C or M = (M << 1) | OLD_C
	// Flags      : N, Z, C
	void CPU::ROL(u16 addr) {
		auto old_carry { getFlag(C) };

		if (m_instruction.addressing == AddressingMode::ACC) {
			setFlag(C, m_reg_a & 0x80);
			m_reg_a = (m_reg_a << 1) | old_carry;

			setFlag(Z, m_reg_a == 0x00);
			setFlag(N, m_reg_a & 0x80);
		} else {
			auto m { memRead(addr) };
			setFlag(C, m & 0x80);
			m = (m << 1) | old_carry;
			memWrite(addr, m);

			setFlag(Z, m == 0x00);
			setFlag(N, m & 0x80);
		}
	}

	// Instruction: Move bits right and fill bit 0 with old carry value
	// Result     : A = (A >> 1) | (OLD_C >> 7) or M = (M >> 1) | (OLD_C >> 7)
	// Flags      : N, Z, C
	void CPU::ROR(u16 addr) {
		auto old_carry { getFlag(C) << 7 };

		if (m_instruction.addressing == AddressingMode::ACC) {
			setFlag(C, m_reg_a & 0x01);
			m_reg_a = (m_reg_a >> 1) | old_carry;

			setFlag(Z, m_reg_a == 0x00);
			setFlag(N, m_reg_a & 0x80);
		} else {
			auto m { memRead(addr) };
			setFlag(C, m & 0x01);
			m = (m >> 1) | old_carry;
			memWrite(addr, m);

			setFlag(Z, m == 0x00);
			setFlag(N, m & 0x80);
		}
	}

	void CPU::RTI(u16 /*unused*/) {}

	void CPU::RTS(u16 /*unused*/) {}

	// Instruction: Subtract with Borrow In
	// Result     : A = A - M - (1 - C)
	// Flags      : C, V, N, Z
	void CPU::SBC(u16 addr) {
		auto m { memRead(addr) };
		u16 result { static_cast<u16>(static_cast<u16>(m_reg_a) - m - (1 - getFlag(C))) };

		setFlag(C, result > 0xff);
		setFlag(V, ((m_reg_a ^ result) & (~m ^ result) & 0x80) != 0x00);

		m_reg_a = result & 0x00ff;

		setFlag(Z, m_reg_a == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	// Instruction: Set Carry flag
	// Result     : C = 1
	void CPU::SEC(u16 /*unused*/) {
		setFlag(C, true);
	}

	// Instruction: Set Decimal flag
	// Result     : D = 1
	void CPU::SED(u16 /*unused*/) {
		setFlag(D, true);
	}

	// Instruction: Set Interrupt flag
	// Result     : I = 1
	void CPU::SEI(u16 /*unused*/) {
		setFlag(I, true);
	}

	// Instruction: Stores the contents of the Accumulator into memory.
	// Result     : M = A
	void CPU::STA(u16 addr) {
		memWrite(addr, m_reg_a);
	}

	// Instruction: Stores the contents of the X register into memory.
	// Result     : M = X
	void CPU::STX(u16 addr) {
		memWrite(addr, m_reg_x);
	}

	// Instruction: Stores the contents of the Y register into memory.
	// Result     : M = Y
	void CPU::STY(u16 addr) {
		memWrite(addr, m_reg_y);
	}

	// Instruction: Copy contents of the Accumulator into the X register.
	// Result     : X = A
	// Flags      : Z, N
	void CPU::TAX(u16 /*unused*/) {
		m_reg_x = m_reg_a;

		setFlag(Z, m_reg_x == 0x00);
		setFlag(N, m_reg_x & 0x80);
	}

	// Instruction: Copy contents of the Accumulator into the Y register.
	// Result     : Y = A
	// Flags      : Z, N
	void CPU::TAY(u16 /*unused*/) {
		m_reg_y = m_reg_a;

		setFlag(Z, m_reg_y == 0x00);
		setFlag(N, m_reg_y & 0x80);
	}

	// Instruction: Copy contents of the Stack Pointer into the X register.
	// Result     : X = SP
	// Flags      : Z, N
	void CPU::TSX(u16 /*unused*/) {
		m_reg_x = m_sp;
		setFlag(Z, m_reg_x == 0x00);
		setFlag(N, m_reg_x & 0x80);
	}

	// Instruction: Copy contents of the X register into the Accumulator.
	// Result     : A = X
	// Flags      : Z, N
	void CPU::TXA(u16 /*unused*/) {
		m_reg_a = m_reg_x;
		setFlag(Z, m_reg_a == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}

	// Instruction: Copy contents of the X register into the Stack Pointer.
	// Result     : SP = X
	void CPU::TXS(u16 /*unused*/) {
		m_sp = m_reg_x;
	}

	// Instruction: Copy contents of the Y register into the Accumulator.
	// Result     : A = Y
	// Flags      : Z, N
	void CPU::TYA(u16 /*unused*/) {
		m_reg_a = m_reg_y;

		setFlag(Z, m_reg_a == 0x00);
		setFlag(N, m_reg_a & 0x80);
	}
} // namespace nes
