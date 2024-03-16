#ifndef _EMU_CPU_H_
#define _EMU_CPU_H_

#include "common/types.h"

enum CpuInterrupt {
	INTERRUPT_NONE,
	INTERRUPT_NMI,   // Non-Maskable Interrupt
	INTERRUPT_RESET, // Reset Interrupt
	INTERRUPT_IRQ,   // Interrupt Request
	INTERRUPT_BRK,   // BRK Interrupt
};

enum CpuState {
	STATE_BRANCH = 0x01,
	STATE_INTERRUPT = 0x02,
};

enum ProgramStatus {
	STATUS_C = 0x01, // [0] - Carry Flag
	STATUS_Z = 0x02, // [1] - Zero Flag
	STATUS_I = 0x04, // [2] - Interrupt Disable
	STATUS_D = 0x08, // [3] - Decimal (Never Used)
	STATUS_B = 0x10, // [4] - Break Flag
	STATUS_U = 0x20, // [5] - Unused
	STATUS_V = 0x40, // [6] - Overflow Flag
	STATUS_N = 0x80, // [7] - Negative Flag
};

typedef struct CPU {
	struct {
		u16 pc; // Program Counter
		u8 sp;  // Stack Pointer
		u8 st;  // Processor Status[NV.BDIZC]

		u8 a; // Accumulator
		u8 x; // X Index
		u8 y; // Y Index
	} reg;

	u8 cycles; // Remaining cycles
	u8 state;
	u8 interrupt; // Next pending interrupt

	u8 ram[UINT16_MAX + 1];
} CPU;

void cpu_powerup(CPU *cpu);
void cpu_shutdown(CPU *cpu);

void cpu_clock(CPU *cpu);

void cpu_reset(CPU *cpu);
void cpu_interrupt(CPU *cpu, u8 interrupt);

u8 ram_read8(const CPU *cpu, u16 addr);
u16 ram_read16(const CPU *cpu, u16 addr);
void ram_write8(CPU *cpu, u16 addr, u8 data);

u8 stack_pop8(CPU *cpu);
u16 stack_pop16(CPU *cpu);
void stack_push8(CPU *cpu, u8 data);
void stack_push16(CPU *cpu, u16 data);

#endif /* _EMU_CPU_H_ */
