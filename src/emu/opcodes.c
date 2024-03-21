#include "emu/opcodes.h"

#include "common/utils.h"
#include "emu/cpu.h"

#include <assert.h>

#define is_pagecrossed(a, b) ((a) & 0xff00) != ((b) & 0xff00)

// clang-format off
const Opcode opcode_lookup[OPCODE_COUNT] = {
	{op_brk, "BRK", ADDR_IMP, 7, 0}, {op_ora, "ORA", ADDR_IDX, 6, 0}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "SLO", ADDR_IDX, 8, 0},
	{op_nop, "NOP", ADDR_ZP0, 3, 0}, {op_ora, "ORA", ADDR_ZP0, 3, 0}, {op_asl, "ASL", ADDR_ZP0, 5, 0}, {op_nil, "SLO", ADDR_ZP0, 5, 0},
	{op_php, "PHP", ADDR_IMP, 3, 0}, {op_ora, "ORA", ADDR_IMM, 2, 0}, {op_asl, "ASL", ADDR_ACC, 2, 0}, {op_nil, "ANC", ADDR_IMM, 2, 0},
	{op_nop, "NOP", ADDR_ABS, 4, 0}, {op_ora, "ORA", ADDR_ABS, 4, 0}, {op_asl, "ASL", ADDR_ABS, 6, 0}, {op_nil, "SLO", ADDR_ABS, 6, 0},
	{op_bpl, "BPL", ADDR_REL, 2, 1}, {op_ora, "ORA", ADDR_IZY, 5, 1}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "SLO", ADDR_IZY, 8, 0},
	{op_nop, "NOP", ADDR_ZPX, 4, 0}, {op_ora, "ORA", ADDR_ZPX, 4, 0}, {op_asl, "ASL", ADDR_ZPX, 6, 0}, {op_nil, "SLO", ADDR_ZPX, 6, 0},
	{op_clc, "CLC", ADDR_IMP, 2, 0}, {op_ora, "ORA", ADDR_ABY, 4, 1}, {op_nop, "NOP", ADDR_IMP, 2, 0}, {op_nil, "SLO", ADDR_ABY, 7, 0},
	{op_nop, "NOP", ADDR_ABX, 4, 5}, {op_ora, "ORA", ADDR_ABX, 4, 1}, {op_asl, "ASL", ADDR_ABX, 7, 0}, {op_nil, "SLO", ADDR_ABX, 7, 0},
	{op_jsr, "JSR", ADDR_ABS, 6, 0}, {op_and, "AND", ADDR_IDX, 6, 0}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "RLA", ADDR_IDX, 8, 0},
	{op_bit, "BIT", ADDR_ZP0, 3, 0}, {op_and, "AND", ADDR_ZP0, 3, 0}, {op_rol, "ROL", ADDR_ZP0, 5, 0}, {op_nil, "RLA", ADDR_ZP0, 5, 0},
	{op_plp, "PLP", ADDR_IMP, 4, 0}, {op_and, "AND", ADDR_IMM, 2, 0}, {op_rol, "ROL", ADDR_ACC, 2, 0}, {op_nil, "ANC", ADDR_IMM, 2, 0},
	{op_bit, "BIT", ADDR_ABS, 4, 0}, {op_and, "AND", ADDR_ABS, 4, 0}, {op_rol, "ROL", ADDR_ABS, 6, 0}, {op_nil, "RLA", ADDR_ABS, 6, 0},
	{op_bmi, "BMI", ADDR_REL, 2, 1}, {op_and, "AND", ADDR_IZY, 5, 1}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "RLA", ADDR_IZY, 8, 0},
	{op_nop, "NOP", ADDR_ZPX, 4, 0}, {op_and, "AND", ADDR_ZPX, 4, 0}, {op_rol, "ROL", ADDR_ZPX, 6, 0}, {op_nil, "RLA", ADDR_ZPX, 6, 0},
	{op_sec, "SEC", ADDR_IMP, 2, 0}, {op_and, "AND", ADDR_ABY, 4, 1}, {op_nop, "NOP", ADDR_IMP, 2, 0}, {op_nil, "RLA", ADDR_ABY, 7, 0},
	{op_nop, "NOP", ADDR_ABX, 4, 1}, {op_and, "AND", ADDR_ABX, 4, 1}, {op_rol, "ROL", ADDR_ABX, 7, 0}, {op_nil, "RLA", ADDR_ABX, 7, 0},
	{op_rti, "RTI", ADDR_IMP, 6, 0}, {op_eor, "EOR", ADDR_IDX, 6, 0}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "SRE", ADDR_IDX, 8, 0},
	{op_nop, "NOP", ADDR_ZP0, 3, 0}, {op_eor, "EOR", ADDR_ZP0, 3, 0}, {op_lsr, "LSR", ADDR_ZP0, 5, 0}, {op_nil, "SRE", ADDR_ZP0, 5, 0},
	{op_pha, "PHA", ADDR_IMP, 3, 0}, {op_eor, "EOR", ADDR_IMM, 2, 0}, {op_lsr, "LSR", ADDR_ACC, 2, 0}, {op_nil, "ASR", ADDR_IMM, 2, 0},
	{op_jmp, "JMP", ADDR_ABS, 3, 0}, {op_eor, "EOR", ADDR_ABS, 4, 0}, {op_lsr, "LSR", ADDR_ABS, 6, 0}, {op_nil, "SRE", ADDR_ABS, 6, 0},
	{op_bvc, "BVC", ADDR_REL, 2, 1}, {op_eor, "EOR", ADDR_IZY, 5, 1}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "SRE", ADDR_IZY, 8, 0},
	{op_nop, "NOP", ADDR_ZPX, 4, 0}, {op_eor, "EOR", ADDR_ZPX, 4, 0}, {op_lsr, "LSR", ADDR_ZPX, 6, 0}, {op_nil, "SRE", ADDR_ZPX, 6, 0},
	{op_cli, "CLI", ADDR_IMP, 2, 0}, {op_eor, "EOR", ADDR_ABY, 4, 1}, {op_nop, "NOP", ADDR_IMP, 2, 0}, {op_nil, "SRE", ADDR_ABY, 7, 0},
	{op_nop, "NOP", ADDR_ABX, 4, 1}, {op_eor, "EOR", ADDR_ABX, 4, 1}, {op_lsr, "LSR", ADDR_ABX, 7, 0}, {op_nil, "SRE", ADDR_ABX, 7, 0},
	{op_rts, "RTS", ADDR_IMP, 6, 0}, {op_adc, "ADC", ADDR_IDX, 6, 0}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "RRA", ADDR_IDX, 8, 0},
	{op_nop, "NOP", ADDR_ZP0, 3, 0}, {op_adc, "ADC", ADDR_ZP0, 3, 0}, {op_ror, "ROR", ADDR_ZP0, 5, 0}, {op_nil, "RRA", ADDR_ZP0, 5, 0},
	{op_pla, "PLA", ADDR_IMP, 4, 0}, {op_adc, "ADC", ADDR_IMM, 2, 0}, {op_ror, "ROR", ADDR_ACC, 2, 0}, {op_nil, "ARR", ADDR_IMM, 2, 0},
	{op_jmp, "JMP", ADDR_IND, 5, 0}, {op_adc, "ADC", ADDR_ABS, 4, 0}, {op_ror, "ROR", ADDR_ABS, 6, 0}, {op_nil, "RRA", ADDR_ABS, 6, 0},
	{op_bvs, "BVS", ADDR_REL, 2, 1}, {op_adc, "ADC", ADDR_IZY, 5, 1}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "RRA", ADDR_IZY, 8, 0},
	{op_nop, "NOP", ADDR_ZPX, 4, 0}, {op_adc, "ADC", ADDR_ZPX, 4, 0}, {op_ror, "ROR", ADDR_ZPX, 6, 0}, {op_nil, "RRA", ADDR_ZPX, 6, 0},
	{op_sei, "SEI", ADDR_IMP, 2, 0}, {op_adc, "ADC", ADDR_ABY, 4, 1}, {op_nop, "NOP", ADDR_IMP, 2, 0}, {op_nil, "RRA", ADDR_ABY, 7, 0},
	{op_nop, "NOP", ADDR_ABX, 4, 1}, {op_adc, "ADC", ADDR_ABX, 4, 1}, {op_ror, "ROR", ADDR_ABX, 7, 0}, {op_nil, "RRA", ADDR_ABX, 7, 0},
	{op_nop, "NOP", ADDR_IMM, 2, 0}, {op_sta, "STA", ADDR_IDX, 6, 0}, {op_nop, "NOP", ADDR_IMM, 2, 0}, {op_nil, "SAX", ADDR_IDX, 6, 0},
	{op_sty, "STY", ADDR_ZP0, 3, 0}, {op_sta, "STA", ADDR_ZP0, 3, 0}, {op_stx, "STX", ADDR_ZP0, 3, 0}, {op_nil, "SAX", ADDR_ZP0, 3, 0},
	{op_dey, "DEY", ADDR_IMP, 2, 0}, {op_nop, "NOP", ADDR_IMM, 2, 0}, {op_txa, "TXA", ADDR_IMP, 2, 0}, {op_nil, "XAA", ADDR_IMM, 2, 0},
	{op_sty, "STY", ADDR_ABS, 4, 0}, {op_sta, "STA", ADDR_ABS, 4, 0}, {op_stx, "STX", ADDR_ABS, 4, 0}, {op_nil, "SAX", ADDR_ABS, 4, 0},
	{op_bcc, "BCC", ADDR_REL, 2, 1}, {op_sta, "STA", ADDR_IZY, 6, 0}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "SHA", ADDR_IZY, 6, 0},
	{op_sty, "STY", ADDR_ZPX, 4, 0}, {op_sta, "STA", ADDR_ZPX, 4, 0}, {op_stx, "STX", ADDR_ZPY, 4, 0}, {op_nil, "SAX", ADDR_ZPY, 4, 0},
	{op_tya, "TYA", ADDR_IMP, 2, 0}, {op_sta, "STA", ADDR_ABY, 5, 0}, {op_txs, "TXS", ADDR_IMP, 2, 0}, {op_nil, "SHS", ADDR_ABY, 5, 0},
	{op_nil, "SHY", ADDR_ABX, 5, 0}, {op_sta, "STA", ADDR_ABX, 5, 0}, {op_nil, "SHX", ADDR_ABY, 5, 0}, {op_nil, "SHA", ADDR_ABY, 5, 0},
	{op_ldy, "LDY", ADDR_IMM, 2, 0}, {op_lda, "LDA", ADDR_IDX, 6, 0}, {op_ldx, "LDX", ADDR_IMM, 2, 0}, {op_nil, "LAX", ADDR_IDX, 6, 0},
	{op_ldy, "LDY", ADDR_ZP0, 3, 0}, {op_lda, "LDA", ADDR_ZP0, 3, 0}, {op_ldx, "LDX", ADDR_ZP0, 3, 0}, {op_nil, "LAX", ADDR_ZP0, 3, 0},
	{op_tay, "TAY", ADDR_IMP, 2, 0}, {op_lda, "LDA", ADDR_IMM, 2, 0}, {op_tax, "TAX", ADDR_IMP, 2, 0}, {op_nil, "LAX", ADDR_IMM, 2, 0},
	{op_ldy, "LDY", ADDR_ABS, 4, 0}, {op_lda, "LDA", ADDR_ABS, 4, 0}, {op_ldx, "LDX", ADDR_ABS, 4, 0}, {op_nil, "LAX", ADDR_ABS, 4, 0},
	{op_bcs, "BCS", ADDR_REL, 2, 1}, {op_lda, "LDA", ADDR_IZY, 5, 1}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "LAX", ADDR_IZY, 5, 1},
	{op_ldy, "LDY", ADDR_ZPX, 4, 0}, {op_lda, "LDA", ADDR_ZPX, 4, 0}, {op_ldx, "LDX", ADDR_ZPY, 4, 0}, {op_nil, "LAX", ADDR_ZPY, 4, 0},
	{op_clv, "CLV", ADDR_IMP, 2, 0}, {op_lda, "LDA", ADDR_ABY, 4, 1}, {op_tsx, "TSX", ADDR_IMP, 2, 0}, {op_nil, "LAS", ADDR_ABY, 4, 1},
	{op_ldy, "LDY", ADDR_ABX, 4, 1}, {op_lda, "LDA", ADDR_ABX, 4, 1}, {op_ldx, "LDX", ADDR_ABY, 4, 1}, {op_nil, "LAX", ADDR_ABY, 4, 1},
	{op_cpy, "CPY", ADDR_IMM, 2, 0}, {op_cmp, "CMP", ADDR_IDX, 6, 0}, {op_nop, "NOP", ADDR_IMM, 2, 0}, {op_nil, "DCP", ADDR_IDX, 8, 0},
	{op_cpy, "CPY", ADDR_ZP0, 3, 0}, {op_cmp, "CMP", ADDR_ZP0, 3, 0}, {op_dec, "DEC", ADDR_ZP0, 5, 0}, {op_nil, "DCP", ADDR_ZP0, 5, 0},
	{op_iny, "INY", ADDR_IMP, 2, 0}, {op_cmp, "CMP", ADDR_IMM, 2, 0}, {op_dex, "DEX", ADDR_IMP, 2, 0}, {op_nil, "SBX", ADDR_IMM, 2, 0},
	{op_cpy, "CPY", ADDR_ABS, 4, 0}, {op_cmp, "CMP", ADDR_ABS, 4, 0}, {op_dec, "DEC", ADDR_ABS, 6, 0}, {op_nil, "DCP", ADDR_ABS, 6, 0},
	{op_bne, "BNE", ADDR_REL, 2, 1}, {op_cmp, "CMP", ADDR_IZY, 5, 1}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "DCP", ADDR_IZY, 8, 0},
	{op_nop, "NOP", ADDR_ZPX, 4, 0}, {op_cmp, "CMP", ADDR_ZPX, 4, 0}, {op_dec, "DEC", ADDR_ZPX, 6, 0}, {op_nil, "DCP", ADDR_ZPX, 6, 0},
	{op_cld, "CLD", ADDR_IMP, 2, 0}, {op_cmp, "CMP", ADDR_ABY, 4, 1}, {op_nop, "NOP", ADDR_IMP, 2, 0}, {op_nil, "DCP", ADDR_ABY, 7, 0},
	{op_nop, "NOP", ADDR_ABX, 4, 1}, {op_cmp, "CMP", ADDR_ABX, 4, 1}, {op_dec, "DEC", ADDR_ABX, 7, 0}, {op_nil, "DCP", ADDR_ABX, 7, 0},
	{op_cpx, "CPX", ADDR_IMM, 2, 0}, {op_sbc, "SBC", ADDR_IDX, 6, 0}, {op_nop, "NOP", ADDR_IMM, 2, 0}, {op_nil, "ISC", ADDR_IDX, 8, 0},
	{op_cpx, "CPX", ADDR_ZP0, 3, 0}, {op_sbc, "SBC", ADDR_ZP0, 3, 0}, {op_inc, "INC", ADDR_ZP0, 5, 0}, {op_nil, "ISC", ADDR_ZP0, 5, 0},
	{op_inx, "INX", ADDR_IMP, 2, 0}, {op_sbc, "SBC", ADDR_IMM, 2, 0}, {op_nop, "NOP", ADDR_IMP, 2, 0}, {op_sbc, "SBC", ADDR_IMM, 2, 0},
	{op_cpx, "CPX", ADDR_ABS, 4, 0}, {op_sbc, "SBC", ADDR_ABS, 4, 0}, {op_inc, "INC", ADDR_ABS, 6, 0}, {op_nil, "ISC", ADDR_ABS, 6, 0},
	{op_beq, "BEQ", ADDR_REL, 2, 1}, {op_sbc, "SBC", ADDR_IZY, 5, 1}, {op_nil, "JAM", ADDR_IMP, 0, 0}, {op_nil, "ISC", ADDR_IZY, 8, 0},
	{op_nop, "NOP", ADDR_ZPX, 4, 0}, {op_sbc, "SBC", ADDR_ZPX, 4, 0}, {op_inc, "INC", ADDR_ZPX, 6, 0}, {op_nil, "ISC", ADDR_ZPX, 6, 0},
	{op_sed, "SED", ADDR_IMP, 2, 0}, {op_sbc, "SBC", ADDR_ABY, 4, 1}, {op_nop, "NOP", ADDR_IMP, 2, 0}, {op_nil, "ISC", ADDR_ABY, 7, 0},
	{op_nop, "NOP", ADDR_ABX, 4, 1}, {op_sbc, "SBC", ADDR_ABX, 4, 1}, {op_inc, "INC", ADDR_ABX, 7, 0}, {op_nil, "ISC", ADDR_ABX, 7, 0},
}; // clang-format on

