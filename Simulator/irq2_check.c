#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int dec_string_to_int(char number[]);

int irq2_load(FILE * irq2in_file, int * addresses){
	int line_number;
	char line[256];
	int i = 0;
	while (fscanf(irq2in_file, "%[^\n]\n", line) != EOF) {
		line_number = dec_string_to_int(line);
		addresses[i] = line_number;
		i++;
	}
	return 0;
}
