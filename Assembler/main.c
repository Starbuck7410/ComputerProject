#include <stdio.h>
#include <string.h>
long find_instruction();
long find_register();

const int LABEL_COUNT = 250;

int pow_int(int a, int b){ // Do i really have to explain this...?
	if (b == 0){
		return 1;
	}
	if (a == 0){
		return 0;
	}
	int value = a;
	b--;
	while (b>0){
		value *= a;
		b--;
	}
	return value;
}

long long get_component(char * line, char * component, int start){ // get pointer to a line, extract 1st element and return new offset for the next component
	int i = start;

	while (* (line + i) == '$' || * (line + i) >= '0') {
		component[i - start] = * (line + i);
		i++;
	}

	component[i - start] = '\0'; // Terminate the string

	while (!(* (line + i) == '$' || * (line + i) >= '0')){ // Clear trailing whitespaces
		i++;
	}
	return i;
}

int dec_string_to_int(char number[]){ // Decimal string to int
	int value = 0;
	int i = 0;
	int sign = 1;
	if (number[0] == '-'){
		sign = -1;
		i++;
	}
	for (; i < strlen(number); i++){
		value += (number[i] - '0') * sign * pow_int(10, (strlen(number) - i - 1));
	}
	return value;
}


int hex_string_to_int(char number[]){ // You MUST pass the string WITHOUT the 0x
	int value = 0;
	int i = 0;
	int sign = 1;
	if (number[0] == '-'){ // fuck it let's allow negative hex numbers why not (0x-8000)
		sign = -1;
		i++;
	}
	for (; i < strlen(number); i++){
		if ('0' <= number[i] && number[i] <= '9'){
			value += (number[i] - '0') * sign * pow_int(16, (strlen(number) - i - 1));
		}
		if ('a' <= number[i] && number[i] <= 'f'){
			value += (number[i] - 'a' + 10) * sign * pow_int(16, (strlen(number) - i - 1));
		}
		if ('A' <= number[i] && number[i] <= 'F'){
			value += (number[i] - 'A' + 10) * sign * pow_int(16, (strlen(number) - i - 1));
		}
	}
	return value;
}


int main(int argc, char* argv[]) { // argv[1] = program.asm, argv[2] = imemin.txt, argv[3] = dmemin.txt
	FILE *asmb; //file pointer to program.asm
	FILE *mcode; //file pointer to imemin.txt 
	asmb = fopen(argv[1], "r");// only for read
	mcode = fopen("imemin.txt","w");//read and write
	char* line = NULL; 
	long len; 
	long long read; // in case the file is big 
	char labels [LABEL_COUNT][2]; // support for up to 250 labels
	
	// TODO initialize comes here
	// imemin.txt should get initialized to 000000000000\n * 4096
	// dmemin.txt should get initialized to 00000000\n * 4096


	// 1st pass to read labels
	while ((read = getline(&line, &len, asmb)) != -1) { 
	
	}
	rewind (asmb);


	// 2nd pass on the code:
	long long decoded_instruction; // 48 bits per instruction
    while ((read = getline(&line, &len, asmb)) != -1) { 
		
		// Cut out whitespaces
		int start = 0;
		while(line[start] == ' ' || line[start] == '	'){
			start++;
		}

		// Check the line isnt a comment
		if (line[start] == '#'){
			continue;
		}

		// Removes LF at the end of lines
		if (line[read-1] == '\n'){
		 	line[read-1] = '\0'; 
		}

		// Get 1st component (opcode)
		char op_code[10];
		start = get_component(line, op_code, start);
		printf("Instruction:   | %s\n", op_code);
		decoded_instruction = find_instruction(op_code) << 40; 

		long long decoded_reg;
		// get all 4 registers
		for (int i = 0; i<4; i++){
			char reg[10];
			start = get_component(line, reg, start);
			// decode reg into decoded register here
			decoded_reg = find_register(reg);
			decoded_instruction += decoded_reg << (24 + 4*(3-i));
			printf("got Register:  | %s (%d)\n", reg, decoded_reg);
		}

		// get both immediates
		for (int i = 0; i<2; i++){
			char imm[10];
			start = get_component(line, imm, start);
			int converted_imm = 0;

			if ('0' <= imm[0] && imm[0] <= '9'){
				converted_imm = dec_string_to_int(imm);
				decoded_instruction += converted_imm << (12*(1-i)); // turn immediate from string to number
			}
			
			printf("got immediate: | %s (%d)\n", imm, converted_imm);
		}

		printf("Final opcode:  | %012lx\n", decoded_instruction);
		fprintf(mcode, "%012lx\n", decoded_instruction); // Write to machine code file
	}
	fclose(asmb);
	fclose(mcode); 
	return 0;
}
