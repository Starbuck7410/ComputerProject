#include <stdio.h>
#include <string.h>

int eq_str(char str1[], char str2[]){
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

