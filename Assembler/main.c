#include <stdio.h>
int find_instruction();
int find_register();

int get_component(char * line, char * op_code,int start){
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

int main(int argc, char* argv[]) {
	FILE *asmb;
	FILE *mcode;
	asmb = fopen(argv[1], "r");
	mcode = fopen("imemin.txt","w");
	char* line = NULL; 
	long len; 
	long read; // in case the file is big 
	long decoded_instruction; // 48 bits per instruction means ints are not enough...
    while ((read = getline(&line, &len, asmb)) != -1) { 
		if (line[read-1] == '\n'){
		 	line[read-1] = '\0'; 
		} // remove newlines if they exist

		char op_code[10];
		int start = get_component(line, op_code, 0);

		printf("Instruction:  | %s\n", op_code);
		decoded_instruction = find_instruction(op_code); // placeholder decoder

		// get all 4 registers
		for (int i = 0; i<4; i++){
			char reg[10];
			start = get_component(line + start, reg, start);
			// decode reg into decoded register here
			printf("got Register: | %s\n", reg);
		}

		// get both immediates
		for (int i = 0; i<2; i++){
			char reg[10];
			start = get_component(line + start, reg, start);
			// turn immediate from string to number
			printf("got immediate: | %s\n", reg);
		}

		//combine and write line with fprintf
		// fprintf(mcode, "%x\n", decoded_instruction);
	}
	fclose(asmb);
	fclose(mcode); 
	return 0;
}
