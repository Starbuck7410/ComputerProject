#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "functions.h"

#define HALT_OP 21
#define MAX_PC 4096
// argv[0] = sim.exe,      argv[1] = imemin.txt,   argv[2] = dmemin.txt
// argv[3] = diskin.txt,   argv[4] = irq2in.txt,   argv[5] = dmemout.txt
// argv[6] = regout.txt,   argv[7] = trace.txt,    argv[8] = hwregtrace.txt
// argv[9] = cycles.txt,  argv[10] = leds.txt,    argv[11] =  display7seg.txt 
// argv[12] = diskout.txt, argv[13] = monitor.txt, argv[14] = monitor.yuv

int main(int argc, char * argv[]) {
	int debug = 0;
	if(argc == 2 && (eq_str(argv[1], "-h") || eq_str(argv[1], "-H"))){
		print_help();
		return 0;
	}

	if(argc < 15){
		error("Not enough arguments. Use the -h flag for more info.\n");
		return 1;
	}
	if (argc == 16 && (eq_str(argv[15], "-d") || eq_str(argv[15], "-d"))) {
		debug = 1;
	}
	if(argc > 15 && !debug){
		error("Too many arguments. Use the -h flag for more info.\n");
		return 1;
	}
	int pc = 0;
	int cycles = 0;
	long long instruction;
	// The initial values of the local and hardware registers on reset are 0.
	int registers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned int io_registers[23] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	// initialize monitor array and set to 0's.
	unsigned char monitor[256 * 256];
	for (int k = 0; k < 256 * 256; k++) monitor[k] = 0;

	int local_memory[4096];
	int doom_counter = 0;
	unsigned int temp_leds = 0;
	unsigned int temp_7seg = 0; // temp variables to check if the leds or 7 seg changed
	int in_isr = 0; //if in ISR then 1, else 0.
	int irq = 0;
	char IOReg_name[20];

	FILE* mcode = fopen(argv[1], "r");
	fill_llarray_from_file(local_memory, argv[2]); // fills local memory from dmemin.txt
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
		error("Cant open file diskin. Crashing...");
		return 1;
	}

	int * disk_data = load_disk(disk_in_file);

	while(1){ // main run loop


		// ------- STAGE: Fetch -------
		instruction = fetch(mcode, pc);
		
		
		// ------- STAGE: Decode -------
		// takes long long, outputs int opcode, int[4] reg addresses, int[2] imm values
		// changes their values in decode as such: 
		// opcode is returned 
		// reg[0] = rd, reg[1] = rs, reg[2] = rt, reg[3] = rm
		// imm[0] = immediate 1, imm[1] = immediate 2
		int opcode, inst_regs[4], imm[2];
		opcode = decode(instruction, inst_regs, imm);
		registers[1] = imm[0];
        registers[2] = imm[1];
		// printf("DEBUG: INSTRUCTION - %012llX \n", instruction);

		

		// ------- STAGE: Traces -------
		trace_out(trace_file, pc, instruction, registers);

		
		if ((opcode == 19) || (opcode == 20)) {
			get_IO_reg_name(inst_regs, registers, IOReg_name);
			fprintf(hwregtrace_file, "%d ", cycles);
			if (opcode == 19){ 
				fprintf(hwregtrace_file, "READ ");// Read
			} else {
				fprintf(hwregtrace_file, "WRITE ");// Write
			}

			fprintf(hwregtrace_file, "%s ", IOReg_name);

			if (opcode == 19){
				fprintf(hwregtrace_file, "%08X\n", io_registers[registers[inst_regs[1]] + registers[inst_regs[2]]]); 
			} else { 
				fprintf(hwregtrace_file, "%08X\n", registers[inst_regs[3]]);
			}
		}
		// ------- STAGE: Execute -------
		if (execute(opcode, inst_regs, imm, registers, &pc, local_memory, io_registers, &in_isr, debug)){
			error("Error in execute\n");
			return 1;
		}

		
		// ------- STAGE: I/O -------
		
		if(temp_leds != io_registers[9]){
			fprintf(leds_file, "%d %08x\n", cycles, io_registers[9]);
		}
		if(temp_7seg != io_registers[10]){
			fprintf(disp7seg_file, "%d %08x\n", cycles, io_registers[10]);
		}
		temp_leds = io_registers[9];
		temp_7seg = io_registers[10];


		//if monitor command is 1 write to monitor array, data from I/O to adress given by I/O.
		if (io_registers[22]) {
			monitor[io_registers[20]] = io_registers[21];
			io_registers[22] = 0;
		}

		// doom counter is the counter for the clock cycles since calling the diskcmd
		if (io_registers[14]) {
			if (debug){
				printf("Instruction: %d\n", opcode);
				printf("Disk command: %d\n", io_registers[14]);
			} 
			if(execute_disk(io_registers, disk_data, local_memory)){
				error("Error in execute_disk\n");
				return 1;
			}
			doom_counter = 1024; 

		}
		if (doom_counter) {
			doom_counter--;
			if (!doom_counter) {
				io_registers[17] = 0;
				io_registers[3] = 1;
				// raise the interrupt
			}
		}

		// ------- STAGE: Interrupts -------

		// Timer
		if(io_registers[11]){
			if (io_registers[12] - io_registers[13]) {
				io_registers[3] = 0;
				io_registers[12]++;
			}else{
				io_registers[3] = 1;
				io_registers[12] = 0;
			}
		}

		// checks for Interrupt 2
		irq2_check(irq2in_file, cycles, io_registers);
		
		// executing interrupts
		irq = (io_registers[0] && io_registers[3]) || (io_registers[1] && io_registers[4]) || (io_registers[2] && io_registers[5]);
		if (irq & !in_isr) {
			io_registers[7] = pc + 1;
			pc = io_registers[6] - 1;
			in_isr = 1;
		}

		cycles++;
		io_registers[8] = cycles;
		// halt instruction
		if (opcode == HALT_OP || pc >= MAX_PC)
		{ 
			printf("Halted successfully!\n");
			break;
		}
		pc++;
	}
	
	// writing into regout.txt
	for (int i = 3; i < 16; i++)
	{
		fprintf(regout_file, "%08X\n", registers[i]);
	}

	// writing into cycles.txt
	fprintf(cycles_file, "%d", cycles);

	dmemout(local_memory, argv[5]);
	monitor_out(argv[13], argv[14], monitor); //13 - monitor.txt, 14 - monitor.yuv

	save_disk(disk_out_file, disk_data);
	fclose(disp7seg_file);
	fclose(leds_file);
	fclose(mcode);
	fclose(trace_file);
	fclose(regout_file);
	fclose(cycles_file);
	fclose(irq2in_file);
	fclose(hwregtrace_file);
	return 0;
}