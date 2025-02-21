#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
long long hex_string_to_long_long(char number[], int len);

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



