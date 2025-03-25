#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#define BLOCK_SIZE 20000
#define LOOP_STACK_SIZE 500

// Brainfuck translation is as follows:
// $s0 will be used for the values
// $t0 will be used for addresses
// for example: + will load from $s0 to $t0, increment $t0, and store back at $s0
// The program pointer will start at 4096/2 = 2048


int main(int argc, char * argv[]){ // loops on the main bf code, transpiles each character to a chunk of assembly code.
    int loop_stack_idx = 0;
    int loop_stack[LOOP_STACK_SIZE];
    int loop_id = 0;
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
    
    typeface(next_block);
    fprintf(asmb_file ,"%s\n", next_block);
    fprintf(asmb_file, "\n");
    
    init_code(next_block);
    fprintf(asmb_file ,"%s\n", next_block);
    fprintf(asmb_file, "# PROGRAM START\n");
    int in_dex = 0;
    char prevc;
    int count = 1;
    while (1){
        char c = fgetc(bf_file);
        
        if(prevc == '+' || prevc == '-' || prevc == '<' || prevc == '>'){
            if(prevc == c){
                count++;
                strncpy(next_block, "", 1);
            }else{
                switch (prevc){
                    case '+':
                        add(next_block, count);
                        break;
                    case '-':
                        sub(next_block, count);
                        break;
                    case '>':
                        right(next_block, count);
                        break;
                    case '<':
                        left(next_block, count);
                        break;
                }
                count = 1;
            }
        }else{
            switch (prevc){
                case '.':
                    out(next_block);
                    break;
                case '[':
                    open_loop(next_block, loop_id);
                    loop_stack[loop_stack_idx] = loop_id;
                    loop_stack_idx++;
                    loop_id++;
                    break;
                case ']':
                    loop_stack_idx--;
                    close_loop(next_block, loop_stack[loop_stack_idx]);
                    break;
                case ',':
                    in(next_block, in_dex);
                    in_dex++;
                    break;
                default:
                    strncpy(next_block, "", 1);
                    break;
            }
        }
        
        prevc = c;
        if(strcmp(next_block, "")){
            fprintf(asmb_file ,"%s\n", next_block);
        }
        if (c == EOF){
            break;
        }
    }
    halt(next_block);
    fprintf(asmb_file ,"%s\n", next_block);

    print_proc(next_block);
    fprintf(asmb_file ,"%s\n", next_block);
    fclose(bf_file);
    fclose(asmb_file);

    if(loop_stack_idx != 0){
        printf("ERROR: MISMATCHING BRACKETS DETECTED.\n This code may run, but I take no responsibility for it.\n");
    }

    return 0;
}