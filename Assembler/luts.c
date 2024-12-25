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


long long find_instruction(char name[]){
    if (eq_str(name, "hello")){
        return 0x50;
    }
    return 1;
}

long long find_register(char reg[]) {     //This function recives a string starting with '$' and reg name and returns its integer number.
    if ((reg[1] == 'z' & reg[2] == 'e' & reg[3] == 'r' & reg[4] == 'o') | reg[1] == '0') //0 for $zero or $0
        return 0;
    if (reg[1] == 'i' & reg[2] == 'm' & reg[3] == 'm' & reg[4] == '1') 
        return 1;
    if (reg[1] == 'i' & reg[2] == 'm' & reg[3] == 'm' & reg[4] == '2')
        return 2;
    if (reg[1] == 'v' & reg[2] == '0')
        return 3;
    if (reg[1] == 'a' & reg[2] == '0')
        return 4;
    if (reg[1] == 'a' & reg[2] == '1')
        return 5;
    if (reg[1] == 'a' & reg[2] == '2')
        return 6;
    if (reg[1] == 't' & reg[2] == '0')
        return 7;
    if (reg[1] == 't' & reg[2] == '1')
        return 8;
    if (reg[1] == 't' & reg[2] == '2')
        return 9;
    if (reg[1] == 's' & reg[2] == '0')
        return 10;
    if (reg[1] == 's' & reg[2] == '1')
        return 11;
    if (reg[1] == 's' & reg[2] == '2')
        return 12;
    if (reg[1] == 'g' & reg[2] == 'p')
        return 13;
    if (reg[1] == 's' & reg[2] == 'p')
        return 14;
    if (reg[1] == 'r' & reg[2] == 'a')
        return 15;
    return -1;                    //If no register found -> return -1
} 