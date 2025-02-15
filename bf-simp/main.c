#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#define BLOCK_SIZE 200

// Brainfuck translation is as follows:
// $s0 will be used for the values
// $t0 will be used for addresses
// for example: + will load from $s0 to $t0, increment $t0, and store back at $s0
// The program pointer will start at 4096/2 = 2048


int main(int argc, char * argv[]){ // loops on the main bf code, transpiles each character to a chunk of assembly code.
    char next_block [BLOCK_SIZE];
    if (argc != 3){
        printf("Usage: %s <brainfuck file> <output assembly file>\n", argv[0]);
        return 1;
    }
    FILE * bf_file = fopen(argv[1], "r"); // Open the brainfuck code
    if (bf_file == NULL){
        printf("Error: Brainfuck file not found\n");
        return 1;
    }
    FILE * asmb_file = fopen(argv[2], "w");
    if (asmb_file == NULL){
        printf("Error: Can't open assembly output file\n");
        return 1;
    }


    // Formalities are OVER. Time to write some ASSEMBLY.
    init_code(next_block);
    fprintf(asmb_file ,"%s\n", next_block);
    while (1){
        char c = fgetc(bf_file);
        if (c == EOF){
            break;
        }
        switch (c){
            case '+':
                add(next_block);
                break;
            case '-':
                sub(next_block);
                break;
            case '>':
                right(next_block);
                break;
            case '<':
                left(next_block);
                break;
            case '.':
                out(next_block);
                break;
            default:
                strncpy(next_block, "", 1);
                break;
            }
        fprintf(asmb_file ,"%s\n", next_block);
    }
    halt(next_block);
    fprintf(asmb_file ,"%s\n", next_block);
    fclose(bf_file);
    fclose(asmb_file);
    return 0;
}