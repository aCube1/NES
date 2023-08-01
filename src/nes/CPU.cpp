#include "nes/CPU.hpp"

#include "nes/Bus.hpp"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace {
	[[nodiscard]] bool isPageCrossed(u16 a, u16 b) {
		return (a & 0xff00) != (b & 0xff00);
	}
} // namespace

namespace nes {
	CPU::CPU() {
		// clang-format off
		m_optable = {
			Opcode { "BRK", IMP, &CPU::BRK, 7, 0 }, Opcode { "ORA", IZX, &CPU::ORA, 6, 0 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZX, &CPU::NIL, 8, 0 },
			Opcode { "NOP", ZP0, &CPU::NOP, 3, 0 }, Opcode { "ORA", ZP0, &CPU::ORA, 3, 0 }, Opcode { "ASL", ZP0, &CPU::ASL, 5, 0 }, Opcode { "???", ZP0, &CPU::NIL, 5, 0 },
			Opcode { "PHP", IMP, &CPU::PHP, 3, 0 }, Opcode { "ORA", IMM, &CPU::ORA, 2, 0 }, Opcode { "ASL", ACC, &CPU::ASL, 2, 0 }, Opcode { "???", IMM, &CPU::NIL, 2, 0 },
			Opcode { "NOP", ABS, &CPU::NOP, 4, 0 }, Opcode { "ORA", ABS, &CPU::ORA, 4, 0 }, Opcode { "ASL", ABS, &CPU::ASL, 6, 0 }, Opcode { "???", ABS, &CPU::NIL, 6, 0 },
			Opcode { "BPL", REL, &CPU::BPL, 2, 1 }, Opcode { "ORA", IZY, &CPU::ORA, 5, 1 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZY, &CPU::NIL, 8, 0 },
			Opcode { "NOP", ZPX, &CPU::NOP, 4, 0 }, Opcode { "ORA", ZPX, &CPU::ORA, 4, 0 }, Opcode { "ASL", ZPX, &CPU::ASL, 6, 0 }, Opcode { "???", ZPX, &CPU::NIL, 6, 0 },
			Opcode { "CLC", IMP, &CPU::CLC, 2, 0 }, Opcode { "ORA", ABY, &CPU::ORA, 4, 1 }, Opcode { "NOP", IMP, &CPU::NOP, 2, 0 }, Opcode { "???", ABY, &CPU::NIL, 7, 0 },
			Opcode { "NOP", ABX, &CPU::NOP, 4, 1 }, Opcode { "ORA", ABX, &CPU::ORA, 4, 1 }, Opcode { "ASL", ABX, &CPU::ASL, 7, 0 }, Opcode { "???", ABX, &CPU::NIL, 7, 0 },
			Opcode { "JSR", ABS, &CPU::JSR, 6, 0 }, Opcode { "AND", IZX, &CPU::AND, 6, 0 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZX, &CPU::NIL, 8, 0 },
			Opcode { "BIT", ZP0, &CPU::BIT, 3, 0 }, Opcode { "AND", ZP0, &CPU::AND, 3, 0 }, Opcode { "ROL", ZP0, &CPU::ROL, 5, 0 }, Opcode { "???", ZP0, &CPU::NIL, 5, 0 },
			Opcode { "PLP", IMP, &CPU::PLP, 4, 0 }, Opcode { "AND", IMM, &CPU::AND, 2, 0 }, Opcode { "ROL", ACC, &CPU::ROL, 2, 0 }, Opcode { "???", IMM, &CPU::NIL, 2, 0 },
			Opcode { "BIT", ABS, &CPU::BIT, 4, 0 }, Opcode { "AND", ABS, &CPU::AND, 4, 0 }, Opcode { "ROL", ABS, &CPU::ROL, 6, 0 }, Opcode { "???", ABS, &CPU::NIL, 6, 0 },
			Opcode { "BMI", REL, &CPU::BMI, 2, 1 }, Opcode { "AND", IZY, &CPU::AND, 5, 1 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZY, &CPU::NIL, 8, 0 },
			Opcode { "NOP", ZPX, &CPU::NOP, 4, 0 }, Opcode { "AND", ZPX, &CPU::AND, 4, 0 }, Opcode { "ROL", ZPX, &CPU::ROL, 6, 0 }, Opcode { "???", ZPX, &CPU::NIL, 6, 0 },
			Opcode { "SEC", IMP, &CPU::SEC, 2, 0 }, Opcode { "AND", ABY, &CPU::AND, 4, 1 }, Opcode { "NOP", IMP, &CPU::NOP, 2, 0 }, Opcode { "???", ABY, &CPU::NIL, 7, 0 },
			Opcode { "NOP", ABX, &CPU::NOP, 4, 1 }, Opcode { "AND", ABX, &CPU::AND, 4, 1 }, Opcode { "ROL", ABX, &CPU::ROL, 7, 0 }, Opcode { "???", ABX, &CPU::NIL, 7, 0 },
			Opcode { "RTI", IMP, &CPU::RTI, 6, 0 }, Opcode { "EOR", IZX, &CPU::EOR, 6, 0 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZX, &CPU::NIL, 8, 0 },
			Opcode { "NOP", ZP0, &CPU::NOP, 3, 0 }, Opcode { "EOR", ZP0, &CPU::EOR, 3, 0 }, Opcode { "LSR", ZP0, &CPU::LSR, 5, 0 }, Opcode { "???", ZP0, &CPU::NIL, 5, 0 },
			Opcode { "PHA", IMP, &CPU::PHA, 3, 0 }, Opcode { "EOR", IMM, &CPU::EOR, 2, 0 }, Opcode { "LSR", ACC, &CPU::LSR, 2, 0 }, Opcode { "???", IMM, &CPU::NIL, 2, 0 },
			Opcode { "JMP", ABS, &CPU::JMP, 3, 0 }, Opcode { "EOR", ABS, &CPU::EOR, 4, 0 }, Opcode { "LSR", ABS, &CPU::LSR, 6, 0 }, Opcode { "???", ABS, &CPU::NIL, 6, 0 },
			Opcode { "BVC", REL, &CPU::BVC, 2, 1 }, Opcode { "EOR", IZY, &CPU::EOR, 5, 1 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZY, &CPU::NIL, 8, 0 },
			Opcode { "NOP", ZPX, &CPU::NOP, 4, 0 }, Opcode { "EOR", ZPX, &CPU::EOR, 4, 0 }, Opcode { "LSR", ZPX, &CPU::LSR, 6, 0 }, Opcode { "???", ZPX, &CPU::NIL, 6, 0 },
			Opcode { "CLI", IMP, &CPU::CLI, 2, 0 }, Opcode { "EOR", ABY, &CPU::EOR, 4, 1 }, Opcode { "NOP", IMP, &CPU::NOP, 2, 0 }, Opcode { "???", ABY, &CPU::NIL, 7, 0 },
			Opcode { "NOP", ABX, &CPU::NOP, 4, 1 }, Opcode { "EOR", ABX, &CPU::EOR, 4, 1 }, Opcode { "LSR", ABX, &CPU::LSR, 7, 0 }, Opcode { "???", ABX, &CPU::NIL, 7, 0 },
			Opcode { "RTS", IMP, &CPU::RTS, 6, 0 }, Opcode { "ADC", IZX, &CPU::ADC, 6, 0 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZX, &CPU::NIL, 8, 0 },
			Opcode { "NOP", ZP0, &CPU::NOP, 3, 0 }, Opcode { "ADC", ZP0, &CPU::ADC, 3, 0 }, Opcode { "ROR", ZP0, &CPU::ROR, 5, 0 }, Opcode { "???", ZP0, &CPU::NIL, 5, 0 },
			Opcode { "PLA", IMP, &CPU::PLA, 4, 0 }, Opcode { "ADC", IMM, &CPU::ADC, 2, 0 }, Opcode { "ROR", ACC, &CPU::ROR, 2, 0 }, Opcode { "???", IMM, &CPU::NIL, 2, 0 },
			Opcode { "JMP", IND, &CPU::JMP, 5, 0 }, Opcode { "ADC", ABS, &CPU::ADC, 4, 0 }, Opcode { "ROR", ABS, &CPU::ROR, 6, 0 }, Opcode { "???", ABS, &CPU::NIL, 6, 0 },
			Opcode { "BVS", REL, &CPU::BVS, 2, 1 }, Opcode { "ADC", IZY, &CPU::ADC, 5, 1 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZY, &CPU::NIL, 8, 0 },
			Opcode { "NOP", ZPX, &CPU::NOP, 4, 0 }, Opcode { "ADC", ZPX, &CPU::ADC, 4, 0 }, Opcode { "ROR", ZPX, &CPU::ROR, 6, 0 }, Opcode { "???", ZPX, &CPU::NIL, 6, 0 },
			Opcode { "SEI", IMP, &CPU::SEI, 2, 0 }, Opcode { "ADC", ABY, &CPU::ADC, 4, 1 }, Opcode { "NOP", IMP, &CPU::NOP, 2, 0 }, Opcode { "???", ABY, &CPU::NIL, 7, 0 },
			Opcode { "NOP", ABX, &CPU::NOP, 4, 1 }, Opcode { "ADC", ABX, &CPU::ADC, 4, 1 }, Opcode { "ROR", ABX, &CPU::ROR, 7, 0 }, Opcode { "???", ABX, &CPU::NIL, 7, 0 },
			Opcode { "NOP", IMM, &CPU::NOP, 2, 0 }, Opcode { "STA", IZX, &CPU::STA, 6, 0 }, Opcode { "NOP", IMM, &CPU::NOP, 2, 0 }, Opcode { "???", IZX, &CPU::NIL, 6, 0 },
			Opcode { "STY", ZP0, &CPU::STY, 3, 0 }, Opcode { "STA", ZP0, &CPU::STA, 3, 0 }, Opcode { "STX", ZP0, &CPU::STX, 3, 0 }, Opcode { "???", ZP0, &CPU::NIL, 3, 0 },
			Opcode { "DEY", IMP, &CPU::DEY, 2, 0 }, Opcode { "NOP", IMM, &CPU::NOP, 2, 0 }, Opcode { "TXA", IMP, &CPU::TXA, 2, 0 }, Opcode { "???", IMM, &CPU::NIL, 2, 0 },
			Opcode { "STY", ABS, &CPU::STY, 4, 0 }, Opcode { "STA", ABS, &CPU::STA, 4, 0 }, Opcode { "STX", ABS, &CPU::STX, 4, 0 }, Opcode { "???", ABS, &CPU::NIL, 4, 0 },
			Opcode { "BCC", REL, &CPU::BCC, 2, 1 }, Opcode { "STA", IZY, &CPU::STA, 6, 0 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZY, &CPU::NIL, 6, 0 },
			Opcode { "STY", ZPX, &CPU::STY, 4, 0 }, Opcode { "STA", ZPX, &CPU::STA, 4, 0 }, Opcode { "STX", ZPY, &CPU::STX, 4, 0 }, Opcode { "???", ZPY, &CPU::NIL, 4, 0 },
			Opcode { "TYA", IMP, &CPU::TYA, 2, 0 }, Opcode { "STA", ABY, &CPU::STA, 5, 0 }, Opcode { "TXS", IMP, &CPU::TXS, 2, 0 }, Opcode { "???", ABY, &CPU::NIL, 5, 0 },
			Opcode { "???", ABX, &CPU::NIL, 5, 0 }, Opcode { "STA", ABX, &CPU::STA, 5, 0 }, Opcode { "???", ABY, &CPU::NIL, 5, 0 }, Opcode { "???", ABY, &CPU::NIL, 5, 0 },
			Opcode { "LDY", IMM, &CPU::LDY, 2, 0 }, Opcode { "LDA", IZX, &CPU::LDA, 6, 0 }, Opcode { "LDX", IMM, &CPU::LDX, 2, 0 }, Opcode { "???", IZX, &CPU::NIL, 6, 0 },
			Opcode { "LDY", ZP0, &CPU::LDY, 3, 0 }, Opcode { "LDA", ZP0, &CPU::LDA, 3, 0 }, Opcode { "LDX", ZP0, &CPU::LDX, 3, 0 }, Opcode { "???", ZP0, &CPU::NIL, 3, 0 },
			Opcode { "TAY", IMP, &CPU::TAY, 2, 0 }, Opcode { "LDA", IMM, &CPU::LDA, 2, 0 }, Opcode { "TAX", IMP, &CPU::TAX, 2, 0 }, Opcode { "???", IMM, &CPU::NIL, 2, 0 },
			Opcode { "LDY", ABS, &CPU::LDY, 4, 0 }, Opcode { "LDA", ABS, &CPU::LDA, 4, 0 }, Opcode { "LDX", ABS, &CPU::LDX, 4, 0 }, Opcode { "???", ABS, &CPU::NIL, 4, 0 },
			Opcode { "BCS", REL, &CPU::BCS, 2, 1 }, Opcode { "LDA", IZY, &CPU::LDA, 5, 1 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZY, &CPU::NIL, 5, 1 },
			Opcode { "LDY", ZPX, &CPU::LDY, 4, 0 }, Opcode { "LDA", ZPX, &CPU::LDA, 4, 0 }, Opcode { "LDX", ZPY, &CPU::LDX, 4, 0 }, Opcode { "???", ZPY, &CPU::NIL, 4, 0 },
			Opcode { "CLV", IMP, &CPU::CLV, 2, 0 }, Opcode { "LDA", ABY, &CPU::LDA, 4, 1 }, Opcode { "TSX", IMP, &CPU::TSX, 2, 0 }, Opcode { "???", ABY, &CPU::NIL, 4, 1 },
			Opcode { "LDY", ABX, &CPU::LDY, 4, 1 }, Opcode { "LDA", ABX, &CPU::LDA, 4, 1 }, Opcode { "LDX", ABY, &CPU::LDX, 4, 1 }, Opcode { "???", ABY, &CPU::NIL, 4, 1 },
			Opcode { "CPY", IMM, &CPU::CPY, 2, 0 }, Opcode { "CMP", IZX, &CPU::CMP, 6, 0 }, Opcode { "NOP", IMM, &CPU::NOP, 2, 0 }, Opcode { "???", IZX, &CPU::NIL, 8, 0 },
			Opcode { "CPY", ZP0, &CPU::CPY, 3, 0 }, Opcode { "CMP", ZP0, &CPU::CMP, 3, 0 }, Opcode { "DEC", ZP0, &CPU::DEC, 5, 0 }, Opcode { "???", ZP0, &CPU::NIL, 5, 0 },
			Opcode { "INY", IMP, &CPU::INY, 2, 0 }, Opcode { "CMP", IMM, &CPU::CMP, 2, 0 }, Opcode { "DEX", IMP, &CPU::DEX, 2, 0 }, Opcode { "???", IMM, &CPU::NIL, 2, 0 },
			Opcode { "CPY", ABS, &CPU::CPY, 4, 0 }, Opcode { "CMP", ABS, &CPU::CMP, 4, 0 }, Opcode { "DEC", ABS, &CPU::DEC, 6, 0 }, Opcode { "???", ABS, &CPU::NIL, 6, 0 },
			Opcode { "BNE", REL, &CPU::BNE, 2, 1 }, Opcode { "CMP", IZY, &CPU::CMP, 5, 1 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZY, &CPU::NIL, 8, 0 },
			Opcode { "NOP", ZPX, &CPU::NOP, 4, 0 }, Opcode { "CMP", ZPX, &CPU::CMP, 4, 0 }, Opcode { "DEC", ZPX, &CPU::DEC, 6, 0 }, Opcode { "???", ZPX, &CPU::NIL, 6, 0 },
			Opcode { "CLD", IMP, &CPU::CLD, 2, 0 }, Opcode { "CMP", ABY, &CPU::CMP, 4, 1 }, Opcode { "NOP", IMP, &CPU::NOP, 2, 0 }, Opcode { "???", ABY, &CPU::NIL, 7, 0 },
			Opcode { "NOP", ABX, &CPU::NOP, 4, 1 }, Opcode { "CMP", ABX, &CPU::CMP, 4, 1 }, Opcode { "DEC", ABX, &CPU::DEC, 7, 0 }, Opcode { "???", ABX, &CPU::NIL, 7, 0 },
			Opcode { "CPX", IMM, &CPU::CPX, 2, 0 }, Opcode { "SBC", IZX, &CPU::SBC, 6, 0 }, Opcode { "NOP", IMM, &CPU::NOP, 2, 0 }, Opcode { "???", IZX, &CPU::NIL, 8, 0 },
			Opcode { "CPX", ZP0, &CPU::CPX, 3, 0 }, Opcode { "SBC", ZP0, &CPU::SBC, 3, 0 }, Opcode { "INC", ZP0, &CPU::INC, 5, 0 }, Opcode { "???", ZP0, &CPU::NIL, 5, 0 },
			Opcode { "INX", IMP, &CPU::INX, 2, 0 }, Opcode { "SBC", IMM, &CPU::SBC, 2, 0 }, Opcode { "NOP", IMP, &CPU::NOP, 2, 0 }, Opcode { "SBC", IMM, &CPU::SBC, 2, 0 },
			Opcode { "CPX", ABS, &CPU::CPX, 4, 0 }, Opcode { "SBC", ABS, &CPU::SBC, 4, 0 }, Opcode { "INC", ABS, &CPU::INC, 6, 0 }, Opcode { "???", ABS, &CPU::NIL, 6, 0 },
			Opcode { "BEQ", REL, &CPU::BEQ, 2, 1 }, Opcode { "SBC", IZY, &CPU::SBC, 5, 1 }, Opcode { "???", IMP, &CPU::NIL, 2, 0 }, Opcode { "???", IZY, &CPU::NIL, 8, 0 },
			Opcode { "NOP", ZPX, &CPU::NOP, 4, 0 }, Opcode { "SBC", ZPX, &CPU::SBC, 4, 0 }, Opcode { "INC", ZPX, &CPU::INC, 6, 0 }, Opcode { "???", ZPX, &CPU::NIL, 6, 0 },
			Opcode { "SED", IMP, &CPU::SED, 2, 0 }, Opcode { "SBC", ABY, &CPU::SBC, 4, 1 }, Opcode { "NOP", IMP, &CPU::NOP, 2, 0 }, Opcode { "???", ABY, &CPU::NIL, 7, 0 },
			Opcode { "NOP", ABX, &CPU::NOP, 4, 1 }, Opcode { "SBC", ABX, &CPU::SBC, 4, 1 }, Opcode { "INC", ABX, &CPU::INC, 7, 0 }, Opcode { "???", ABX, &CPU::NIL, 7, 0 },
		};
		// clang-format on
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
		m_status = 0x24;
		m_sp = 0xfd;

		m_reg_a = 0x00;
		m_reg_x = 0x00;
		m_reg_y = 0x00;

		m_cycles = 8; // Reset takes time.
	}

