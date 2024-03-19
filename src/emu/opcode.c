#include "emu/opcodes.h"

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

void opcode_execute(CPU *cpu) {
	(void)cpu;
}

void op_adc(CPU *cpu) {

	(void)cpu;
}

void op_and(CPU *cpu) {

	(void)cpu;
}

void op_asl(CPU *cpu) {

	(void)cpu;
}

void op_bcc(CPU *cpu) {

	(void)cpu;
}

void op_bcs(CPU *cpu) {

	(void)cpu;
}

void op_beq(CPU *cpu) {

	(void)cpu;
}

void op_bit(CPU *cpu) {

	(void)cpu;
}

void op_bmi(CPU *cpu) {

	(void)cpu;
}

void op_bne(CPU *cpu) {

	(void)cpu;
}

void op_bpl(CPU *cpu) {

	(void)cpu;
}

void op_brk(CPU *cpu) {

	(void)cpu;
}

void op_bvc(CPU *cpu) {
	(void)cpu;
}

void op_bvs(CPU *cpu) {
	(void)cpu;
}

void op_clc(CPU *cpu) {
	(void)cpu;
}

void op_cld(CPU *cpu) {
	(void)cpu;
}

void op_cli(CPU *cpu) {
	(void)cpu;
}

void op_clv(CPU *cpu) {
	(void)cpu;
}

void op_cmp(CPU *cpu) {
	(void)cpu;
}

void op_cpx(CPU *cpu) {
	(void)cpu;
}

void op_cpy(CPU *cpu) {
	(void)cpu;
}

void op_dec(CPU *cpu) {
	(void)cpu;
}

void op_dex(CPU *cpu) {
	(void)cpu;
}

void op_dey(CPU *cpu) {
	(void)cpu;
}

void op_eor(CPU *cpu) {
	(void)cpu;
}

void op_inc(CPU *cpu) {
	(void)cpu;
}

void op_inx(CPU *cpu) {
	(void)cpu;
}

void op_iny(CPU *cpu) {
	(void)cpu;
}

void op_jmp(CPU *cpu) {
	(void)cpu;
}

void op_jsr(CPU *cpu) {
	(void)cpu;
}

void op_lda(CPU *cpu) {
	(void)cpu;
}

void op_ldx(CPU *cpu) {
	(void)cpu;
}

void op_ldy(CPU *cpu) {
	(void)cpu;
}

void op_lsr(CPU *cpu) {
	(void)cpu;
}

void op_nop(CPU *cpu) {
	(void)cpu;
}

void op_ora(CPU *cpu) {
	(void)cpu;
}

void op_pha(CPU *cpu) {
	(void)cpu;
}

void op_php(CPU *cpu) {
	(void)cpu;
}

void op_pla(CPU *cpu) {
	(void)cpu;
}

void op_plp(CPU *cpu) {
	(void)cpu;
}

void op_rol(CPU *cpu) {
	(void)cpu;
}

void op_ror(CPU *cpu) {
	(void)cpu;
}

void op_rti(CPU *cpu) {
	(void)cpu;
}

void op_rts(CPU *cpu) {
	(void)cpu;
}

void op_sbc(CPU *cpu) {
	(void)cpu;
}

void op_sec(CPU *cpu) {
	(void)cpu;
}

void op_sed(CPU *cpu) {
	(void)cpu;
}

void op_sei(CPU *cpu) {
	(void)cpu;
}

void op_sta(CPU *cpu) {
	(void)cpu;
}

void op_stx(CPU *cpu) {
	(void)cpu;
}

void op_sty(CPU *cpu) {
	(void)cpu;
}

void op_tax(CPU *cpu) {
	(void)cpu;
}

void op_tay(CPU *cpu) {
	(void)cpu;
}

void op_tsx(CPU *cpu) {
	(void)cpu;
}

void op_txa(CPU *cpu) {
	(void)cpu;
}

void op_txs(CPU *cpu) {
	(void)cpu;
}

void op_tya(CPU *cpu) {
	(void)cpu;
}

void op_nil(CPU *cpu) {
	(void)cpu;
}
