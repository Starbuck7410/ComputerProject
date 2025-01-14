int execute(int op_code, int * inst_regs, int * imms, int * registers, int PC, long long local_memory[]) { // returns 0 on success, will decide of error codes for other things
    for (int i = 0; i < 4; i++){
        if (inst_regs[i] == 1){
            registers[1] = imms[0];
        }
        if (inst_regs[i] == 2){
            registers[2] = imms[1];
        }
    }

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
            PC = registers[inst_regs[3]] & 0xFFF;
        }
    }
    if(op_code == 10){ // BNE
        if(registers[inst_regs[1]] != registers[inst_regs[2]]){
            PC = registers[inst_regs[3]] & 0xFFF;
        }
    }
    if(op_code == 11){ // BLT
        if(registers[inst_regs[1]] < registers[inst_regs[2]]){
            PC = registers[inst_regs[3]] & 0xFFF;
        }
    }
    if(op_code == 12){ // BGT
        if(registers[inst_regs[1]] > registers[inst_regs[2]]){
            PC = registers[inst_regs[3]] & 0xFFF;
        }
    }
    if(op_code == 13){ // BLE
        if(registers[inst_regs[1]] <= registers[inst_regs[2]]){
            PC = registers[inst_regs[3]] & 0xFFF;
        }
    }
    if(op_code == 14){ // BGE
        if(registers[inst_regs[1]] >= registers[inst_regs[2]]){
            PC = registers[inst_regs[3]] & 0xFFF;
        }
    }
    if(op_code == 15){ // JAL
        registers[inst_regs[0]] = PC + 1;
        PC = registers[inst_regs[3]] & 0xFFF;
    }
    //  -------------------------------- Memory --------------------------------
    if (op_code == 16){ // LW
        registers[inst_regs[0]] = local_memory[registers[inst_regs[1]] + registers[inst_regs[2]] + registers[inst_regs[3]]];
    }
    if (op_code == 17) { // SW
        local_memory[registers[inst_regs[1]] + registers[inst_regs[2]]] = registers[inst_regs[0]] + registers[inst_regs[3]];
    }
    registers[0] = 0;

}