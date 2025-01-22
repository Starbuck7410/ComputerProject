#include <stdio.h>
void execute_interrupt(unsigned int* io_registers, int* PC, int* in_isr){
	int PC_copy;
	if (*in_isr == 0) {
		io_registers[7] = *PC;
		*PC = io_registers[6];
		*in_isr = 1;
	}

}
