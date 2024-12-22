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


int find_instruction(char name[]){
    if (eq_str(name, "hello")){
        return 0x50;
    }
    return 1;
}

int find_register(char reg[]){
    return 1;
}