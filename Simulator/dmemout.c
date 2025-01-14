#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int dmemout(long long local_mem[], char* arg5) {
	FILE* dmem;
	dmem = fopen(arg5, "w");
	
	for (int i = 0; i < 4096; i++) {
		fprintf(dmem, "%08lX\n", local_mem[i]);
		}
	fclose(dmem);
	return 0;
}