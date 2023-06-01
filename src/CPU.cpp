#include "CPU.hpp"

#include <spdlog/spdlog.h>

namespace nes {
	void CPU::load(const std::vector<u8>& program) {
		m_pc = 0x8000;
		std::copy(program.begin(), program.end(), m_ram.begin() + 0x8000);
	}

	void CPU::run() {
		while (true) {
			auto opcode { memRead(m_pc) };
			m_pc += 1;

			switch (opcode) {
			case 0xa9: // LDA
				m_reg_a = m_ram.at(m_pc);
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

	void CPU::reset() {
		m_reg_a = 0;
		m_reg_x = 0;
		m_reg_y = 0;
		m_status = 0;

		m_pc = memRead16(0xFFFC);
	}

	void CPU::memWrite(u16 addr, u8 data) {
		m_ram.at(addr) = data;
	}

	u16 CPU::memRead(u16 addr) const {
		u8 data {};

		data = m_ram.at(addr);

		return data;
	}

	void CPU::memWrite16(u16 addr, u16 data) {
		auto low { static_cast<u8>(data >> 8) };
		auto high { static_cast<u8>(data & 0xff) };

		memWrite(addr, low);
		memWrite(addr + 1, high);
	}

	u16 CPU::memRead16(u16 addr) const {
		auto low { memRead(addr) };
		auto high { memRead(addr + 1) };

		return (high << 8) | low;
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
