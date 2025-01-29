#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
void trace_out(FILE* trace_file, int PC, long long inst, int registers[])
{
	fprintf(trace_file, "%03X ", PC);
	fprintf(trace_file, "%012llX ", inst);
	for (int i = 0; i < 15; i++)
	{
		fprintf(trace_file, "%08x ", registers[i]);
	}
	fprintf(trace_file, "%08x\n", registers[15]);
}