static void flag_update(u8 *data, u8 flag, bool set) {
	flag_clear(*data, flag);

	if (set) {
		flag_set(*data, flag);
	}
}

static void fetch_negative_zero(CPU *cpu, u8 data) {
	flag_update(&cpu->reg.st, STATUS_N, (data & 0x80) > 0);
	flag_update(&cpu->reg.st, STATUS_N, data == 0);
}

static void fetch_carry(CPU *cpu, u16 data) {
	flag_update(&cpu->reg.st, STATUS_C, data > 0xff);
}

// Overflow flag explanation:
// https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
static void fetch_overflow(CPU *cpu, u16 m, u16 n, u16 result) {
	bool has_overflow = !((m ^ n) & 0x80) && ((m ^ result) & 0x80);
	flag_update(&cpu->reg.st, STATUS_V, has_overflow);
}

static void check_branch(CPU *cpu, i8 offset, u8 flag, u8 set) {
	if (flag_check(cpu->reg.st, flag) != set) {
		return;
	}

	cpu->reg.pc += offset;
	cpu->cycles += 1;

	if (is_pagecrossed(cpu->reg.pc - offset, cpu->reg.pc)) {
		cpu->cycles += 1;
	}
}

static u8 alu_add(CPU *cpu, u8 a, u8 b) {
	u16 result = a + b + flag_check(cpu->reg.st, STATUS_C);

	fetch_overflow(cpu, a, b, result);
	fetch_carry(cpu, result);
	fetch_negative_zero(cpu, result);

	return result;
}

