#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int dmemout(int local_mem[], char * arg5) {
	FILE* dmemout;
	dmemout = fopen(arg5, "w");
	for (int i = 0; i < 4096; i++) {
		fprintf(dmemout, "%08lX\n", local_mem[i]);
	}
	fclose(dmemout);
	dmemout = NULL;
	return 0;
}