#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int monitor_out(char* arg13, char* arg14, unsigned char monitor[]) {
	FILE* monitor_txt = fopen(arg13, "w");
	
	if (!monitor_txt) {
		perror("Error opening .txt file");
		return 1;
	}

	for (int i = 0; i < 256 * 256; i++) {
		fprintf(monitor_txt, "%02X\n", monitor[i]);
	}
	fclose(monitor_txt);
	monitor_txt = NULL;

	FILE* monitor_yuv = fopen(arg14, "wb");
	
	if (!monitor_yuv) {
		perror("Error opening .yuv file");
		return 1;
	}

	fwrite(monitor, sizeof(unsigned char) , 256 * 256, monitor_yuv);
	fclose(monitor_yuv);
	monitor_yuv = NULL;

	return 0;
}