void opcode_decode(CPU *cpu) {
	cpu->opcode = ram_read8(cpu, cpu->reg.pc);
	cpu->reg.pc += 1;

	Opcode op = opcode_lookup[cpu->opcode];

	Address addr = opcode_getaddress(cpu, op.mode);
	cpu->reg.pc += addr.bytes;

	op.exec(cpu, addr.addr);

	cpu->cycles += op.cycles;
	if (addr.has_crossed) {
		cpu->cycles += op.extra;
	}
}

Address opcode_getaddress(const CPU *cpu, enum AddressingMode mode) {
	Address addr = { 0 };

	switch (mode) {
	case ADDR_IMP: // FALLTHROUGH
	case ADDR_ACC:
		break;
	case ADDR_IMM:
		addr.addr = cpu->reg.pc;
		addr.bytes = 1;
		break;
	case ADDR_REL:
		addr.addr = ram_read8(cpu, cpu->reg.pc);
		addr.bytes = 1;

		// if (addr.addr & 0x80) {
		//	addr |= 0xff00;
		// }
		break;
	case ADDR_ZP0:
		addr.addr = ram_read8(cpu, cpu->reg.pc);
		addr.bytes = 1;
		break;
	case ADDR_ZPX:
		addr.addr = ram_read8(cpu, cpu->reg.pc) + cpu->reg.x;
		addr.bytes = 1;
		break;
	case ADDR_ZPY:
		addr.addr = ram_read8(cpu, cpu->reg.pc) + cpu->reg.y;
		addr.bytes = 1;
		break;
	case ADDR_ABS:
		addr.addr = ram_read16(cpu, cpu->reg.pc);
		addr.bytes = 2;
		break;
	case ADDR_ABX:
		addr.addr = ram_read16(cpu, cpu->reg.pc) + cpu->reg.x;
		addr.bytes = 2;
		addr.has_crossed = is_pagecrossed(addr.addr - cpu->reg.x, addr.addr);
		break;
	case ADDR_ABY:
		addr.addr = ram_read16(cpu, cpu->reg.pc) + cpu->reg.y;
		addr.bytes = 2;
		addr.has_crossed = is_pagecrossed(addr.addr - cpu->reg.y, addr.addr);
		break;
	case ADDR_IND: {
		u16 ptr = ram_read16(cpu, cpu->reg.pc);
		u16 hi = ram_read8(cpu, ptr + 1);
		u16 lo = ram_read8(cpu, ptr);

		// Simulate page boundary bug
		if ((ptr & 0x00ff) == 0xff) {
			hi = ram_read8(cpu, ptr & 0xff00); // $xx00
			lo = ram_read8(cpu, ptr);          // $xxff
		}

		addr.addr = (hi << 8) | lo;
		addr.bytes = 2;
	} break;
	case ADDR_IDX:
		addr.addr = ram_read16(cpu, ram_read8(cpu, cpu->reg.pc) + cpu->reg.x);
		addr.bytes = 1;
		break;
	case ADDR_IZY:
		addr.addr = ram_read16(cpu, ram_read8(cpu, cpu->reg.pc)) + cpu->reg.y;
		addr.bytes = 1;
		addr.has_crossed = is_pagecrossed(addr.addr - cpu->reg.y, addr.addr);
		break;
	default:
		assert(0);
	}

	return addr;
}

