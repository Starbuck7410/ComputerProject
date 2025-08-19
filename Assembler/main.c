#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"
#include "mem.h"
#define LABEL_COUNT 300
#define LABEL_SIZE 70
#define LINE_SIZE 300


int main(int argc, char* argv[]) { // argv[1] = program.asm, argv[2] = imemin.txt, argv[3] = dmemin.txt
	if(argc <= 1){
		error("No arguments given. Use the -h flag for more info.\n");
		return 1;
	}
	// -h flag handling:
	if (eq_str(argv[1], "-h")){ 
		print_help();
		return 0;
	}
	if (argc != 5){
		error("Wrong number of arguments. Use the -h flag for more info.\n");
		return 1;
	}

	FILE * asmb_file; //file pointer to program.asm
	FILE * mem_file; //file pointer to memin.txt
	FILE * irq2in_file; //file pointer to irq2in.txt
	FILE * disk_in_file; //file pointer to diskin.txt
	
	asmb_file = fopen(argv[1], "r"); // only for read
	if (asmb_file == NULL){
		error("Failed to read assembly file: ");
		printf("%s\n", argv[1]);
		perror("");
		return 1;	
	}

	mem_file = fopen(argv[2],"w");	//read and write
	if (asmb_file == NULL){
		error("Failed to create mem file: ");
		printf("%s\n", argv[2]);
		perror("");
		return 1;	
	}

	
	irq2in_file = fopen(argv[3], "w");
	if (irq2in_file == NULL){
		error("Failed to create irq2in file: ");
		printf("%s\n", argv[3]);
		perror("");
		return 1;	
	}


	disk_in_file = fopen(argv[4], "w");
	if (disk_in_file == NULL){
		error("Failed to create diskin file: ");
		printf("%s\n", argv[4]);
		perror("");
		return 1;	
	}


	mem_t disk;
	mem_init(&disk);



	char line [LINE_SIZE]; 
	char temp_line[LINE_SIZE];
	strncpy(line, "", 1);
	strncpy(temp_line, "", 1);
	char labels [LABEL_COUNT][LABEL_SIZE]; // support for up to LABEL_COUNT labels
	int label_addresses [LABEL_COUNT];


	mem_t mem;
	mem_init(&mem);
	     

	// 				------------------------------------- 1st pass on the code: ----------------------------------
	int address = 0;
	int label_index = 0;
	int line_index = 0;
	int elements;
	while (elements = fscanf(asmb_file, "%[^\n]\n", temp_line) != EOF) { 
		line_index++;
		if(temp_line[0] == '\0'){
			fgetc(asmb_file);
			continue;
		}
		strncpy(line, temp_line, LINE_SIZE);
		strncpy(temp_line, "", 1);
		// Cut out whitespaces
		char label [LABEL_SIZE];
		int start = clean_string(line);
		

		// Check the line isnt a comment
		if (line[start] == '#' || line[start] == '.'){
			continue;
		}
		get_component(line, label, start);

		if(label[strlen(label) - 1] == ':'){ // if it ends with a ':', its a label
			label[strlen(label) - 1] = '\0';
			// printf("Found label:   | %s\n", label);
			// printf("in line:       | %d\n", line_index);

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
	int64_t decoded_instruction; // 64 bits per instruction
	int64_t converted_instruction;
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

		// Check the line isnt a comment
		if (line[start] == '#'){
			continue;
		}
		
		// Get 1st component (opcode)
		char op_code[10];
		start = get_component(line, op_code, start);


		// Handle .word directives
		if (eq_str(op_code, ".word")){
			int int_word_address, int_word_value;									
			// printf("Directive:     | \"%s\"\n", op_code);
			char word_address[15], word_value[15];									

			start = get_component(line, word_address, start);
			start = get_component(line, word_value, start);
			
			
			int_word_address = str_to_int(word_address);
			int_word_value = str_to_int(word_value);							
		               
			mem_write_idx(&mem, int_word_value, int_word_address);
			continue;
		}
		
		if(eq_str(op_code, ".interrupt")){
			// printf("Directive:     | \"%s\"\n", op_code);
			char interrupt_text[10];
			start = get_component(line, interrupt_text, start);		
			int interrupt_value = str_to_int(interrupt_text);
			printf("Interrupt:     | %d\n", interrupt_value);
			fprintf(irq2in_file, "%d\n", interrupt_value);
			continue;
		}


		if(eq_str(op_code, ".diskpage")){
		
			// printf("Directive:     | \"%s\"\n", op_code);
			char disk_sector_text[10];
			start = get_component(line, disk_sector_text, start);
			
			size_t disk_sector_value = str_to_int(disk_sector_text);
			
			char disk_page_text[2];
			start = get_component(line, disk_page_text, start);
			size_t disk_page = str_to_int(disk_page_text);
			
			// printf("Sector, page:  | %d, %d\n", disk_sector_value, disk_page);

			char word_text[10];
			int32_t word_value;
			for (int i = 0; i < 4; i++){ // 1 page is 4 words
				start = get_component(line, word_text, start);
				word_value = str_to_int(word_text);
				// printf("Word %02d:       | %d\n", i, word_value);
				mem_write_idx(&disk, word_value, disk_sector_value * 16 + disk_page * 4 + i);
			}
			

			continue;
		}

		converted_instruction = find_instruction(op_code);
		if (op_code[strlen(op_code) - 1] == ':' || eq_str(op_code, "")){ // Check if it's not a label
			continue;
		}
		if(converted_instruction == -1){
			printf("\x1B[31mERROR: UNDEFINED INSTRUCTION \"%s\" FOUND AT LINE: %d\x1B[0m\n", op_code, line_index); // We didnt recognize the instruction
			return 1;
		}
		
		decoded_instruction =  (converted_instruction & 0xFF) << 56; 

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
			decoded_instruction += (decoded_reg & 0xF) << (40 + 4*(3-i));
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
						// printf("\x1B[32mReplacing label \"%s\" with address: %d\x1B[0m\n",labels[i] , label_addresses[i]);
						converted_imm = label_addresses[i];
						break;
					}
					if (i == LABEL_COUNT - 1){
						printf("\x1B[31mERROR: LABEL \"%s\" HAS NO ASSOCIATED ADDRESS\x1B[0m\n", imm);
						return 1;
					}
				}	
			}
				decoded_instruction += (converted_imm & 0xFFFFF) << (20*(1-i)); // turn immediate from string to number
		}

		// printf("Final opcode:  | %012llX\n", decoded_instruction);
		mem_write_idx(&mem, (int32_t) (decoded_instruction >> 32), 2 * address);
		mem_write_idx(&mem, (int32_t) decoded_instruction, 2 * address + 1);
		address++;
		
	}

	mem_write_file(&mem, mem_file);
	mem_write_file(&disk, disk_in_file);

	free(mem.data);
	free(disk.data);
	fclose(disk_in_file);
	fclose(irq2in_file);
	fclose(asmb_file);
	fclose(mem_file); 
	return 0;
}
