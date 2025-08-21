
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

int irq2_load(FILE * irq2in_file, int * addresses){
	int line_number;
	char line[256];
	int i = 0;
	while (fscanf(irq2in_file, "%[^\n]\n", line) != EOF) {
		line_number = atoi(line);
		addresses[i] = line_number;
		i++;
	}
	return 0;
}