// Instruction: Add Memory to Accumulator with Carry
// Execution: A = A + M + C
// Flags: C Z I D B - V N
// 		  + + . . . . + +
void op_adc(CPU *cpu, u16 addr) {
	u8 m = ram_read8(cpu, addr);

	cpu->reg.a = alu_add(cpu, cpu->reg.a, m);
}

// Instruction: Logical AND Memory with Accumulator
// Execution: A = A AND M
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_and(CPU *cpu, u16 addr) {
	cpu->reg.a &= ram_read8(cpu, addr);

	fetch_negative_zero(cpu, cpu->reg.a);
}

// Instruction: Arithmetic shift left
// Execution: A|M = A|M << 1
// Flags: C Z I D B - V N
// 		  + + . . . . . +
void op_asl(CPU *cpu, u16 addr) {
	u16 data = cpu->reg.a;

	if (opcode_lookup[cpu->opcode].mode != ADDR_ACC) {
		data = ram_read8(cpu, addr);
	}

	data <<= 1;

	fetch_carry(cpu, data);
	fetch_negative_zero(cpu, data);

	if (opcode_lookup[cpu->opcode].mode == ADDR_ACC) {
		cpu->reg.a = data;
	} else {
		ram_write8(cpu, addr, data);
	}
}

// Instruction: Branch on Carry clear
// Execution: PC = offset if C = 0
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_bcc(CPU *cpu, u16 addr) {
	check_branch(cpu, (i8)addr, STATUS_C, 0);
}

