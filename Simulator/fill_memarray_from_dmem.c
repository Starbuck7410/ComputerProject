#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>


int fill_memarray_from_dmem(long long mem[], char* arg2) {
	int i = 0;
	char data[8];
	FILE* dmem; //file pointer to dmemin.txt
	dmem = fopen(arg2, "r"); //read and write
	while (fscanf(dmem, "%8[^\n]\n", data) != EOF) { //read a line from dmem and put it in 'data'
		mem[i] = hex_string_to_long_long(data); //store it in memory as hex
		i++;
		if (i == 4096) break; 
	}
	fclose(dmem);
	return 0;
	}
