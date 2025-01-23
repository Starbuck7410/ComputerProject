#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#define LABEL_COUNT 250
#define LABEL_SIZE 50
#define LINE_SIZE 250


long long find_instruction();
long long find_register();
long long pow_int(int a, int b);
int eq_str(char str1[], char str2[]);
int dec_string_to_int();
int hex_string_to_int();
int text_to_int(char * text);




long long get_component(char * line, char * component, int start){ // get pointer to a line, extract 1st element and return new offset for the next component
	int i = start;

	while (* (line + i) == '-' || * (line + i) == '.' || * (line + i) == '$' || * (line + i) >= '0' ) {
		component[i - start] = * (line + i);
		i++;
	}

	component[i - start] = '\0'; // Terminate the string

	while (!(* (line + i) == '-' || * (line + i) == '.' || * (line + i) == '$' || * (line + i) >= '0')){ // Clear trailing whitespaces
		i++;
	}
	return i;
}




int main(int argc, char* argv[]) { // argv[1] = program.asm, argv[2] = imemin.txt, argv[3] = dmemin.txt
	FILE *asmb_file; //file pointer to program.asm
	FILE *imem_file;//file pointer to imemin.txt
	FILE *dmem_file;//file pointer to dmemin.txt 
	if (argc < 3){
		printf("\x1B[31mNot enough arguments\x1B[0m");
		return 1;
	}
	asmb_file = fopen(argv[1], "r");// only for read
	if (asmb_file == NULL){
		printf("\x1B[31mFailed to open assembly file: %s\n", argv[1]);
		perror("");
		printf("\x1B[0m");
		return 1;	
	}

	imem_file = fopen(argv[2],"w");	//read and write
	if (asmb_file == NULL){
		printf("\x1B[31mFailed to create imem file: %s\n", argv[1]);
		perror("");
		printf("\x1B[0m");
		return 1;	
	}

	dmem_file = fopen(argv[3], "w");
	if (asmb_file == NULL){
		printf("\x1B[31mFailed to create dmem file: %s\n", argv[1]);
		perror("");
		printf("\x1B[0m");
		return 1;	
	}

	// TODO - Shraga dont just leave this here
	FILE* disk_in_file;
	FILE* irq2in_file;
	int disk_size;
	if(argc > 4){
		disk_in_file = fopen(argv[4], "w");
		irq2in_file = fopen(argv[5], "w");

		if (disk_in_file == NULL){
			printf("\x1B[31mFailed to create diskin file: %s\n", argv[1]);
			perror("");
			printf("\x1B[0m");
			return 1;	
		}
		if (irq2in_file == NULL){
			printf("\x1B[31mFailed to create irq2in file: %s\n", argv[1]);
			perror("");
			printf("\x1B[0m");
			return 1;	
		}
	}


	char line [LINE_SIZE]; 
	char labels [LABEL_COUNT][LABEL_SIZE]; // support for up to 250 labels
	int label_addresses [LABEL_COUNT];


	if(asmb_file == NULL || imem_file == NULL || dmem_file == NULL){
		
	}

	int dmem[4096];
	long long imem[4096];
	for (int i = 0; i < 4096; i++){
		dmem[i] = 0;	// dmemin.txt gets initialized to 00000000\n * 4096
		imem[i] = 0;	// imemin.txt gets initialized to 000000000000\n * 4096
	}       


	
	


	// 				------------------------------------- 1st pass on the code: ----------------------------------
	int address = 0;
	int label_index = 0;
	int line_index = 0;

	while (fscanf(asmb_file, "%[^\n]\n", line) != EOF) { 
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

			// Check if we haven't seen this label before
			for(int i = 0; i < LABEL_COUNT; i++){
				if (eq_str(labels[i], label)){
					printf("\x1B[31mERROR: DUPLICATE LABELS DETECTED IN LINE: %d\x1B[0m\n", line_index);
					return 1;
				}
			}
			
			// Store the found label
			for (int i = 0; i < LABEL_SIZE; i++){
				labels[label_index][i] = label[i]; // because apparently regular assignments dont work?
			} 
			label_addresses[label_index] = address;
			label_index++;
			continue;
		}
		address++;
	}
	rewind(asmb_file);
	// 				------------------------------------- 2nd pass on the code: ----------------------------------

	long long decoded_instruction; // 48 bits per instruction
	long long converted_instruction;
	address = 0;
	line_index = 0;
    while (fscanf(asmb_file, "%[^\n]\n", line) != EOF) { 
		line_index++;
		// Cut out whitespaces
		int start = 0;

		while(line[start] == ' ' || line[start] == '	'){
			start++;
		}
		
		printf("Line:          | %s\n", line);
		// Check the line isnt a comment
		if (line[start] == '#'){
			continue;
		}

		// Get 1st component (opcode)
		char op_code[10];
		start = get_component(line, op_code, start);

		// Handle .word instructions
		if (eq_str(op_code, ".word")){
			int int_word_address, int_word_value;									//define int of address, int of data, array such that x[address] = data and assist vars
			printf("Instruction:   | \"%s\"\n", op_code);
			char word_address[15], word_value[15];									//define string of address and string of data

			start = get_component(line, word_address, start);
			start = get_component(line, word_value, start);
			
			int_word_address = text_to_int(word_address);						//convert string to int
			int_word_value = text_to_int(word_value);							//convert string to int
		               
			dmem[int_word_address] = int_word_value;                                	 //dmem[address] = data
			printf("Word address:  | %d\n", int_word_address);
			printf("Word Value:    | %d\n", int_word_value);
			continue;
		}
		
		// TODO - Shraga you know you want to finisht this feature
		if(eq_str(op_code, ".interrupt")){
			printf("Instruction:   | \"%s\"\n", op_code);
			char interrupt_text[10];
			start = get_component(line, interrupt_text, start);
			int interrupt_value = text_to_int(interrupt_text);
			printf("Interrupt:     | %d\n", interrupt_value);
			fprintf(irq2in_file, "%d\n", interrupt_value);
			continue;
		}

		if(eq_str(op_code, ".disksector")){
			printf("Instruction:   | \"%s\"\n", op_code);
			char disk_sector_text[10];
			start = get_component(line, disk_sector_text, start);
			int disk_sector_value = text_to_int(disk_sector_text);	

			printf("Disk Sector:   | %d\n", disk_sector_value);

			char word_text[10];
			int word_value;
			for (int i = 0; i < 16; i++){ // 1 sector is 16 words
				start = get_component(line, word_text, start);
				word_value = text_to_int(word_text);
				printf("Word %02d:       | %d\n", i, word_value);
			}
			continue;
		}



		if (op_code[strlen(op_code) - 1] == ':' || eq_str(op_code, "")){ // Check if it's not a label
			continue;
		}

		printf("Instruction:   | \"%s\"\n", op_code);
		converted_instruction = find_instruction(op_code);
		if (converted_instruction == -1){
			printf("\x1B[31mERROR: UNDEFINED INSTRUCTION \"%s\" FOUND AT LINE: %d\x1B[0m\n", op_code, line_index); // We didnt recognize the instruction
			return 1;
		}
		decoded_instruction =  (converted_instruction & 0xFF) << 40; 

		long long decoded_reg;
		// get all 4 registers
		for (int i = 0; i<4; i++){
			char reg[10];
			start = get_component(line, reg, start);
			// decode reg into decoded register here
			decoded_reg = find_register(reg);
			if (decoded_reg == 4294967295){
				printf("\x1B[31mERROR: UNKNOWN REGISTER \"%s\" AT LINE %d\x1B[0m\n", reg, line_index); // We didnt recognize the register
				return 1;
			}
			printf("got Register:  | %s (%lld)\n", reg, decoded_reg);
			decoded_instruction += (decoded_reg & 0xF) << (24 + 4*(3-i));
		}

		// get both immediates
		for (int i = 0; i<2; i++){
			char imm[10];
			start = get_component(line, imm, start);
			int converted_imm = 0;

			if ('0' <= imm[0] && imm[0] <= '9' || imm[0] == '-'){
				converted_imm = dec_string_to_int(imm);
			}else{
				for(int i = 0; i < LABEL_COUNT; i++){
					if (eq_str(labels[i], imm)){
						printf("\x1B[32mReplacing label \"%s\" with address: %d\x1B[0m\n",labels[i] , label_addresses[i]);
						converted_imm = label_addresses[i];
						break;
					}
					if (i == LABEL_COUNT - 1){
						printf("\x1B[31mERROR: LABEL \"%s\" HAS NO ASSOCIATED ADDRESS\x1B[0m\n", imm);
						return 1;
					}
				}	
			}
				decoded_instruction += (converted_imm & 0xFFF) << (12*(1-i)); // turn immediate from string to number
			
			printf("got immediate: | %s (%d)\n", imm, converted_imm);
		}

		printf("Final opcode:  | %012llX\n", decoded_instruction);
		imem[address] = decoded_instruction;
		address++;
		// fprintf(imem_file, "%012llx\n", decoded_instruction); // Write to machine code file
		
	}
	// filling dmemin and imemin


	for (int i = 0; i < 4096; i++){
		fprintf(dmem_file, "%08lX\n", dmem[i]);
	}

	for (int i = 0; i < 4096; i++){
		fprintf(imem_file, "%012llX\n", imem[i]);
	}





	fclose(asmb_file);
	fclose(imem_file); 
	fclose(dmem_file);
	return 0;
}
