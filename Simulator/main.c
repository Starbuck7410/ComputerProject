#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int registers[16];
int execute();
int decode();
long long fetch();
int cycles = 0;
int pc = 0;
long long instruction;
int fill_memarray_from_dmem();
/* 
	DANIEL:
	Load dmemin.txt from argument to int array [4096] - DONE* (one strange non-warning).
	implement lw sw - Done, need to test to make sure.
	Output dmemout.txt from int array to argument - done and works.

	ZOHAR:
	Cycles.txt, Trace.txt, Regout.txt output
	Cycles and regout get updated only at the end, but trace gets updated at the TODO:ZOHAR tag

*/

int main(int argc, char * argv[]) {

	long long local_memory[4096];
	//initialize local memory array
	
	fill_memarray_from_dmem(local_memory,argv[2]);
	//fills local memory from dmemin.txt
	
	FILE* mcode; //file pointer to imemin.txt
	mcode = fopen(argv[1], "r"); //read and write

	while(0 == 0){
		instruction = fetch(mcode, pc);
		
		printf("instruction: %012llx\n", instruction);
		
		int opcode, inst_regs[4], imm[2];
		opcode = decode(instruction, inst_regs, imm);
		if (opcode == 21 || pc >= 4096){ // halt instruction
			printf("halted!\n");
			break;
		}
		// takes long long, outputs int opcode, int[4] reg addresses, int[2] imm values
		// changes their values in decode as such: 
		// opcode is returned 
		// reg[0] = rd, reg[1] = rs, reg[2] = rt, reg[3] = rm
		// imm[0] = immediate 1, imm[1] = immediate 2

	//	printf("opcode is %d\nreg rd is %d\nreg rs is %d\nreg rt is %d\nreg rm is %d\nimmediate 1 is %d\nimmediate 2 is %d.\n", opcode, inst_regs[0], inst_regs[1], inst_regs[2], inst_regs[3], imm[0], imm[1]);

		// todo:zohar update the trace here

		execute(opcode, &inst_regs, &imm, &registers, pc, local_memory);
		//printf("values in registers after operation: %d, %d, %d, %d\n", registers[inst_regs[0]], registers[inst_regs[1]], registers[inst_regs[2]], registers[inst_regs[3]]);
		cycles++;
		pc++;
	}

	fclose(mcode);
	dmemout(local_memory,argv[5]);
	return 0;
}



