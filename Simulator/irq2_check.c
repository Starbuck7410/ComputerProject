#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
long long pow_int(int a, int b);
int dec_string_to_int(char number[]);


void irq2_check(FILE* irq2in_file, int cycles, int io_registers[])
{
	int line_number;
	char line[256];
	if (io_registers[5] == 1) io_registers[5] = 0; //turn off irq2 if irq2 was on last cycles.  
	rewind(irq2in_file);
	while (fscanf(irq2in_file, "%[^\n]\n", line) != EOF) {
		line_number = dec_string_to_int(line);
		if (line_number == cycles) {
			io_registers[5] = 1; //irq2status=1
			return;
		}
	}
}