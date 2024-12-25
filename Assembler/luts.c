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


long long find_instruction(char name[]){ //This function recives a string of the instruction and returns its integer number.

    if (eq_str(name, "add")) return 0;
    if (eq_str(name, "sub")) return 1;
    if (eq_str(name, "mac")) return 2;
    if (eq_str(name, "and")) return 3;
    if (eq_str(name, "or")) return 4;
    if (eq_str(name, "xor")) return 5;
    if (eq_str(name, "sll")) return 6;
    if (eq_str(name, "sra")) return 7;
    if (eq_str(name, "srl")) return 8;
    if (eq_str(name, "beq")) return 9;
    if (eq_str(name, "bne")) return 10;
    if (eq_str(name, "blt")) return 11;
    if (eq_str(name, "bgt")) return 12;
    if (eq_str(name, "ble")) return 13;
    if (eq_str(name, "bge")) return 14;
    if (eq_str(name, "jal")) return 15;
    if (eq_str(name, "lw")) return 16;
    if (eq_str(name, "sw")) return 17;
    if (eq_str(name, "reti")) return 18;
    if (eq_str(name, "in")) return 19;
    if (eq_str(name, "out")) return 20;
    if (eq_str(name, "halt")) return 21;
    
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
    return -1;
} 