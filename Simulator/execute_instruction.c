#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
void error(char * text);
int execute(int op_code, int* inst_regs, int* imms, int* registers, 
    int* P_PC, int* local_memory, unsigned int* io_registers, int * in_isr) 
{   

    // returns 0 on success, will decide of error codes for other things

    //  -------------------------------- Arithmetic --------------------------------
    if(op_code == 0){ // ADD
        registers[inst_regs[0]] = registers[inst_regs[1]] + registers[inst_regs[2]] + registers[inst_regs[3]];
    }
    if(op_code == 1){ // SUB
        registers[inst_regs[0]] = registers[inst_regs[1]] - registers[inst_regs[2]] - registers[inst_regs[3]];
    }
    if(op_code == 2){ // MAC
        registers[inst_regs[0]] = registers[inst_regs[1]] * registers[inst_regs[2]] + registers[inst_regs[3]];
    }
    if(op_code == 3){ // AND
        registers[inst_regs[0]] = registers[inst_regs[1]] & registers[inst_regs[2]] & registers[inst_regs[3]];
    }
    if(op_code == 4){ // OR
        registers[inst_regs[0]] = registers[inst_regs[1]] | registers[inst_regs[2]] | registers[inst_regs[3]];
    }    
    if(op_code == 5){ // XOR
        registers[inst_regs[0]] = registers[inst_regs[1]] ^ registers[inst_regs[2]] ^ registers[inst_regs[3]];
    }
    if(op_code == 6){ // SLL
        registers[inst_regs[0]] = registers[inst_regs[1]] << registers[inst_regs[2]];
    }
    if(op_code == 7){ // SRA
        registers[inst_regs[0]] = registers[inst_regs[1]] >> registers[inst_regs[2]];
    }
    if(op_code == 8){ // SRL
        registers[inst_regs[0]] = (registers[inst_regs[1]] >> registers[inst_regs[2]]) & ~ (0xFFFFFFFF << registers[inst_regs[2]]);
    }

    //  -------------------------------- Branches --------------------------------

    if(op_code == 9){ // BEQ
        if(registers[inst_regs[1]] == registers[inst_regs[2]]){
            *P_PC = (registers[inst_regs[3]] & 0xFFF) - 1;
        }
    }
    if(op_code == 10){ // BNE
    // printf("%d != %d: ", registers[inst_regs[1]], registers[inst_regs[2]]);
        if(registers[inst_regs[1]] != registers[inst_regs[2]]){
            // printf("true\n");
            *P_PC = (registers[inst_regs[3]] & 0xFFF) - 1;
        }
    }
    if(op_code == 11){ // BLT
        if(registers[inst_regs[1]] < registers[inst_regs[2]]){
            *P_PC = (registers[inst_regs[3]] & 0xFFF) - 1;
        }
    }
    if(op_code == 12){ // BGT
        if(registers[inst_regs[1]] > registers[inst_regs[2]]){
            *P_PC = (registers[inst_regs[3]] & 0xFFF) - 1;
        }
    }
    if(op_code == 13){ // BLE
        if(registers[inst_regs[1]] <= registers[inst_regs[2]]){
            *P_PC = (registers[inst_regs[3]] & 0xFFF) - 1;
        }
    }
    if(op_code == 14){ // BGE
        if(registers[inst_regs[1]] >= registers[inst_regs[2]]){
            *P_PC = (registers[inst_regs[3]] & 0xFFF) - 1;
        }
    }
    if(op_code == 15){ // JAL
        // printf("Jumping to address %d\n", (registers[inst_regs[3]] & 0xFFF));
        // printf("Returning to address %d\n", * P_PC + 1);
        registers[inst_regs[0]] = * P_PC + 1;
        *P_PC = (registers[inst_regs[3]] & 0xFFF) - 1;
    }
    //  -------------------------------- Memory --------------------------------
    if (op_code == 16){ // LW
        int address = registers[inst_regs[1]] + registers[inst_regs[2]] + registers[inst_regs[3]];
        if(address >= 4095 || address < 0){
            error("Error: Address out of bounds\n");
            return 1;
        }
        registers[inst_regs[0]] = local_memory[address];
        // printf("Loaded value %d from address %d\n", registers[inst_regs[0]], registers[inst_regs[1]] + registers[inst_regs[2]] + registers[inst_regs[3]]);
    }
    if (op_code == 17) { // SW
        int address = registers[inst_regs[1]] + registers[inst_regs[2]];
        if(address >= 4095 || address < 0){
            error("Error: Address out of bounds\n");
            return 1;
        }
        printf("Storing value %d at address %d\n", registers[inst_regs[0]], registers[inst_regs[1]] + registers[inst_regs[2]] + registers[inst_regs[3]]);
        local_memory[address] = registers[inst_regs[0]] + registers[inst_regs[3]];
    }
    //  -------------------------------- I/0 --------------------------------
    if (op_code == 18) { // reti
        *P_PC = io_registers[7] - 1;
        *in_isr = 0;
    }
    if (op_code == 19) { // in
        registers[inst_regs[0]] = io_registers[registers[inst_regs[1]] + registers[inst_regs[2]]];
    }
    if (op_code == 20) { // out
        io_registers[registers[inst_regs[1]] + registers[inst_regs[2]]] = registers[inst_regs[3]];
    }


    registers[0] = 0;
    return 0;
}