// Instruction: Branch on Carry set
// Execution: PC = offset if C = 1
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_bcs(CPU *cpu, u16 addr) {
	check_branch(cpu, (i8)addr, STATUS_C, 1);
}

// Instruction: Branch if equal
// Execution: PC = offset if Z = 0
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_beq(CPU *cpu, u16 addr) {
	check_branch(cpu, (i8)addr, STATUS_Z, 0);
}

// Instruction: Test bits in Memory with Accumulator
// Execution: A&M, N=M7, V=M6
// Flags: C Z I D B - V  N
// 		  . + . . . . M6 M7
void op_bit(CPU *cpu, u16 addr) {
	u8 data = ram_read8(cpu, addr);

	flag_update(&cpu->reg.st, STATUS_Z, (cpu->reg.a & data) == 0x00);
	flag_update(&cpu->reg.st, STATUS_V, data & 0x40);
	flag_update(&cpu->reg.st, STATUS_N, data & 0x80);
}

// Instruction: Branch on minus
// Execution: PC = offset if N = 1
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_bmi(CPU *cpu, u16 addr) {
	check_branch(cpu, (i8)addr, STATUS_N, 1);
}

// Instruction: Branch if not equal
// Execution: PC = offset if Z = 1
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_bne(CPU *cpu, u16 addr) {
	check_branch(cpu, (i8)addr, STATUS_Z, 1);
}

