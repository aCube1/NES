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
typedef void (*Instruction)(CPU *, u16);

typedef struct Address {
	u16 addr;
	u8 bytes;
	bool has_crossed;
} Address;

typedef struct Opcode {
	Instruction exec;
	const char *name;
	u8 mode;
	u8 cycles;
	u8 extra;
} Opcode;

extern const Opcode opcode_lookup[OPCODE_COUNT];

void opcode_decode(CPU *cpu);
Address opcode_getaddress(const CPU *cpu, enum AddressingMode mode);

// clang-format off
void op_adc(CPU *cpu, u16 addr); void op_and(CPU *cpu, u16 addr);
void op_asl(CPU *cpu, u16 addr); void op_bcc(CPU *cpu, u16 addr);
void op_bcs(CPU *cpu, u16 addr); void op_beq(CPU *cpu, u16 addr);
void op_bit(CPU *cpu, u16 addr); void op_bmi(CPU *cpu, u16 addr);
void op_bne(CPU *cpu, u16 addr); void op_bpl(CPU *cpu, u16 addr);
void op_brk(CPU *cpu, u16 addr); void op_bvc(CPU *cpu, u16 addr);
void op_bvs(CPU *cpu, u16 addr); void op_clc(CPU *cpu, u16 addr);
void op_cld(CPU *cpu, u16 addr); void op_cli(CPU *cpu, u16 addr);
void op_clv(CPU *cpu, u16 addr); void op_cmp(CPU *cpu, u16 addr);
void op_cpx(CPU *cpu, u16 addr); void op_cpy(CPU *cpu, u16 addr);
void op_dec(CPU *cpu, u16 addr); void op_dex(CPU *cpu, u16 addr);
void op_dey(CPU *cpu, u16 addr); void op_eor(CPU *cpu, u16 addr);
void op_inc(CPU *cpu, u16 addr); void op_inx(CPU *cpu, u16 addr);
void op_iny(CPU *cpu, u16 addr); void op_jmp(CPU *cpu, u16 addr);
void op_jsr(CPU *cpu, u16 addr); void op_lda(CPU *cpu, u16 addr);
void op_ldx(CPU *cpu, u16 addr); void op_ldy(CPU *cpu, u16 addr);
void op_lsr(CPU *cpu, u16 addr); void op_nop(CPU *cpu, u16 addr);
void op_ora(CPU *cpu, u16 addr); void op_pha(CPU *cpu, u16 addr);
void op_php(CPU *cpu, u16 addr); void op_pla(CPU *cpu, u16 addr);
void op_plp(CPU *cpu, u16 addr); void op_rol(CPU *cpu, u16 addr);
void op_ror(CPU *cpu, u16 addr); void op_rti(CPU *cpu, u16 addr);
void op_rts(CPU *cpu, u16 addr); void op_sbc(CPU *cpu, u16 addr);
void op_sec(CPU *cpu, u16 addr); void op_sed(CPU *cpu, u16 addr);
void op_sei(CPU *cpu, u16 addr); void op_sta(CPU *cpu, u16 addr);
void op_stx(CPU *cpu, u16 addr); void op_sty(CPU *cpu, u16 addr);
void op_tax(CPU *cpu, u16 addr); void op_tay(CPU *cpu, u16 addr);
void op_tsx(CPU *cpu, u16 addr); void op_txa(CPU *cpu, u16 addr);
void op_txs(CPU *cpu, u16 addr); void op_tya(CPU *cpu, u16 addr);
// clang-format on

// TODO: Proper handle 'illegal' opcodes
void op_nil(CPU *cpu, u16 addr);

#endif /* _EMU_OPCODES_H_ */
