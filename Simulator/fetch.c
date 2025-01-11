#include <stdio.h>
#include <string.h>

// like in the assembler, but returns long long
long long pow_int(int a, int b) { 
	if (b == 0) {
		return 1;
	}
	if (a == 0) {
		return 0;
	}
	long long value = a;
	b--;
	while (b > 0) {
		value *= a;
		b--;
	}
	return value;
}

// like in the assembler, but returns long long
long long hex_string_to_long_long(char number[]) { // You MUST pass the string WITHOUT the 0x
	long long value = 0;
	int i = 0;
	int sign = 1;
	if (number[0] == '-') { // fuck it let's allow negative hex numbers why not (0x-8000)
		sign = -1;
		i++;
	}
	for (; i < strlen(number); i++) {
		if ('0' <= number[i] && number[i] <= '9') {
			value += (number[i] - '0') * sign * pow_int(16, (strlen(number) - i - 1));
		}
		if ('a' <= number[i] && number[i] <= 'f') {
			value += (number[i] - 'a' + 10) * sign * pow_int(16, (strlen(number) - i - 1));
		}
		if ('A' <= number[i] && number[i] <= 'F') {
			value += (number[i] - 'A' + 10) * sign * pow_int(16, (strlen(number) - i - 1));
		}
	}
	return value;
}

long long fetch(FILE* imemin_file, int PC) {
    int current_line = 0;
    char line[13];
	char line_without_null_terminator[12];
    // Read lines until the desired line is found
    while (fgets(line, PC, imemin_file) != NULL) {
        current_line++;
        if (current_line == PC) {
			strncpy(line_without_null_terminator, line, 12);
            return hex_string_to_long_long(line_without_null_terminator);  // Return the line as along long if it matches the PC
        }
    }

}