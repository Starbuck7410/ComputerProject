#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
long long hex_string_to_long_long(char number[], int len);

int fill_llarray_from_file(int mem[], char* dmemin_file_path) {
	int i = 0;
	char data[9];
	FILE* dmem; //file pointer to dmemin.txt
	dmem = fopen(dmemin_file_path, "r"); //read and write
	while (fscanf(dmem, "%[^\n]\n", data) != EOF && i < 4096) { //read a line from dmem and put it in 'data'
		mem[i] = (int) (hex_string_to_long_long(data, 8) & 0xFFFFFFFF); //store it in memory as hex
		i++;
	}
	fclose(dmem);
	dmem = NULL;
	return 0;
}

int dmemout(int * local_mem, char * arg5) {
	FILE* dmemout;
	dmemout = fopen(arg5, "w");
	if(dmemout == NULL){
		perror("Failed to open file");
		return 1;
	}
	for (int i = 0; i < 4096; i++) {
		fprintf(dmemout, "%08lX\n", local_mem[i]);
	}
	fclose(dmemout);
	dmemout = NULL;
	return 0;
}



