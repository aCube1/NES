#ifndef _EMU_OPCODES_H_
#define _EMU_OPCODES_H_

#include "common/types.h"

#define OPCODE_COUNT 256

enum AddressingMode {
	ADDR_IMP, // Implied          | No Operand
	ADDR_ACC, // Accumulator      | A
	ADDR_IMM, // Immediate        | #VALUE
	ADDR_REL, // Relative         | SBYTE
	ADDR_ZP0, // ZeroPage         | $ZP
	ADDR_ZPX, // ZeroPage, X      | $ZP + X
	ADDR_ZPY, // ZeroPage, Y      | $ZP + Y
	ADDR_ABS, // Absolute         | $ADDR
	ADDR_ABX, // Absolute, X      | $ADDR + X
	ADDR_ABY, // Absolute, Y      | $ADDR + Y
	ADDR_IND, // Indirect         | ($ADDR)
	ADDR_IDX, // Indexed Indirect | ($ZP + X)
	ADDR_IZY, // Indirect Indexed | ($ZP) + Y
};

typedef struct CPU CPU;
typedef void (*Instruction)(CPU *);

typedef struct Opcode {
	Instruction exec;
	const char *name;
	u8 mode;
	u8 cycles;
	bool cross;
} Opcode;

extern const Opcode opcode_lookup[OPCODE_COUNT];

void opcode_execute(CPU *cpu);

// clang-format off
void op_adc(CPU *cpu); void op_and(CPU *cpu); void op_asl(CPU *cpu); void op_bcc(CPU *cpu);
void op_bcs(CPU *cpu); void op_beq(CPU *cpu); void op_bit(CPU *cpu); void op_bmi(CPU *cpu);
void op_bne(CPU *cpu); void op_bpl(CPU *cpu); void op_brk(CPU *cpu); void op_bvc(CPU *cpu);
void op_bvs(CPU *cpu); void op_clc(CPU *cpu); void op_cld(CPU *cpu); void op_cli(CPU *cpu);
void op_clv(CPU *cpu); void op_cmp(CPU *cpu); void op_cpx(CPU *cpu); void op_cpy(CPU *cpu);
void op_dec(CPU *cpu); void op_dex(CPU *cpu); void op_dey(CPU *cpu); void op_eor(CPU *cpu);
void op_inc(CPU *cpu); void op_inx(CPU *cpu); void op_iny(CPU *cpu); void op_jmp(CPU *cpu);
void op_jsr(CPU *cpu); void op_lda(CPU *cpu); void op_ldx(CPU *cpu); void op_ldy(CPU *cpu);
void op_lsr(CPU *cpu); void op_nop(CPU *cpu); void op_ora(CPU *cpu); void op_pha(CPU *cpu);
void op_php(CPU *cpu); void op_pla(CPU *cpu); void op_plp(CPU *cpu); void op_rol(CPU *cpu);
void op_ror(CPU *cpu); void op_rti(CPU *cpu); void op_rts(CPU *cpu); void op_sbc(CPU *cpu);
void op_sec(CPU *cpu); void op_sed(CPU *cpu); void op_sei(CPU *cpu); void op_sta(CPU *cpu);
void op_stx(CPU *cpu); void op_sty(CPU *cpu); void op_tax(CPU *cpu); void op_tay(CPU *cpu);
void op_tsx(CPU *cpu); void op_txa(CPU *cpu); void op_txs(CPU *cpu); void op_tya(CPU *cpu);
// clang-format on

// TODO: Proper handle 'illegal' opcodes
void op_nil(CPU *cpu);

#endif /* _EMU_OPCODES_H_ */
