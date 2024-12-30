#include <stdio.h>
#include <string.h>
long find_instruction();
long find_register();

const int LABEL_COUNT = 250;

int pow_int(int a, int b){
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

long long get_component(char * line, char * op_code, int start){
	int i = 0;
	while (* (line + i) != ' ' && * (line + i) != '\0' && * (line + i) != '	'){
		op_code[i] = *(line +i);
		i++;
	}
	if (op_code[i-1] == ',' && i != 0){
		op_code[i-1] = '\0';
	}
	op_code[i] = '\0';
	return start + i + 1;
	

}

int string_to_int(char number[]){
	int value = 0;
	int i = 0;
	int sign = 1;
	if (number[0] == '-'){
		sign = -1;
		i++;
	}
	for (; i < strlen(number); i++){
		value += (number[i] - 48) * sign * pow_int(10, (strlen(number) - i - 1));
	}
	return value;
}


int main(int argc, char* argv[]) {
	FILE *asmb;//file pointer to  program.asm
	FILE *mcode;//file pointer to  imemin.txt 
	asmb = fopen(argv[1], "r");// only for read
	mcode = fopen("imemin.txt","w");//read and write
	char* line = NULL; 
	long len; 
	long long read; // in case the file is big 
	char labels [LABEL_COUNT][2]; // support for up to 250 labels

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
		start = get_component(line + start, op_code, start);
		printf("Instruction:   | %s\n", op_code);
		decoded_instruction = find_instruction(op_code) << 40; 

		long long decoded_reg;
		// get all 4 registers
		for (int i = 0; i<4; i++){
			char reg[10];
			start = get_component(line + start, reg, start);
			// decode reg into decoded register here
			decoded_reg = find_register(reg);
			decoded_instruction += decoded_reg << (24 + 4*(3-i));
			printf("got Register:  | %s (%d)\n", reg, decoded_reg);
		}

		// get both immediates
		for (int i = 0; i<2; i++){
			char imm[10];
			start = get_component(line + start, imm, start);
			int converted_imm = 0;

			if ('0' <= imm[0] && imm[0] <= '9'){
				converted_imm = string_to_int(imm);
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
