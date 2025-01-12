int execute(int op_code, int * inst_regs, int * imms, int * registers){ // returns 0 on success, will decide of error codes for other things
    for (int i = 0; i < 4; i++){
        if (inst_regs[i] == 1){
            registers[1] = imms[0];
        }
        if (inst_regs[i] == 2){
            registers[2] = imms[1];
        }
    }

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
    registers[0] = 0;

}