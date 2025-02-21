#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int eq_str(char str1[], char str2[]){
	if(str1 == NULL || str2 == NULL){
		return 0;
	}
    if (strlen(str1) != strlen(str2)){
        return 0;
    }
    for (int i = 0; i < strlen(str1); i++){
        if (str1[i] != str2[i]){
            return 0;
        }
    }
    return 1;
}

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

int dec_string_to_int(char number[]){ // Decimal string to int
	int len = strlen(number);
	int value = 0;
	int i = 0;
	int sign = 1;
	if (number[0] == '-'){
		sign = -1;
		i++;
	}
	for (; i < len; i++){
		value += (number[i] - '0') * sign * (int) pow_int(10, (len - i - 1));
	}
	return value;
}


// like in the assembler, but returns long long
long long hex_string_to_long_long(char number[], int len) 
{ // You MUST pass the string WITHOUT the 0x
	long long value = 0;
	int i = 0;
	int sign = 1;
	if (number[0] == '-') { // fuck it let's allow negative hex numbers why not (0x-8000)
		sign = -1;
		i++;
	}
	for (; i < len; i++) {
		if ('0' <= number[i] && number[i] <= '9') {
			value += (number[i] - '0') * sign * pow_int(16, (len - i - 1));
		}
		if ('a' <= number[i] && number[i] <= 'f') {
			value += (number[i] - 'a' + 10) * sign * pow_int(16, (len - i - 1));
		}
		if ('A' <= number[i] && number[i] <= 'F') {
			value += (number[i] - 'A' + 10) * sign * pow_int(16, (len - i - 1));
		}
	}
	return value;
}

int get_IO_reg_name(int inst_regs[], int registers[], char IOReg_name[]) {
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 0) strcpy(IOReg_name, "irq0enable");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 1) strcpy(IOReg_name, "irq1enable");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 2) strcpy(IOReg_name, "irq2enable");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 3) strcpy(IOReg_name, "irq0status");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 4) strcpy(IOReg_name, "irq1status");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 5) strcpy(IOReg_name, "irq2status");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 6) strcpy(IOReg_name, "irqhandler");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 7) strcpy(IOReg_name, "irqreturn");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 8) strcpy(IOReg_name, "clks");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 9) strcpy(IOReg_name, "leds");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 10) strcpy(IOReg_name, "display7seg");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 11) strcpy(IOReg_name, "timerenable");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 12) strcpy(IOReg_name, "timercurrent");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 13) strcpy(IOReg_name, "timermax");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 14) strcpy(IOReg_name, "diskcmd");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 15) strcpy(IOReg_name, "disksector");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 16) strcpy(IOReg_name, "diskbuffer");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 17) strcpy(IOReg_name, "diskstatus");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 20) strcpy(IOReg_name, "monitoraddr");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 21) strcpy(IOReg_name, "monitordata");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 22) strcpy(IOReg_name, "monitorcmd");
	return 0;
}

void error(char * text){
	printf("\x1B[31m");
	printf("%s", text);
	printf("\x1B[0m");
}

void warn(char * text){
	printf("\e[0;33m");
	printf("%s", text);
	printf("\x1B[0m");
}

void print_help(){
    printf("NASS Simulator:\n");
	printf("Nameless asssembler & simulator for SIMP (Name pending approval)\n");
    printf("Usage: ./simulate <imemin.txt> <dmemin.txt> <diskin.txt> <irq2in.txt> <dmemout.txt> <regout.txt> <trace.txt> <hwregtrace.txt> <cycles.txt> <leds.txt> <display7seg.txt> <diskout.txt> <monitor.txt> <monitor.yuv>\n");
    printf("The program will run an assembled program and output the results to the files specified.\n\n");
    printf("NASS is free software using by the GPL V3 license.\n");
	printf("Copyright Shraga, Zohar, Daniel and Raz\n");
}

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