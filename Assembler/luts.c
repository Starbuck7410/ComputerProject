#include <stdio.h>
#include <string.h>

int eq_str();


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
    return -1;
}

int find_register(char reg[]) { //this function takes the exact string name of the register ($s0 and not $s0,) and returns the register number, as well as some I/O shit.
    if ((eq_str(reg, "$zero")) | (eq_str(reg, "$0")) | (eq_str(reg, "irq0enable")))
        return 0;
    if ((eq_str(reg, "$imm1")) | (eq_str(reg, "irq1enable")))
        return 1;
    if ((eq_str(reg, "$imm2")) | (eq_str(reg, "irq2enable")))
        return 2;
    if ((eq_str(reg, "$v0")) | (eq_str(reg, "irq0status")))
        return 3;
    if ((eq_str(reg, "$a0")) | (eq_str(reg, "irq1status")))
        return 4;
    if ((eq_str(reg, "$a1")) | (eq_str(reg, "irq2status")))
        return 5;
    if ((eq_str(reg, "$a2")) | (eq_str(reg, "irqhandler")))
        return 6;
    if ((eq_str(reg, "$t0")) | (eq_str(reg, "irqreturn")))
        return 7;
    if ((eq_str(reg, "$t1")) | (eq_str(reg, "clks")))
        return 8;
    if ((eq_str(reg, "$t2")) | (eq_str(reg, "leds")))
        return 9;
    if ((eq_str(reg, "$s0")) | (eq_str(reg, "display7seg")))
        return 10;
    if ((eq_str(reg, "$s1")) | (eq_str(reg, "timerenable")))
        return 11;
    if ((eq_str(reg, "$s2")) | (eq_str(reg, "timercurrent")))
        return 12;
    if ((eq_str(reg, "$gp")) | (eq_str(reg, "timermax")))
        return 13;
    if ((eq_str(reg, "$sp")) | (eq_str(reg, "diskcmd")))
        return 14;
    if ((eq_str(reg, "$ra")) | (eq_str(reg, "disksector")))
        return 15;
    if (eq_str(reg, "diskbuffer"))
        return 16;
    if (eq_str(reg, "diskstatus"))
        return 17;
    if (eq_str(reg, "reserved"))
        return 18;
    if (eq_str(reg, "monitoraddr"))
        return 20;
    if (eq_str(reg, "monitordata"))
        return 21;
    if (eq_str(reg, "monitorcmd"))
        return 22;
    return -1; //if no register name found -> returns -1
}