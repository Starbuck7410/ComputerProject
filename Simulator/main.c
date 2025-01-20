#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int execute(int op_code, int* inst_regs, int* imms, int* registers, int* P_PC, int* local_memory, unsigned int * io_registers);
int decode(long long input, int regs[], int imm[]);
long long fetch(FILE* imemin_file, int PC);
int cycles = 0;
long long instruction;
int fill_memarray_from_dmem(int mem[], char* dmemin_file_path);
int dmemout(int local_mem[], char* arg5);


void trace_out(FILE* trace_file, int PC, long long inst, int registers[]);

#define HALT_OP 21
#define MAX_PC 4096

int main(int argc, char * argv[]) {
	int pc = 0;
	int registers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned int io_registers[22];
	unsigned char monitor[256 * 256];
	int local_memory[4096];
	//initialize local memory array
	fill_memarray_from_dmem(local_memory,argv[2]);
	//fills local memory from dmemin.txt
	
	FILE* mcode; //file pointer to imemin.txt
	mcode = fopen(argv[1], "r"); //read and write
	if (mcode == NULL) {
		perror("ERROR");
		return 1;
	}

	FILE* trace_file = fopen("trace.txt", "w");
	FILE* regout_file = fopen("regout.txt", "w");
	FILE* cycles_file = fopen("cycles.txt", "w");

	while(1){
		instruction = fetch(mcode, pc);
		
		 //printf("instruction: %012llX\n", instruction);
		
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

		//printf("opcode is %d\nreg rd is %d\nreg rs is %d\nreg rt is %d\nreg rm is %d\nimmediate 1 is %d\nimmediate 2 is %d.\n", opcode, inst_regs[0], inst_regs[1], inst_regs[2], inst_regs[3], imm[0], imm[1]);

		trace_out(trace_file, pc, instruction, registers);

		// DANIEL:
		// TODO Check monitorcmd
		// execute_monitor(unsigned int * io_registers, unsigned char * monitor);
		
		// SHRAGA:
		// TODO Check diskcmd
		// execute_disk(unsigned int * io_registers, FILE * disk_file, int * local_memory, int * doom_counter)
		// doom counter is the counter for the clock cycles since calling the diskcmd

		// ZOHAR
		// TODO Check Interrupts (irq = (irq0enable & irq0status) | (irq1enable & irq1status) | (irq2enable & irq2status))
		// If / else with the execute
		// execute_interrupt(unsigned int * io_registers, int * PC)

		if (execute(opcode, inst_regs, imm, registers, &pc, local_memory, io_registers)){
			printf("Error in execute\n");
			return 1;
		}

		//printf("values in registers after operation: %d, %d, %d, %d\n", registers[inst_regs[0]], registers[inst_regs[1]], registers[inst_regs[2]], registers[inst_regs[3]]);
		cycles++;
		io_registers[8] = cycles;
		pc++;
	}
	
	// writing into regout.txt
	for (int i = 3; i < 16; i++)
	{
		fprintf(regout_file, "%08x ", registers[i]);
	}

	// writing into cycles.txt
	fprintf(cycles_file, "%d", cycles);

	dmemout(local_memory, argv[5]);

	fclose(mcode);
	fclose(trace_file);
	fclose(regout_file);
	fclose(cycles_file);
	return 0;
}