// Instruction: Branch on plus
// Execution: PC = offset if N = 0
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_bpl(CPU *cpu, u16 addr) {
	check_branch(cpu, (i8)addr, STATUS_N, 0);
}

// Instruction: Force break
// Execution: Push PC, Push S, PC = $fffe, I = 1
// Flags: C Z I D B - V N
// 		  . . 1 . . . . .
void op_brk(CPU *cpu, u16 addr) {
	(void)addr;
	cpu_interrupt(cpu, INTERRUPT_BRK);
}

// Instruction: Branch on Overflow clear
// Execution: PC = offset if V = 0
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_bvc(CPU *cpu, u16 addr) {
	check_branch(cpu, (i8)addr, STATUS_V, 0);
}

// Instruction: Branch on Overflow set
// Execution: PC = offset if V = 1
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_bvs(CPU *cpu, u16 addr) {
	check_branch(cpu, (i8)addr, STATUS_V, 1);
}

// Instruction: Clear Carry flag
// Execution: C = 0
// Flags: C Z I D B - V N
// 		  1 . . . . . . .
void op_clc(CPU *cpu, u16 addr) {
	(void)addr;
	flag_clear(cpu->reg.st, STATUS_C);
}

// Instruction: Clear Decimal flag
// Execution: D = 0
// Flags: C Z I D B - V N
// 		  . . . 1 . . . .
void op_cld(CPU *cpu, u16 addr) {
	(void)addr;
	flag_clear(cpu->reg.st, STATUS_D);
}

// Instruction: Clear Interrupt flag
// Execution: I = 0
// Flags: C Z I D B - V N
// 		  . . 1 . . . . .
void op_cli(CPU *cpu, u16 addr) {
	(void)addr;
	flag_clear(cpu->reg.st, STATUS_I);
}

// Instruction: Clear Overflow flag
// Execution: V = 0
// Flags: C Z I D B - V N
// 		  . . . . . . 1 .
void op_clv(CPU *cpu, u16 addr) {
	(void)addr;
	flag_clear(cpu->reg.st, STATUS_V);
}

// Instruction: Compare Memory with Accumulator
// Execution: A - M
// Flags: C Z I D B - V N
// 		  + + . . . . . +
void op_cmp(CPU *cpu, u16 addr) {
	u16 diff = cpu->reg.a - ram_read8(cpu, addr);

	flag_update(&cpu->reg.st, STATUS_C, diff & 0xff00);
	fetch_negative_zero(cpu, diff);
}

// Instruction: Compare Memory with Index X
// Execution: X - M
// Flags: C Z I D B - V N
// 		  + + . . . . . +
void op_cpx(CPU *cpu, u16 addr) {
	u16 diff = cpu->reg.x - ram_read8(cpu, addr);

	flag_update(&cpu->reg.st, STATUS_C, diff & 0xff00);
	fetch_negative_zero(cpu, diff);
}

// Instruction: Compare Memory with Index Y
// Execution: Y - M
// Flags: C Z I D B - V N
// 		  + + . . . . . +
void op_cpy(CPU *cpu, u16 addr) {
	u16 diff = cpu->reg.y - ram_read8(cpu, addr);

	flag_update(&cpu->reg.st, STATUS_C, diff & 0xff00);
	fetch_negative_zero(cpu, diff);
}

// Instruction: Decrement Memory by 1
// Execution: M = M - 1
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_dec(CPU *cpu, u16 addr) {
	u8 data = ram_read8(cpu, addr);
	data -= 1;

	ram_write8(cpu, addr, data);
	fetch_negative_zero(cpu, data);
}

// Instruction: Decrement Index X by 1
// Execution: X = X - 1
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_dex(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.x -= 1;
	fetch_negative_zero(cpu, cpu->reg.x);
}

// Instruction: Decrement Index Y by 1
// Execution: Y = Y - 1
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_dey(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.y -= 1;
	fetch_negative_zero(cpu, cpu->reg.y);
}

// Instruction: Logical XOR Memory with Accumulator
// Execution: A = A XOR M
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_eor(CPU *cpu, u16 addr) {
	cpu->reg.a ^= ram_read8(cpu, addr);

	fetch_negative_zero(cpu, cpu->reg.a);
}

// Instruction: Increment Memory by 1
// Execution: M = M + 1
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_inc(CPU *cpu, u16 addr) {
	u8 data = ram_read8(cpu, addr);
	data += 1;

	ram_write8(cpu, addr, data);
	fetch_negative_zero(cpu, data);
}

