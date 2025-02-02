#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"
#define LABEL_COUNT 250
#define LABEL_SIZE 50
#define LINE_SIZE 250


int main(int argc, char* argv[]) { // argv[1] = program.asm, argv[2] = imemin.txt, argv[3] = dmemin.txt
	if(argc <= 1){
		error("No arguments given. Use the -h flag for more info.\n");
		return 1;
	}
	// // -h flag handling:
	if (eq_str(argv[1], "-h") || eq_str(argv[1], "-H")){ 
		print_help();
		return 0;
	}
	if (argc < 3){
		error("Not enough arguments. Use the -h flag for more info.\n");
		return 1;
	}

	FILE *asmb_file; //file pointer to program.asm
	FILE *imem_file; //file pointer to imemin.txt
	FILE *dmem_file; //file pointer to dmemin.txt 
	asmb_file = fopen(argv[1], "r");// only for read
	if (asmb_file == NULL){
		error("Failed to create assembly file: ");
		printf("%s\n", argv[1]);
		perror("");
		return 1;	
	}

	imem_file = fopen(argv[2],"w");	//read and write
	if (asmb_file == NULL){
		error("Failed to create imem file: ");
		printf("%s\n", argv[1]);
		perror("");
		return 1;	
	}

	dmem_file = fopen(argv[3], "w");
	if (asmb_file == NULL){
		error("Failed to create dmem file: ");
		printf("%s\n", argv[1]);
		perror("");
		return 1;	
	}

	FILE* disk_in_file;
	FILE* irq2in_file;
	int disk_size;
	int *disk;
	if(argc > 4){
		disk_in_file = fopen(argv[4], "w");
		irq2in_file = fopen(argv[5], "w");

		if (disk_in_file == NULL){
			error("Failed to create diskin file: ");
			printf("%s\n", argv[1]);
			perror("");
			return 1;	
		}
		if (irq2in_file == NULL){
			error("Failed to create irq2in file: ");
			printf("%s\n", argv[1]);
			perror("");
			return 1;	
		}
		disk = (int*) malloc(512 * 128); // 512 sectors of 128 bytes
	}


	char line [LINE_SIZE]; 
	char temp_line[LINE_SIZE];
	strncpy(line, "", 1);
	strncpy(temp_line, "", 1);
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
	int elements;
	while (elements = fscanf(asmb_file, "%[^\n]\n", temp_line) != EOF) { 
		line_index++;
		if(temp_line[0] == '\0'){
			printf("Skipping empty line\n");
			fgetc(asmb_file);
			continue;
		}
		strncpy(line, temp_line, LINE_SIZE);
		strncpy(temp_line, "", 1);
		// Cut out whitespaces
		char label [LABEL_SIZE];
		int start = clean_string(line);
		

		// Check the line isnt a comment
		if (line[start] == '#'){
			continue;
		}
		get_component(line, label, start);

		if(label[strlen(label) - 1] == ':'){ // if it ends with a ':', its a label
			label[strlen(label) - 1] = '\0';
			printf("Found label:   | %s\n", label);
			printf("in line:       | %d\n", line_index);

			// Check if we haven't seen this label before
			for(int i = 0; i < LABEL_COUNT; i++){
				if (eq_str(labels[i], label)){
					error("ERROR: DUPLICATE LABELS DETECTED IN LINE: ");
					printf("%d\n", line_index);
					return 1;
				}
			}
			
			// Store the found label

			strncpy(labels[label_index], label, LABEL_SIZE); 
			
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
    while (elements = fscanf(asmb_file, "%[^\n]\n", temp_line) != EOF) { 
		line_index++;
		if(temp_line[0] == '\0'){
			fgetc(asmb_file);
			continue;
		}
		strncpy(line, temp_line, LINE_SIZE);
		strncpy(temp_line, "", 1);
		// Cut out whitespaces
		int start = clean_string(line);

		printf("Line:          | %s\n", line);
		// Check the line isnt a comment
		if (line[start] == '#' && line[start + 1] != '.'){
			printf("Skipping comment\n");
			continue;
		}
		
		// Get 1st component (opcode)
		char op_code[10];
		start = get_component(line, op_code, start);


		// Handle .word instructions
		if (eq_str(op_code, ".word")){
			int int_word_address, int_word_value;									//define int of address, int of data, array such that x[address] = data and assist vars
			printf("Directive:     | \"%s\"\n", op_code);
			char word_address[15], word_value[15];									//define string of address and string of data

			start = get_component(line, word_address, start);
			start = get_component(line, word_value, start);
			
			
			int_word_address = str_to_int(word_address);						//convert string to int
			int_word_value = str_to_int(word_value);							//convert string to int
		               
			dmem[int_word_address] = int_word_value;                                	 //dmem[address] = data
			printf("Word address:  | %d\n", int_word_address);
			printf("Word Value:    | %d\n", int_word_value);
			continue;
		}
		
		if(eq_str(op_code, "#.interrupt")){
			if(argc > 4){
				printf("Directive:     | \"%s\"\n", op_code);
				char interrupt_text[10];
				start = get_component(line, interrupt_text, start);
				
				int interrupt_value = str_to_int(interrupt_text);
				printf("Interrupt:     | %d\n", interrupt_value);
				fprintf(irq2in_file, "%d\n", interrupt_value);
			} else {
				warn("WARNING: #.interrupt used without extra features mode on.\nFor more info run with the flag -h.\n");
			}
			continue;
		}

		if(eq_str(op_code, "#.disksector")){
			if(argc > 4){
				printf("Directive:     | \"%s\"\n", op_code);
				char disk_sector_text[10];
				start = get_component(line, disk_sector_text, start);
				
				int disk_sector_value = str_to_int(disk_sector_text);
				if (disk_sector_value * 16 >= disk_size){
					for (int i = disk_size; i < disk_sector_value * 16; i++){
						disk[i] = 0;
					}
					disk_size = (disk_sector_value + 1) * 16;
					disk[disk_size + 1] = EOF;	
				}	

				printf("Disk Sector:   | %d\n", disk_sector_value);

				char word_text[10];
				int word_value;
				for (int i = 0; i < 16; i++){ // 1 sector is 16 words
					start = get_component(line, word_text, start);
					word_value = str_to_int(word_text);
					// printf("Word %02d:       | %d\n", i, word_value);
					disk[disk_sector_value * 16 + i] = word_value;
				}
			} else {
				warn("WARNING: #.disksector used without extra features mode on.\nFor more info run with the flag -h.\n");
			}

			continue;
		}

		converted_instruction = find_instruction(op_code);
		if (op_code[strlen(op_code) - 1] == ':' || eq_str(op_code, "")){ // Check if it's not a label
			continue;
		}
		if(!eq_str(op_code, "#.disksector") && !eq_str(op_code, "#.interrupt") && converted_instruction == -1){
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
			decoded_instruction += (decoded_reg & 0xF) << (24 + 4*(3-i));
		}

		// get both immediates
		for (int i = 0; i<2; i++){
			char imm[10];
			start = get_component(line, imm, start);
			int converted_imm = 0;

			if ('0' <= imm[0] && imm[0] <= '9' || imm[0] == '-'){
				converted_imm = str_to_int(imm);
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
		}

		printf("Final opcode:  | %012llX\n", decoded_instruction);
		imem[address] = decoded_instruction;
		address++;
		
	}
	// filling dmemin and imemin

	for (int i = 0; i < 4096; i++){
		fprintf(dmem_file, "%08lX\n", dmem[i]);
	}

	for (int i = 0; i < 4096; i++){
		fprintf(imem_file, "%012llX\n", imem[i]);
	}


	if(argc > 4){
		for(int i = 0; i < disk_size; i++){
			fprintf(disk_in_file, "%08X\n", disk[i]);
		}
	}

	if(argc > 4){
		fclose(disk_in_file);
		fclose(irq2in_file);
	}

	free(disk);
	fclose(asmb_file);
	fclose(imem_file); 
	fclose(dmem_file);
	return 0;
}