	void CPU::irq() {
		// Is interrupt allowed
		if (getFlag(I)) {
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
			"{0:#06x} {1:#04x} {2}      A:{3:#04x} X:{4:#04x} Y:{5:#04x} P:{6:#04x} "
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

	void CPU::stackPush16(u16 data) {
		stackPush((data >> 8) & 0x00ff);
		stackPush(data & 0x00ff);
	}

	u8 CPU::stackPop() {
		m_sp += 1;
		return memRead(0x0100 + m_sp);
	}

	u16 CPU::stackPop16() {
		auto l { stackPop() };
		auto h { stackPop() };
		return (h << 8) | l;
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
		if (!getFlag(C)) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Carry Set
	// Result     : if (C == 1) pc = addr
	void CPU::BCS(u16 addr) {
		if (getFlag(C)) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Equal
	// Result     : if (Z == 1) pc = addr
	void CPU::BEQ(u16 addr) {
		if (getFlag(Z)) {
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
		setFlag(V, m & 0x40);
		setFlag(N, m & 0x80);
	}

	// Instruction: Branch if Negative
	// Result     : if (N == 1) pc = addr
	void CPU::BMI(u16 addr) {
		if (getFlag(N)) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Not Equal
	// Result     : if (Z == 0) pc = addr
	void CPU::BNE(u16 addr) {
		if (!getFlag(Z)) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Positive
	// Result     : if (N == 0) pc = addr
	void CPU::BPL(u16 addr) {
		if (!getFlag(N)) {
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
		if (!getFlag(V)) {
			m_pc += addr;
			m_cycles += isPageCrossed(m_pc, m_pc - addr) ? 2 : 1;
		}
	}

	// Instruction: Branch if Overflow Set
	// Result     : if (V == 1) pc = addr
	void CPU::BVS(u16 addr) {
		if (getFlag(V)) {
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
		setFlag(D, false);
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
		m_status = (stackPop() & 0xef) | 0x20;
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

	// Instruction: Return from interrupt.
	// Result     : Status <- Stack and PC <- Stack
	void CPU::RTI(u16 /*unused*/) {
		m_status = stackPop() | B | U;
		m_pc = stackPop16();
	}

	// Instruction: Return from sub-routine
	// Result     : PC <- Stack
	void CPU::RTS(u16 /*unused*/) {
		m_pc = stackPop16() + 1;
	}

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

	// Instruction: Simply do nothing
	// Result     : Cycles wasted, have a good day!
	// Flags      : Nothing is changed, okay?
	void CPU::NIL(u16 /* unused */) {
		// HEY MAN, THIS FUNCTION DOES NOTHING!
	}
} // namespace nes