// Instruction: Increment Index X by 1
// Execution: X = X + 1
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_inx(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.x += 1;
	fetch_negative_zero(cpu, cpu->reg.x);
}

// Instruction: Increment Index Y by 1
// Execution: Y = Y + 1
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_iny(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.y += 1;
	fetch_negative_zero(cpu, cpu->reg.y);
}

// Instruction: Jump to new location
// Execution: PC = addr
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_jmp(CPU *cpu, u16 addr) {
	cpu->reg.pc = addr;
}

// Instruction: Jump to Subroutine, saving return address
// Execution: Push PC, PC = addr
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_jsr(CPU *cpu, u16 addr) {
	stack_push16(cpu, cpu->reg.pc);
	cpu->reg.pc = addr;
}

// Instruction: Load Accumulator with Memory
// Execution: A = M
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_lda(CPU *cpu, u16 addr) {
	cpu->reg.a = ram_read8(cpu, addr);
	fetch_negative_zero(cpu, cpu->reg.a);
}

// Instruction: Load X Index with Memory
// Execution: X = M
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_ldx(CPU *cpu, u16 addr) {
	cpu->reg.x = ram_read8(cpu, addr);
	fetch_negative_zero(cpu, cpu->reg.x);
}

// Instruction: Load Y Index with Memory
// Execution: Y = M
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_ldy(CPU *cpu, u16 addr) {
	cpu->reg.y = ram_read8(cpu, addr);
	fetch_negative_zero(cpu, cpu->reg.y);
}

// Instruction: Logical shift right
// Execution: A|M = A|M >> 1
// Flags: C Z I D B - V N
// 		  + + . . . . . +
void op_lsr(CPU *cpu, u16 addr) {
	u8 data = cpu->reg.a;

	if (opcode_lookup[cpu->opcode].mode != ADDR_ACC) {
		data = ram_read8(cpu, addr);
	}

	flag_update(&cpu->reg.st, STATUS_C, (data & 0x01) > 0);

	data >>= 1;

	fetch_negative_zero(cpu, data);

	if (opcode_lookup[cpu->opcode].mode == ADDR_ACC) {
		cpu->reg.a = data;
	} else {
		ram_write8(cpu, addr, data);
	}
}

// Instruction: No operation
// Execution: ---
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_nop(CPU *cpu, u16 addr) {
	(void)cpu;
	(void)addr;
}

// Instruction: Logical OR Memory with Accumulator
// Execution: A = A OR M
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_ora(CPU *cpu, u16 addr) {
	cpu->reg.a |= ram_read8(cpu, addr);

	fetch_negative_zero(cpu, cpu->reg.a);
}

// Instruction: Push Accumulator on Stack
// Execution: Push A
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_pha(CPU *cpu, u16 addr) {
	(void)addr;
	stack_push8(cpu, cpu->reg.a);
}

// Instruction: Push Processor Status on Stack
// Execution: Push S
// Flags: C Z I D B - V N
// 		  - - - - - - - -
void op_php(CPU *cpu, u16 addr) {
	(void)addr;
	// PHP always push Status with Break and Unused bits set
	stack_push8(cpu, cpu->reg.st | STATUS_B | STATUS_U);
}

// Instruction: Pull Accumulator from Stack
// Execution: A = Pop A
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_pla(CPU *cpu, u16 addr) {
	(void)addr;
	cpu->reg.a = stack_pop8(cpu);
}

// Instruction: Pull Processor Status from Stack
// Execution: S = Pop S
// Flags: C Z I D B - V N
// 		  * * * * * * * *
void op_plp(CPU *cpu, u16 addr) {
	(void)addr;
	cpu->reg.st = stack_pop8(cpu);
}

// Instruction: Rotate 1 bit Left
// Execution: A|M = (A|M << 1) OR OLD_C
// Flags: C Z I D B - V N
// 		  + + . . . . . +
void op_rol(CPU *cpu, u16 addr) {
	u8 old_c = flag_check(cpu->reg.st, STATUS_C);
	u16 data = cpu->reg.a;

	if (opcode_lookup[cpu->opcode].mode != ADDR_ACC) {
		data = ram_read8(cpu, addr);
	}

	data = (data << 1) | old_c;

	fetch_carry(cpu, data);
	fetch_negative_zero(cpu, data);

	if (opcode_lookup[cpu->opcode].mode == ADDR_ACC) {
		cpu->reg.a = data;
	} else {
		ram_write8(cpu, addr, data);
	}
}

