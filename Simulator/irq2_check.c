#include <stdio.h>

void irq2_check(FILE* irq2in_file, int pc,int io_registers[])
{
	int lineNumber;
	char line[256];
	if (io_registers[2] == 1) io_registers[2] == 0; //turn off irq2 if irq2 was on last cycles.  

	while (fgets(line, sizeof(line), irq2in_file)) {
		lineNumber = atoi(line);
		if (lineNumber == pc) {
			io_registers[2] == 1;//irq2enable=1
			io_registers[5] == 1;//irq2status=1
		}
	}
}