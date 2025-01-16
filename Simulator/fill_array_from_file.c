#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
long long hex_string_to_long_long();
long long pow_int();

int fill_int_array_from_file(int mem[], char* filename) {
	int i = 0;
	char data[8 + 1];
	FILE* file; //file pointer to filein.txt
	file = fopen(filename, "r"); //read and write
	while (fscanf(file, "%[^\n]\n", data) != EOF && i < 4096) { //read a line from file and put it in 'data'
		mem[i] = (int) (hex_string_to_long_long(data, 8) & 0xFFFFFFFF); //store it in memory as hex
		i++;
	}
	fclose(file);
	file = NULL;
	return 0;
}

int fill_long_array_from_file(long long mem[], char* filename) {
	int i = 0;
	char data[12 + 1];
	FILE* file; //file pointer to filein.txt
	file = fopen(filename, "r"); //read and write
	while (fscanf(file, "%[^\n]\n", data) != EOF && i < 4096) { //read a line from file and put it in 'data'
		mem[i] = (hex_string_to_long_long(data, 12) & 0xFFFFFFFFFFFF); //store it in memory as hex
		i++;
	}
	fclose(file);
	file = NULL;
	return 0;
}