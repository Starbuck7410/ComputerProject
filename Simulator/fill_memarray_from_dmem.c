#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
long long hex_string_to_long_long();

int fill_memarray_from_dmem(int mem[], char* arg2) {
	int i = 0;
	char data[9];
	FILE* dmem; //file pointer to dmemin.txt
	dmem = fopen(arg2, "r"); //read and write
	while (fscanf(dmem, "%[^\n]\n", data) != EOF && i < 4096) { //read a line from dmem and put it in 'data'
		mem[i] = (int) (hex_string_to_long_long(data, 8) & 0xFFFFFFFF); //store it in memory as hex
		i++;
	}
	fclose(dmem);
	dmem = NULL;
	return 0;
}
