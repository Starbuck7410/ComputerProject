#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "mem.h"
#include "functions.h"
#include "instructions.h"


int execute_instruction(instruction_t instruction, machine_state_t * machine_state, int debug) {   

    // returns 0 on success, will decide of error codes for other things

    //  -------------------------------- Arithmetic --------------------------------
    if(instruction.opcode == 0){ // ADD
        machine_state->registers[instruction.registers[0]] =
            machine_state->registers[instruction.registers[1]]
          + machine_state->registers[instruction.registers[2]] 
          + machine_state->registers[instruction.registers[3]];
    }
    if(instruction.opcode == 1){ // SUB
        machine_state->registers[instruction.registers[0]] = 
              machine_state->registers[instruction.registers[1]] 
            - machine_state->registers[instruction.registers[2]] 
            - machine_state->registers[instruction.registers[3]];
    }
    if(instruction.opcode == 2){ // MAC
        machine_state->registers[instruction.registers[0]] = 
              machine_state->registers[instruction.registers[1]] 
            * machine_state->registers[instruction.registers[2]] 
            + machine_state->registers[instruction.registers[3]];
    }
    if(instruction.opcode == 3){ // AND
        machine_state->registers[instruction.registers[0]] = 
              machine_state->registers[instruction.registers[1]] 
            & machine_state->registers[instruction.registers[2]] 
            & machine_state->registers[instruction.registers[3]];
    }
    if(instruction.opcode == 4){ // OR
        machine_state->registers[instruction.registers[0]] =
              machine_state->registers[instruction.registers[1]] 
            | machine_state->registers[instruction.registers[2]] 
            | machine_state->registers[instruction.registers[3]];
    }    
    if(instruction.opcode == 5){ // XOR
        machine_state->registers[instruction.registers[0]] = 
              machine_state->registers[instruction.registers[1]] 
            ^ machine_state->registers[instruction.registers[2]] 
            ^ machine_state->registers[instruction.registers[3]];
    }
    if(instruction.opcode == 6){ // SLL
        machine_state->registers[instruction.registers[0]] = 
               machine_state->registers[instruction.registers[1]] 
            << machine_state->registers[instruction.registers[2]];
    }
    if(instruction.opcode == 7){ // SRA
        machine_state->registers[instruction.registers[0]] = 
               machine_state->registers[instruction.registers[1]] 
            >> machine_state->registers[instruction.registers[2]];
    }
    if(instruction.opcode == 8){ // SRL
        machine_state->registers[instruction.registers[0]] = 
          (machine_state->registers[instruction.registers[1]] >> machine_state->registers[instruction.registers[2]]) 
          & ~ (0xFFFFFFFF << (32 - machine_state->registers[instruction.registers[2]]));
    }

    //  -------------------------------- Branches --------------------------------

    if(instruction.opcode == 9){ // BEQ
        if(machine_state->registers[instruction.registers[1]] == machine_state->registers[instruction.registers[2]]){
            machine_state->PC = (machine_state->registers[instruction.registers[3]] & 0xFFF) - 2;
        }
    }
    if(instruction.opcode == 10){ // BNE
        if(machine_state->registers[instruction.registers[1]] != machine_state->registers[instruction.registers[2]]){
            // printf("true\n");
            machine_state->PC = (machine_state->registers[instruction.registers[3]] & 0xFFF) - 2;
        }
    }
    if(instruction.opcode == 11){ // BLT
        if(machine_state->registers[instruction.registers[1]] < machine_state->registers[instruction.registers[2]]){
            machine_state->PC = (machine_state->registers[instruction.registers[3]] & 0xFFFF) - 2;
        }
    }
    if(instruction.opcode == 12){ // BGT
        if(machine_state->registers[instruction.registers[1]] > machine_state->registers[instruction.registers[2]]){
            machine_state->PC = (machine_state->registers[instruction.registers[3]] & 0xFFFF) - 2;
        }
    }
    if(instruction.opcode == 13){ // BLE
        if(machine_state->registers[instruction.registers[1]] <= machine_state->registers[instruction.registers[2]]){
            machine_state->PC = (machine_state->registers[instruction.registers[3]] & 0xFFFF) - 2;
        }
    }
    if(instruction.opcode == 14){ // BGE
        if(machine_state->registers[instruction.registers[1]] >= machine_state->registers[instruction.registers[2]]){
            machine_state->PC = (machine_state->registers[instruction.registers[3]] & 0xFFFF) - 2;
        }
    }
    if(instruction.opcode == 15){ // JAL
        machine_state->registers[instruction.registers[0]] = machine_state->PC + 2;
        machine_state->PC = (machine_state->registers[instruction.registers[3]] & 0xFFFF) - 2;
    }
    //  -------------------------------- Memory --------------------------------
    if (instruction.opcode == 16){ // LW
        if(instruction.registers[0] == 0 && debug){
            printf("DEBUG PRINT: Register %d is %d\n", instruction.registers[1], machine_state->registers[instruction.registers[1]]);
            return 0;
        }
        int address = machine_state->registers[instruction.registers[1]] + machine_state->registers[instruction.registers[2]];
        int value = mem_read_idx(machine_state->memory, address) + machine_state->registers[instruction.registers[3]];
        if(address >= machine_state->memory->max_size || address < 0){
            error("Error: Address out of bounds\n");
            return 1;
        }
        machine_state->registers[instruction.registers[0]] = value;
    }
    if (instruction.opcode == 17) { // SW
        int address = machine_state->registers[instruction.registers[1]] + machine_state->registers[instruction.registers[2]];
        if(address >= machine_state->memory->max_size || address < 0){
            error("Error: Address out of bounds\n");
            return 1;
        }
        mem_write_idx(machine_state->memory, machine_state->registers[instruction.registers[0]] + machine_state->registers[instruction.registers[3]], address);
    }
    //  -------------------------------- I/0 --------------------------------
    if (instruction.opcode == 18) { // reti
        machine_state->PC =  machine_state->io_registers[7] - 2;
        machine_state->in_isr = 0;
    }
    if (instruction.opcode == 19) { // in
        machine_state->registers[instruction.registers[0]] = machine_state->io_registers[
            machine_state->registers[instruction.registers[1]] + machine_state->registers[instruction.registers[2]]
        ];
    }
    if (instruction.opcode == 20) { // out
        machine_state->io_registers[
            machine_state->registers[instruction.registers[1]] + machine_state->registers[instruction.registers[2]]
        ] = machine_state->registers[instruction.registers[3]];
    }

    machine_state->registers[0] = 0;
    return 0;
}

void decode_instruction(uint64_t input, instruction_t * instruction) {
	instruction->immediates[1] = (input & 0x7FFFF) - (input & 0x80000);
	input = (input >> 20);
	instruction->immediates[0] = (input & 0x7FFFF) - (input & 0x80000);
	input = (input >> 20);
	instruction->registers[3] = (input & 0xF);
	input = (input >> 4);
	instruction->registers[2] = (input & 0xF);
	input = (input >> 4);
	instruction->registers[1] = (input & 0xF);
	input = (input >> 4);
	instruction->registers[0] = (input & 0xF);
	input = (input >> 4);
	instruction->opcode = (int32_t)input;
}