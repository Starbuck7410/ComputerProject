#include <string.h>

int init_code(char * next_block){
    char * init_code = "add $s0, $imm1, $imm1, $zero, 1024, 0  # Initialize the memory pointer\n"
                       "add $sp, $imm1, $zero, $zero, 512, 0  # Initialize the output stack pointer\n"
                       "# Program starts here\n";
    strncpy(next_block, init_code, strlen(init_code) + 1);
    return 0;
}

int add(char * next_block){
    char * add_asm = "lw $t0, $s0, $zero, $zero, 0, 0  # load\n"
                     "sw $t0, $s0, $zero, $imm1, 1, 0  # add and store\n";
    strncpy(next_block, add_asm, strlen(add_asm) + 1);
    return 0;
}

int sub(char * next_block){
    char * sub_asm = "lw $t0, $s0, $zero, $zero, 0, 0  # load\n"
                     "sw $t0, $s0, $zero, $imm1, -1, 0  # substract and store\n";
    strncpy(next_block, sub_asm, strlen(sub_asm) + 1);
    return 0;
}

int right(char * next_block){
    char * rgt_asm = "add $s0, $s0, $imm1, $zero, 1, 0  # Move pointer to the right\n";
    strncpy(next_block, rgt_asm, strlen(rgt_asm) + 1);
    return 0;
}

int left(char * next_block){
    char * lft_asm = "add $s0, $s0, $imm1, $zero, 1, 0 # Move pointer to the left\n";
    strncpy(next_block, lft_asm, strlen(lft_asm) + 1);
    return 0;
}

int out(char * next_block){
    char * out_asm = "lw $t0, $s0, $zero, $zero, 0, 0  # load current byte\n"
                     "sw $t0, $sp, $zero, $zero, 0, 0  # store in output stack\n"
                     "add $sp, $sp, $imm1, $zero, -1, 0  # increment output stack pointer\n";
    strncpy(next_block, out_asm, strlen(out_asm) + 1);
    return 0;
}

int halt(char * next_block){
    char * halt_asm = "halt $zero, $zero, $zero, $zero, 0, 0 # Stop the program\n";
    strncpy(next_block, halt_asm, strlen(halt_asm) + 1);
    return 0;
}