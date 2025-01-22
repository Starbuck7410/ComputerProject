#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int execute(int op_code, int* inst_regs, int* imms, int* registers, int* P_PC, int* local_memory, unsigned int * io_registers);
int decode(long long input, int regs[], int imm[]);
long long fetch(FILE* imemin_file, int PC);
int cycles = 0;
long long instruction;
int fill_memarray_from_dmem(int mem[], char* dmemin_file_path);
int dmemout(int * local_mem, char* arg5);
void execute_interrupt(unsigned int* io_registers, int* PC, int* in_isr);
int execute_disk(unsigned int * io_registers, FILE * disk_file, int * local_memory);
int monitor_out(char* arg13, char* arg14, unsigned char monitor[]);
void trace_out(FILE* trace_file, int PC, long long inst, int registers[]);
void irq2_check(FILE* irq2in_file, int pc, int io_registers[]);

#define HALT_OP 21
#define MAX_PC 4096

int main(int argc, char * argv[]) {
	int pc = 0;

	// The initial values of the local and hardware registers on reset are 0.
	int registers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned int io_registers[23] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	// initialize monitor array and set to 0's.
	unsigned char monitor[256 * 256];
	for (int k = 0; k < 256 * 256; k++) monitor[k] = 0;

	int local_memory[4096];
	int doom_counter = 0;
	int in_isr = 0; //if in ISR then 1, else 0.
	int irq = 0;

	

	FILE* mcode = fopen(argv[1], "r");
	fill_memarray_from_dmem(local_memory, argv[2]); // fills local memory from dmemin.txt
	FILE* disk_in_file = fopen(argv[3], "r");
	FILE* irq2in_file = fopen(argv[4], "r");
	// dmemout 5 handled in its own function
	FILE* regout_file = fopen(argv[6], "w");
	FILE* trace_file = fopen(argv[7], "w");
	FILE* hwregtrace_file = fopen(argv[8], "w");
	FILE* cycles_file = fopen(argv[9], "w");
	FILE* leds_file = fopen(argv[10], "w");
	FILE* disp7seg_file = fopen(argv[11], "w");
	FILE* disk_out_file = fopen(argv[12], "w");
	// Monitor files 13 and 14 also opened in respective functions
	
	if (mcode == NULL) {
		perror("ERROR");
		return 1;
	}

	if (disk_in_file == NULL){
		perror("ERROR ");
		printf("Cant open file diskin. Crashing...");
		return 1;
	}

	while(1){ // main run loop

		instruction = fetch(mcode, pc);
		
		//printf("instruction: %012llX\n", instruction);
		

		// takes long long, outputs int opcode, int[4] reg addresses, int[2] imm values
		// changes their values in decode as such: 
		// opcode is returned 
		// reg[0] = rd, reg[1] = rs, reg[2] = rt, reg[3] = rm
		// imm[0] = immediate 1, imm[1] = immediate 2
		int opcode, inst_regs[4], imm[2];
		opcode = decode(instruction, inst_regs, imm);

		// halt instruction
		if (opcode == HALT_OP || pc >= MAX_PC)
		{ 
			printf("halted!\n");
			break;
		}

		// STAGE: Traces
		trace_out(trace_file, pc, instruction, registers);
		// TODO - Daniel check if in/out and update hwregtrace file
	
		// STAGE:  Interrupts
		
		// checks for Interrupt 2
		irq2_check(irq2in_file, pc, io_registers);
		
		// executing interrupts
		irq = (io_registers[0] & io_registers[3]) | (io_registers[1] & io_registers[4]) | (io_registers[2] & io_registers[5]);
		if (irq == 1) {
			execute_interrupt(io_registers, &pc, &in_isr);
		}


		// STAGE:  Execute
		if (execute(opcode, inst_regs, imm, registers, &pc, local_memory, io_registers)){
			printf("Error in execute\n");
			return 1;
		}
		
		// STAGE:  I/O
		
		// TODO - Shraga:
		// Write LEDs & 7segdisp
		// surprise tool that will help you later: fprintf(leds_file, "%uX", io_registers[9]);
		
		//if monitor command is 1 write to monitor array, data from I/O to adress given by I/O.
		if (io_registers[22]) {
			monitor[io_registers[20]] = io_registers[21];
			io_registers[22] = 0;
		}


		// TODO - SHRAGA Rewrite this garbage
		// execute_disk(unsigned int * io_registers, FILE * disk_file, int * local_memory)
		// doom counter is the counter for the clock cycles since calling the diskcmd
		if (io_registers[14]) {
			doom_counter = 1024; //TODO later put this in a check if execute disk succeeded
			execute_disk(io_registers, disk_in_file, local_memory);
		}
		if (doom_counter) {
			doom_counter--;
			if (!doom_counter) {
				io_registers[17] = 0;
				// raise the interrupt
			}
		}

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
	monitor_out(argv[13], argv[14], monitor); //13 - monitor.txt, 14 - monitor.yuv


	fclose(leds_file);
	fclose(mcode);
	fclose(trace_file);
	fclose(regout_file);
	fclose(cycles_file);
	fclose(irq2in_file);
	return 0;
}



