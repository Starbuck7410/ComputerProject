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
		//////////////////////////////////////////////////////////
		FILE* file = fopen("text.txt", "r");                             //Open the asm instructions file
		char L[500], char_val1[15], char_val2[15];                        //define Line, string of address, string of data
		int int_val1, int_val2, x[4096], space, offset = 0, val1_0x, val2_0x; //define int of address, int of data, array such that x[address] = data and assist vars
		for (int i = 0; i < 4096; i++)                                     //initialize array to all zeros
			x[i] = 0;

		while (fscanf(file, "%500[^\n]\n", L) != EOF) {                     //read from instructions file only 500 chars or \n and save it to string L exit when reached End Of File (EOF)
			space = 0;                                                  //boolean var space tells if the space char between address and data have been found, initialized to 0
			memset(char_val1, '\0', sizeof(char_val1));                 //before each iteration set address and data strings to all /0 so they wont carry leftovers between iterations
			memset(char_val2, '\0', sizeof(char_val2));
			val1_0x = 0;                                                //set to zero flags that indicate if the address or data are given in hex
			val2_0x = 0;
			offset = 6;                                                 //we read the string L using a for loop with indicator w. because we know that the first 5 letters are .word we set an offset to 6
			if (L[0] == '.') {                                            //indicates when a .word command has been found
				for (int w = 6;w <= strlen(L);w++) {                          //for loop to go over the command
					if (L[w] == ' ') {                                    //if space has been found set space flag, skip the actual space indicator (w++)
						space = 1;
						w++;
						offset = w;                                     //set the offset to match the string length to only be whats necessary
					}

					if (space == 0) {                                     //if no space has been found, we're dealing with the address string
						if (L[w + 1] == 'x') {                              //check if it's give in hex, the [w+1] skips the "0" in "0x" as to not accidentally check for 0 for it to actually be 0 in dec
							val1_0x = 1;
							w = w + 2;
							offset = 8;
						}                               //update the w indicator as to not include "0x" (important for how Saar made Hex to Dec)
						char_val1[w - offset] = L[w];                     //copy the address part in string L to the address string
					}

					if (space == 1)                                      //a space has been found therefore were dealing with the data string
						if (L[w + 1] == 'x') {
							val2_0x = 1;                                //check if it's in hex
							w = w + 2;
							offset = w;                                 //update the offset accordingly
						}
					char_val2[w - offset] = L[w];


				}
				if (val1_0x)                                             //check if need to translate the string into dec or hex
					int_val1 = hex_string_to_int(char_val1);            //actual conversion
				else
					int_val1 = dec_string_to_int(char_val1);
				if (val2_0x)
					int_val2 = hex_string_to_int(char_val2);
				else
					int_val2 = dec_string_to_int(char_val2);

				x[int_val1] = int_val2;                                 //x[address] = data
			}
		}
		fclose(file);
	}
		//////////////////////////////////////////////////////////
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
