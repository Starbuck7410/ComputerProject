#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define LINE_LEN 14
#endif

#ifdef __GNUC__
#define LINE_LEN 13
#endif
long long pow_int(int a, int b);
long long hex_string_to_long_long(char number[], int len);

long long fetch(FILE* imemin_file, int PC) 
{
	// printf("PC = %d, char %d\n", PC, PC * 14);
	char line[13];
	fseek(imemin_file, PC * LINE_LEN, SEEK_SET);
	if (fgets(line, 13, imemin_file) != NULL) {
		// printf("%s\n", line);
		return hex_string_to_long_long(line, 12);  
	}
	return 0;
	// Return the line as a long long if it matches the PC
}