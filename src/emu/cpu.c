#include "emu/cpu.h"

#include <assert.h>
#include <string.h>

#define STACK_ADDR 0x100

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