// Instruction: Rotate 1 bit Right
// Execution: A|M = (A|M >> 1) OR (OLD_C << 7)
// Flags: C Z I D B - V N
// 		  + + . . . . . +
void op_ror(CPU *cpu, u16 addr) {
	u8 old_c = flag_check(cpu->reg.st, STATUS_C) << 7;
	u16 data = cpu->reg.a;

	if (opcode_lookup[cpu->opcode].mode != ADDR_ACC) {
		data = ram_read8(cpu, addr);
	}

	flag_update(&cpu->reg.st, STATUS_C, (data & 0x01) > 0);
	data = old_c | (data >> 1);

	fetch_negative_zero(cpu, data);

	if (opcode_lookup[cpu->opcode].mode == ADDR_ACC) {
		cpu->reg.a = data;
	} else {
		ram_write8(cpu, addr, data);
	}
}

// Instruction: Return from Interrupt
// Execution: S = Pop S, PC = Pop PC
// Flags: C Z I D B - V N
// 		  * * * * * * * *
void op_rti(CPU *cpu, u16 addr) {
	(void)addr;
	// NOTE: This site[1] says that the Break flag and Bit 5 are ignored when pulling
	// the Processor Status from stack, but another site[2] says it does nothing about it
	// [1]: https://www.masswerk.at/6502/6502_instruction_set.html
	// [2]: https://www.nesdev.org/obelisk-6502-guide/reference.html
	//
	// cpu->reg.st = stack_pop8(cpu) | ~(STATUS_B | STATUS_U);

	cpu->reg.st = stack_pop8(cpu);
	cpu->reg.pc = stack_pop16(cpu);
}

// Instruction: Return from Subroutine
// Execution: PC = Pop PC + 1
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_rts(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.pc = stack_pop16(cpu);
}

// Instruction: Subtract Memory to Accumulator with Carry
// Execution: A = A + (~M) + C
// Flags: C Z I D B - V N
// 		  + + . . . . + +
void op_sbc(CPU *cpu, u16 addr) {
	u8 m = ram_read8(cpu, addr);

	// The subtraction operation is essentially a ADC but with the Memory negated:
	// https://www.middle-engine.com/blog/posts/2020/06/23/programming-the-nes-the-6502-in-detail#subtraction-as-addition
	cpu->reg.a = alu_add(cpu, cpu->reg.a, ~m);
}

// Instruction: Set Carry flag
// Execution: C = 1
// Flags: C Z I D B - V N
// 		  1 . . . . . . .
void op_sec(CPU *cpu, u16 addr) {
	(void)addr;
	flag_set(cpu->reg.st, STATUS_C);
}

// Instruction: Set Decimal flag
// Execution: D = 1
// Flags: C Z I D B - V N
// 		  . . . 1 . . . .
void op_sed(CPU *cpu, u16 addr) {
	(void)addr;
	flag_set(cpu->reg.st, STATUS_D);
}

// Instruction: Set Interrupt flag
// Execution: I = 1
// Flags: C Z I D B - V N
// 		  . . I . . . . .
void op_sei(CPU *cpu, u16 addr) {
	(void)addr;
	flag_set(cpu->reg.st, STATUS_I);
}

// Instruction: Store Accumulator in Memory
// Execution: M = A
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_sta(CPU *cpu, u16 addr) {
	ram_write8(cpu, addr, cpu->reg.a);
}

// Instruction: Store Index X in Memory
// Execution: M = X
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_stx(CPU *cpu, u16 addr) {
	ram_write8(cpu, addr, cpu->reg.a);
}

// Instruction: Store Index Y in Memory
// Execution: M = Y
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_sty(CPU *cpu, u16 addr) {
	ram_write8(cpu, addr, cpu->reg.a);
}

// Instruction: Transfer Accumulator to Index X
// Execution: X = A
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_tax(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.x = cpu->reg.a;
	fetch_negative_zero(cpu, cpu->reg.x);
}

// Instruction: Transfer Accumulator to Index Y
// Execution: Y = A
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_tay(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.y = cpu->reg.a;
	fetch_negative_zero(cpu, cpu->reg.y);
}

// Instruction: Transfer Stack Pointer to Index X
// Execution: X = SP
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_tsx(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.x = cpu->reg.sp;
	fetch_negative_zero(cpu, cpu->reg.x);
}

// Instruction: Transfer Index X to Accumulator
// Execution: A = X
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_txa(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.a = cpu->reg.x;
	fetch_negative_zero(cpu, cpu->reg.a);
}

// Instruction: Transfer Index X to Stack Pointer
// Execution: SP = X
// Flags: C Z I D B - V N
// 		  . . . . . . . .
void op_txs(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.sp = cpu->reg.x;
}

// Instruction: Transfer Index Y to Accumulator
// Execution: A = Y
// Flags: C Z I D B - V N
// 		  . + . . . . . +
void op_tya(CPU *cpu, u16 addr) {
	(void)addr;

	cpu->reg.a = cpu->reg.y;
	fetch_negative_zero(cpu, cpu->reg.a);
}

void op_nil(CPU *cpu, u16 addr) {
	op_nop(cpu, addr);
}
