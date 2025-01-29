#include <stdio.h>
#include <stdlib.h>
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
		value += (number[i] - '0') * sign * pow_int(10, (len - i - 1));
	}
	return value;
}


int hex_string_to_int(char number[]){ // You MUST pass the string WITHOUT the 0x
	int len = strlen(number);
	int value = 0;
	int i = 0;
	int sign = 1;
	if (number[0] == '-'){ // fuck it let's allow negative hex numbers why not (0x-8000)
		sign = -1;
		i++;
	}
	for (; i < len; i++){
		if ('0' <= number[i] && number[i] <= '9'){
			value += (number[i] - '0') * sign * pow_int(16, (len - i - 1));
		}
		if ('a' <= number[i] && number[i] <= 'f'){
			value += (number[i] - 'a' + 10) * sign * pow_int(16, (len - i - 1));
		}
		if ('A' <= number[i] && number[i] <= 'F'){
			value += (number[i] - 'A' + 10) * sign * pow_int(16, (len - i - 1));
		}
	}
	return value;
}

int str_to_int(char * text){
	int value;
	if (text[0] == '0' && text[1] == 'x')   {						//check if need to translate the string into dec or hex
		value = hex_string_to_int(text + 2);				//actual conversion
	}else{
		value = dec_string_to_int(text);
	}
	return value;
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
	printf("NASS Assembler:\n");
	printf("Nameless asssembler & simulator for SIMP (Name pending approval)\n");
	printf("Usage: ./assembler <program.asm> <imemin.txt> <dmemin.txt> [diskin.txt] [irq2in.txt]\n");
	printf("You do not have to specify the last 2 arguments.\n");
	printf("Specifying them enables our special \"Extra features mode\"\n");
	printf("Extra features mode allows you to use the .disksector and .interrupt instructions\n");
	printf(".disksector allows you to specify content for the diskin file\n");
	printf("and .interrupt allows you to specify interrupt cycles for the irq2in file\n\n");
	printf("NASS is free software using by the GPL V3 license.\n");
	printf("Copyright Shraga, Zohar, Daniel and Raz\n");
}

long long get_component(char * line, char * component, int start){ // get pointer to a line, extract 1st element and return new offset for the next component
	int i = start;

	if ( * (line + i) == '\0'){
		error("ERROR: NOT ENOUGH ARGS GIVEN TO INSTRUCTION IN LINE\n");
		printf("%s\n", line);
		exit(1);
	}
	
	while (* (line + i) == '-' || * (line + i) == '.' || * (line + i) == '#' || * (line + i) == '$' || * (line + i) >= '0' ) {
		component[i - start] = * (line + i);
		i++;
	}

	component[i - start] = '\0'; // Terminate the string
	while (!(* (line + i) == '\0' || * (line + i) == '-' || * (line + i) == '.' || * (line + i) == '$' || * (line + i) >= '0')){ // Clear trailing whitespaces
		i++;
	}
	return i;
}
