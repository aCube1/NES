#include "CPU.hpp"

#include <spdlog/spdlog.h>

namespace nes {
	void CPU::interpret(const std::vector<u8>& program) {
		while (true) {
			auto opcode { static_cast<size_t>(program.at(m_pc)) };
			m_pc += 1;

			switch (opcode) {
			case 0xa9: // LDA
				m_reg_a = program.at(m_pc);
				m_pc += 1;

				setFlag(Flags::Z, m_reg_a == 0);
				setFlag(Flags::N, m_reg_a & 0x80);
				break;
			case 0xaa: // TAX
				m_reg_x = m_reg_a;

				setFlag(Flags::Z, m_reg_x == 0x00);
				setFlag(Flags::N, m_reg_x & 0x80);
				break;
			case 0xe8: // INX
				m_reg_x += 1;

				setFlag(Flags::Z, m_reg_x == 0x00);
				setFlag(Flags::N, m_reg_x & 0x80);
				break;
			case 0x00: // BRK
				return;
			default:
				spdlog::error("Unable to decode {:x}.", opcode);
				return;
			}
		}
	}

	uint8_t CPU::getStatus() const {
		return m_status;
	}

	uint8_t CPU::getA() const {
		return m_reg_a;
	}

	uint8_t CPU::getX() const {
		return m_reg_x;
	}

	uint8_t CPU::getY() const {
		return m_reg_y;
	}

	uint8_t CPU::getFlag(CPU::Flags flag) const {
		return (m_status & flag) ? 1 : 0;
	}

	void CPU::setFlag(CPU::Flags flag, bool v) {
		if (v) {
			m_status |= flag;
		} else {
			m_status &= (~flag);
		}
	}
} // namespace nes
