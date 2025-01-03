#include <stdio.h>
#include <string.h>
long find_instruction();
long find_register();
int eq_str();
int pow_int();
int dec_string_to_int();
int hex_string_to_int();
void dmemin_fill();


const int LABEL_COUNT = 250;
const int LABEL_SIZE = 50;
const int LINE_SIZE = 250;



long long get_component(char * line, char * component, int start){ // get pointer to a line, extract 1st element and return new offset for the next component
	int i = start;

	while (* (line + i) == '.' || * (line + i) == '$' || * (line + i) >= '0' ) {
		component[i - start] = * (line + i);
		i++;
	}

	component[i - start] = '\0'; // Terminate the string

	while (!(* (line + i) == '.' || * (line + i) == '$' || * (line + i) >= '0')){ // Clear trailing whitespaces
		i++;
	}
	return i;
}




int main(int argc, char* argv[]) { // argv[1] = program.asm, argv[2] = imemin.txt, argv[3] = dmemin.txt
	FILE *asmb; //file pointer to program.asm
	FILE *mcode;//file pointer to imemin.txt
	FILE *datamem;//file pointer to dmemin.txt 
	asmb = fopen(argv[1], "r");// only for read
	mcode = fopen(argv[2],"w");//read and write
	char line [LINE_SIZE]; 
	datamem = fopen(argv[3], "w");
	char labels [LABEL_COUNT][LABEL_SIZE]; // support for up to 250 labels
	int label_addresses [LABEL_COUNT];

	
	int int_word_address, int_word_value, dmem[4096];			//define int of address, int of data, array such that x[address] = data and assist vars
	for (int i = 0; i < 4096; i++){
		dmem[i] = 0;
	}       

	// TODO initialize comes here
	// imemin.txt should get initialized to 000000000000\n * 4096
	// dmemin.txt should get initialized to 00000000\n * 4096


	// 				------------------------------------- 1st pass on the code: ----------------------------------
	int address = 0;
	int label_index = 0;
	int line_index = 0;
	while (fscanf(asmb, "%[^\n]\n", line) != EOF) { 
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
	// 				------------------------------------- 2nd pass on the code: ----------------------------------

	long long decoded_instruction; // 48 bits per instruction
	long long converted_instruction;
	line_index = 0;
    while (fscanf(asmb, "%[^\n]\n", line) != EOF) { 
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
			printf("Instruction:   | \"%s\"\n", op_code);
			char word_address[15], word_value[15];											//define string of address and string of data

			start = get_component(line, word_address, start);
			start = get_component(line, word_value, start);
			
			if (word_address[0] == '0' && word_address[1] == 'x')   {						//check if need to translate the string into dec or hex
				int_word_address = hex_string_to_int(* (&(word_address) + 2));				//actual conversion
			}else{
				int_word_address = dec_string_to_int(word_address);
			}
				
			if (word_value[0] == '0' && word_value[1] == 'x')   {							//check if need to translate the string into dec or hex
				int_word_value = hex_string_to_int(* (&(word_value) + 2));					//actual conversion
			}else{
				int_word_value = dec_string_to_int(word_value);
			}
		               
				dmem[int_word_address] = int_word_value;                                	 //dmem[address] = data
				printf("Word address:  | %d\n", int_word_address);
				printf("Word Value:    | %d\n", int_word_value);
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
	// filling dmemin
	int length_of_dmem = sizeof(dmem) / sizeof(dmem[0]);
	
	dmemin_fill(datamem, dmem, length_of_dmem);

	fclose(asmb);
	fclose(mcode); 
	fclose(datamem);
	return 0;
}
