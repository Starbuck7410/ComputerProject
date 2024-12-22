int eq_str(char str1[], char str2[]){
    if (sizeof(*str1) != sizeof(*str2)){
        return 0;
    }
    for (int i = 0; i < sizeof(*str1); i++){
        if (str1[i] != str2[i]){
            return 0;
        }
    }
    return 1;
}


int find_instruction(char name[]){
    if (eq_str(name, "hello")){
        return 2;
    }
    return 1;
}

int find_register(char reg[]){
    return 1;
}