#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int execute(int op_code, int* inst_regs, int* imms, int* registers, int* P_PC, int* local_memory);
int decode(long long input, int regs[], int imm[]);
long long fetch(FILE* imemin_file, int PC);
int cycles = 0;
long long instruction;
int fill_memarray_from_dmem(int mem[], char* dmemin_file_path);
int dmemout(int local_mem[], char* arg5);

/* 
	
	ZOHAR:
	Cycles.txt, Trace.txt, Regout.txt output
	Cycles and regout get updated only at the end, but trace gets updated at the TODO:ZOHAR tag

*/
void trace_out(FILE* trace_file, int PC, long long inst, int registers[]);

#define HALT_OP 21
#define MAX_PC 4096

int main(int argc, char * argv[]) {
	int pc = 0;
	int registers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int local_memory[4096];
	//initialize local memory array
	
	fill_memarray_from_dmem(local_memory,argv[2]);
	printf("DEBUG - Calculating fib %d\n", local_memory[64]);
	//fills local memory from dmemin.txt
	
	FILE* mcode; //file pointer to imemin.txt
	mcode = fopen(argv[1], "r"); //read and write

	FILE* trace_file = fopen("trace.txt", "w");

	while(1){
		instruction = fetch(mcode, pc);
		
		// printf("instruction: %012llX\n", instruction);
		
		int opcode, inst_regs[4], imm[2];
		opcode = decode(instruction, inst_regs, imm);

		// halt instruction
		if (opcode == HALT_OP || pc >= MAX_PC)
		{ 
			printf("halted!\n");
			break;
		}
		// takes long long, outputs int opcode, int[4] reg addresses, int[2] imm values
		// changes their values in decode as such: 
		// opcode is returned 
		// reg[0] = rd, reg[1] = rs, reg[2] = rt, reg[3] = rm
		// imm[0] = immediate 1, imm[1] = immediate 2

	//	printf("opcode is %d\nreg rd is %d\nreg rs is %d\nreg rt is %d\nreg rm is %d\nimmediate 1 is %d\nimmediate 2 is %d.\n", opcode, inst_regs[0], inst_regs[1], inst_regs[2], inst_regs[3], imm[0], imm[1]);

		trace_out(trace_file, pc, instruction, registers);

		if (execute(opcode, inst_regs, imm, registers, &pc, local_memory)){
			printf("Error in execute\n");
			return 1;
		}
		//printf("values in registers after operation: %d, %d, %d, %d\n", registers[inst_regs[0]], registers[inst_regs[1]], registers[inst_regs[2]], registers[inst_regs[3]]);
		cycles++;
		pc++;
	}

	printf("DEBUG - Fibonacci number %d: %d\n", local_memory[64], local_memory[65]);
	fclose(mcode);
	fclose(trace_file);
	mcode = NULL;
	dmemout(local_memory, argv[5]);
	return 0;
}



