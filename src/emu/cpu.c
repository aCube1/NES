#include "emu/cpu.h"

#include "common/log.h"
#include "common/utils.h"

#include <assert.h>
#include <string.h>

#define STACK_ADDR   0x0100

#define VECTOR_NMI   0xfffa
#define VECTOR_RESET 0xfffc
#define VECTOR_IRQ   0xfffe

// MOS6502 Interrupt behaviors: https://www.nesdev.org/wiki/CPU_interrupts
static void fetch_interrupt(CPU *cpu) {
	if (!flag_check(cpu->state, STATE_INTERRUPT)) {
		return;
	}

	// Check if IRQs are enabled
	if ((cpu->reg.st & STATUS_I) && cpu->interrupt != INTERRUPT_NMI) {
		cpu->interrupt = INTERRUPT_NONE;
		return;
	}

	u16 addr;
	switch (cpu->interrupt) {
	case INTERRUPT_NMI:
		addr = VECTOR_NMI;
		break;
	case INTERRUPT_RESET:
		addr = VECTOR_RESET;
		break;
	case INTERRUPT_BRK:
		flag_set(cpu->reg.st, STATUS_B);
		// FALLTHROUGH
	case INTERRUPT_IRQ:
		addr = VECTOR_IRQ;
		break;
	default:
		log_error("[CPU] > Unknown Interrupt type: %d", cpu->interrupt);
		return;
	}

	// Writes on reset are suppressed, but SP is modified anyway
	if (cpu->interrupt == INTERRUPT_RESET) {
		cpu->reg.sp -= 3;
	} else {
		stack_push16(cpu, cpu->reg.pc);
		stack_push8(cpu, cpu->reg.st | STATUS_U);
	}

	flag_set(cpu->reg.st, STATUS_I);
	cpu->reg.pc = ram_read16(cpu, addr);

	// All interrupts take 7 cycles to complete; BRK Cycles are added on opcode execution
	if (cpu->interrupt != INTERRUPT_BRK) {
		cpu->cycles = 7;
	}

	cpu->interrupt = INTERRUPT_NONE;
}

// PowerUp state from: https://www.nesdev.org/wiki/CPU_power_up_state
void cpu_powerup(CPU *cpu) {
	assert(cpu != NULL);

	memset(cpu, 0, sizeof(CPU));
	cpu->reg.sp = 0xfd;
	cpu->reg.st = STATUS_U | STATUS_B | STATUS_I;
}

void cpu_shutdown(CPU *cpu) {
	assert(cpu != NULL);
}

void cpu_clock(CPU *cpu) {
	assert(cpu != NULL);

	if (cpu->cycles == 0) {
		if (cpu->interrupt != INTERRUPT_NONE) {
			fetch_interrupt(cpu);
		} else {
			// TODO: Execute CPU opcode step: FETCH, DECODE, EXECUTE
		}
	}

	cpu->cycles -= 1;
}

void cpu_reset(CPU *cpu) {
	cpu_interrupt(cpu, INTERRUPT_RESET);
	fetch_interrupt(cpu);
}

void cpu_interrupt(CPU *cpu, u8 interrupt) {
	assert(cpu != NULL);

	if (interrupt == INTERRUPT_IRQ && cpu->interrupt == INTERRUPT_NMI) {
		return; // Non-Maskable Interrupt preceeds Interrupt Request
	}
	cpu->interrupt = interrupt;
}

u8 ram_read8(const CPU *cpu, u16 addr) {
	assert(cpu != NULL);
	return cpu->ram[addr];
}

u16 ram_read16(const CPU *cpu, u16 addr) {
	assert(cpu != NULL);
	return (cpu->ram[addr + 1] << 8) | cpu->ram[addr];
}

void ram_write8(CPU *cpu, u16 addr, u8 data) {
	assert(cpu != NULL);

	cpu->ram[addr] = data;
}

u8 stack_pop8(CPU *cpu) {
	assert(cpu != NULL);

	cpu->reg.sp += 1;
	return ram_read8(cpu, STACK_ADDR + cpu->reg.sp);
}

u16 stack_pop16(CPU *cpu) {
	u8 lo = stack_pop8(cpu);
	u8 hi = stack_pop8(cpu);
	return (hi << 8) | lo;
}

void stack_push8(CPU *cpu, u8 data) {
	assert(cpu != NULL);

	ram_write8(cpu, STACK_ADDR + cpu->reg.sp, data);
	cpu->reg.sp -= 1;
}

void stack_push16(CPU *cpu, u16 data) {
	stack_push8(cpu, data >> 8);
	stack_push8(cpu, data & 0x00ff);
}
