#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int monitor_out(char* arg13, char* arg14, unsigned char monitor[]) {
	FILE* monitxt;
	monitxt = fopen(arg13, "w");

	if (!monitxt) {
		perror("Error opening .txt file");
		return 1;
	}

	for (int i = 0; i < 256 * 256; i++) {
		fprintf(monitxt, "%02X\n", monitor[i]);
	}
	fclose(monitxt);
	monitxt = NULL;


	FILE* monityuv;
	monityuv = fopen(arg14, "wb");
	if (!monityuv) {
		perror("Error opening .yuv file");
		return 1;
	}

	for (int i = 0; i < 256 * 256; i++) {
		fwrite(monitor, sizeof(unsigned char), 1, monityuv);
	}
	fclose(monityuv);
	monityuv = NULL;

	return 0;
}