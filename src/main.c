#include "emu/cpu.h"

#include <stdlib.h>

int main(void) {
	CPU cpu = { 0 };
	cpu_powerup(&cpu);

	cpu_shutdown(&cpu);
	return EXIT_SUCCESS;
}
