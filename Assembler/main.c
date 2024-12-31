#include <stdio.h>
#include <string.h>
long find_instruction();
long find_register();
int eq_str();

const int LABEL_COUNT = 250;
const int LABEL_SIZE = 50;

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

	while (* (line + i) == '$' || * (line + i) >= '0' ) {
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
	char labels [LABEL_COUNT][LABEL_SIZE]; // support for up to 250 labels
	int label_addresses [LABEL_COUNT];
	// TODO initialize comes here
	// imemin.txt should get initialized to 000000000000\n * 4096
	// dmemin.txt should get initialized to 00000000\n * 4096


	// 1st pass to read labels
	int address = 0;
	int label_index = 0;
	int line_index = 0;
	while ((read = getline(&line, &len, asmb)) != -1) { 
		line_index++;
		// Cut out whitespaces
		char label [LABEL_SIZE];
		int start = 0;
		while(line[start] == ' ' || line[start] == '	'){
			start++;
		}

		// Check the line isnt a comment
		if (line[start] == '#'){
			continue;
		}


		get_component(line, label, start);
		if(label[strlen(label) - 1] == ':'){ // if it ends with a ':', its a label
			label[strlen(label) - 1] = '\0';
			printf("Found label:   | %s\n", label);
			printf("in line:       | %d\n", address);

			// Check if we haven't seen this labal before
			for(int i = 0; i < LABEL_COUNT; i++){
				if (eq_str(labels[i],label)){
					printf("\x1B[31mERROR: DUPLICATE LABELS DETECTED IN LINE: %d\x1B[0m\n", line_index);
					return 1;
				}
			}
			
			// Store the found label
			for (int i = 0; i < LABEL_SIZE; i++){
				labels[label_index][i] = label[i]; // because apparently regular assignments dont work?
			} 
			label_addresses[label_index] = address;

		}
		address++;
	}
	rewind (asmb);
	// return 0;

	// 2nd pass on the code:
	long long decoded_instruction; // 48 bits per instruction
	long long converted_instruction;
	line_index = 0;
    while ((read = getline(&line, &len, asmb)) != -1) { 
		line_index++;
		// Cut out whitespaces
		int start = 0;
		while(line[start] == ' ' || line[start] == '	'){
			start++;
		}

		// Check the line isnt a comment
		if (line[start] == '#'){
			continue;
		}

		// Get 1st component (opcode)
		char op_code[10];
		start = get_component(line, op_code, start);

		if (op_code[strlen(op_code) - 1] == ':' || eq_str(op_code, "")){ // Check if it's not a label
			continue;
		}

		printf("Instruction:   | \"%s\"\n", op_code);
		converted_instruction = find_instruction(op_code);
		if (converted_instruction == -1){
			printf("\x1B[31mERROR: UNDEFINED INSTRUCTION \"%s\" FOUND AT LINE: %d\x1B[0m\n", op_code, line_index); // We didnt recognize the instruction
			return 1;
		}
		decoded_instruction =  converted_instruction << 40; 

		long long decoded_reg;
		// get all 4 registers
		for (int i = 0; i<4; i++){
			char reg[10];
			start = get_component(line, reg, start);
			// decode reg into decoded register here
			decoded_reg = find_register(reg);
			if (decoded_reg == -1){
				printf("\x1B[31mERROR: UNKNOWN REGISTER \"%s\" AT LINE %d\x1B[0m\n", reg, line_index); // We didnt recognize the register
				return 1;
			}
			printf("got Register:  | %s (%d)\n", reg, decoded_reg);
			decoded_instruction += decoded_reg << (24 + 4*(3-i));
		}

		// get both immediates
		for (int i = 0; i<2; i++){
			char imm[10];
			start = get_component(line, imm, start);
			int converted_imm = 0;

			if ('0' <= imm[0] && imm[0] <= '9'){
				converted_imm = dec_string_to_int(imm);
			}else{
				for(int i = 0; i < LABEL_COUNT; i++){
					if (eq_str(labels[i], imm)){
						printf("\x1B[32mReplacing label \"%s\" with address: %d\x1B[0m\n",labels[i] , label_addresses[i]);
						converted_imm =label_addresses[i];
						break;
					}
					if (i == LABEL_COUNT - 1){
						printf("\x1B[31mERROR: LABEL \"%s\" HAS NO ASSOCIATED ADDRESS\x1B[0m\n", imm);
						return 1;
					}
				}	
			}
				decoded_instruction += converted_imm << (12*(1-i)); // turn immediate from string to number
			
			printf("got immediate: | %s (%d)\n", imm, converted_imm);
		}

		printf("Final opcode:  | %012lx\n", decoded_instruction);
		fprintf(mcode, "%012lx\n", decoded_instruction); // Write to machine code file
		
	}
	fclose(asmb);
	fclose(mcode); 
	return 0;
}
