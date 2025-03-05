#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
int dec_string_to_int(char number[]);

int irq2_load(char * irq2in_file_name, int * addresses){
	FILE * irq2in_file = fopen(irq2in_file_name, "r");
	int line_number;
	char line[256];
	int i = 0;
	while (fscanf(irq2in_file, "%[^\n]\n", line) != EOF) {
		line_number = dec_string_to_int(line);
		addresses[i] = line_number;
		i++;
	}
	fclose(irq2in_file);
	return 0;
}
