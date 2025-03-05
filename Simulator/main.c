#define _CRT_SECURE_NO_WARNINGS
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "functions.h"

#define HALT_OP 21
#define MAX_PC 4096
#define IRQ_SIZE 100
// argv[0] = sim.exe,      argv[1] = imemin.txt,   argv[2] = dmemin.txt
// argv[3] = diskin.txt,   argv[4] = irq2in.txt,   argv[5] = dmemout.txt
// argv[6] = regout.txt,   argv[7] = trace.txt,    argv[8] = hwregtrace.txt
// argv[9] = cycles.txt,  argv[10] = leds.txt,    argv[11] =  display7seg.txt 
// argv[12] = diskout.txt, argv[13] = monitor.txt, argv[14] = monitor.yuv


int scale = 3;
XImage * image;
Display * display;
Window window;
int screen;
int slow = 30000; // larger values run faster but are choppier

char * create_screen(int size_x, int size_y) {
    // Connect to the X server
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Error: Cannot open X display\n");
        return NULL;
    }

    screen = DefaultScreen(display);

    // Get screen depth (bits per pixel)
    int depth = DefaultDepth(display, screen);
    printf("Screen Depth: %d bits per pixel\n", depth);

    // Ensure we are working with 32-bit color (ARGB)
    if (depth != 24 && depth != 32) {
        fprintf(stderr, "Unsupported depth: %d\n", depth);
        return NULL;
    }

    // Create a window
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                        10, 10, size_x * scale, size_y * scale , 1,
                                        BlackPixel(display, screen),
                                        WhitePixel(display, screen));

    XStoreName(display, window, "Screen");
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    // Allocate memory for the image
    size_t bytes_per_pixel = 4;
    size_t image_size = size_x * scale * size_y * scale * bytes_per_pixel;

    char *image_data = malloc(image_size);
    if (!image_data) {
        fprintf(stderr, "Error: Failed to allocate memory\n");
        return NULL;
    }

    // Create an XImage
    image = XCreateImage(display, DefaultVisual(display, screen),
                                 depth, ZPixmap, 0,
                                 image_data, size_x * scale, size_y * scale,
                                 32, 0);

    if (!image) {
        fprintf(stderr, "Error: Failed to create XImage\n");
        free(image_data);
        return NULL;
    }
    return image_data;
}

int main(int argc, char * argv[]) {
	int debug = 0;
	if(argc == 2 && (eq_str(argv[1], "-h") || eq_str(argv[1], "-H"))){
		print_help();
		return 0;
	}

	if(argc < 13){
		error("Not enough arguments. Use the -h flag for more info.\n");
		return 1;
	}
	if (argc == 14 && (eq_str(argv[13], "-d") || eq_str(argv[13], "-D"))) {
		debug = 1;
	}
	if(argc > 13 && !debug){
		error("Too many arguments. Use the -h flag for more info.\n");
		return 1;
	}
	int pc = 0;
	int cycles = 0;
	long long instruction;
	// The initial values of the local and hardware registers on reset are 0.
	// I thus declare register 18 to be the keyboard character, register 24 to be the interrupt itself, and register 19 to be the keyboard handler
	int registers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned int io_registers[24] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	
	long long imem[4096];
	int local_memory[4096];
	int irq2_addresses[IRQ_SIZE];
	int doom_counter = 0;
	unsigned int temp_leds = 0;
	unsigned int temp_7seg = 0; // temp variables to check if the leds or 7 seg changed
	int in_isr = 0; //if in ISR then 1, else 0.
	int irq = 0;
	char IOReg_name[20];

	fill_ll_array_from_file(imem, argv[1]); // fills local memory from dmemin.txt
	fill_int_array_from_file(local_memory, argv[2]); // fills local memory from dmemin.txt
	FILE* disk_in_file = fopen(argv[3], "r");
	FILE* irq2in_file = fopen(argv[4], "r");
	irq2_load(argv[4], irq2_addresses);
	// dmemout 5 handled in its own function
	FILE* regout_file = fopen(argv[6], "w");
	FILE* trace_file = fopen(argv[7], "w");
	FILE* hwregtrace_file = fopen(argv[8], "w");
	FILE* cycles_file = fopen(argv[9], "w");
	FILE* leds_file = fopen(argv[10], "w");
	FILE* disp7seg_file = fopen(argv[11], "w");
	FILE* disk_out_file = fopen(argv[12], "w");

	

	if (disk_in_file == NULL){
		perror("ERROR ");
		error("Cant open file diskin. Crashing...");
		return 1;
	}

	int * disk_data = load_disk(disk_in_file);

	int size_x = 256;
    int size_y = 256;
    char * monitor = create_screen(size_x, size_y);
	// initialize monitor array and set to 0's.
	for (int k = 0; k < 256 * 256 * 4 * scale * scale; k++) monitor[k] = 0;
	XEvent event;
	XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, size_x * scale, size_y * scale);


	while(1){ // main run loop
		// usleep(slow);

		// ------- STAGE: Fetch -------

		instruction = imem[pc];
		
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

		

		// ------- STAGE: Traces -------
		if(debug){
				trace_out(trace_file, pc, instruction, registers);
			
			
			if ((opcode == 19) || (opcode == 20)) {
				get_IO_reg_name(inst_regs, registers, IOReg_name);
				fprintf(hwregtrace_file, "%d ", cycles);
				if (opcode == 19){ 
					fprintf(hwregtrace_file, "READ "); // Read
				} else {
					fprintf(hwregtrace_file, "WRITE "); // Write
				}

				fprintf(hwregtrace_file, "%s ", IOReg_name);

				if (opcode == 19){
					fprintf(hwregtrace_file, "%08X\n", io_registers[registers[inst_regs[1]] + registers[inst_regs[2]]]); 
				} else { 
					fprintf(hwregtrace_file, "%08X\n", registers[inst_regs[3]]);
				}
			}
		}
		// ------- STAGE: Execute -------
		if (execute(opcode, inst_regs, imm, registers, &pc, local_memory, io_registers, &in_isr, debug)){
			error("Error in execute\n");
			return 1;
		}

		
		// ------- STAGE: I/O -------
		if(debug){
			if(temp_leds != io_registers[9]){
				fprintf(leds_file, "%d %08x\n", cycles, io_registers[9]);
			}
			if(temp_7seg != io_registers[10]){
				fprintf(disp7seg_file, "%d %08x\n", cycles, io_registers[10]);
			}
			temp_leds = io_registers[9];
			temp_7seg = io_registers[10];

		}


		// if monitor command is 1 write to monitor array, data from I/O to adress given by I/O.
		if (io_registers[22]) {
			int x = io_registers[20] % 256;
			int y = io_registers[20] / 256;
			
			char * monitor_ptr = monitor + scale * (x + y * 256 * scale) * 4;
			for(int i = 0; i < scale * scale; i++){
				int xscale = i % scale * 4;
				int yscale = (i / scale) * 256 * scale * 4;
				* (monitor_ptr + xscale + yscale) = io_registers[21] & 0xFF;
				* (monitor_ptr + xscale + yscale + 1) = io_registers[21] & 0xFF;
				* (monitor_ptr + xscale + yscale + 2)  = io_registers[21] & 0xFF;

				// * (monitor_ptr + xscale + yscale) =  0xFF;
				// * (monitor_ptr + xscale + yscale + 1) =  0xFF;
				// * (monitor_ptr + xscale + yscale + 2)  = 0xFF;
			}
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
		
		if(io_registers[24]){
			io_registers[24] = 0;
		}

		if(cycles % slow == 0){
			XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, size_x * scale, size_y * scale);	
			if(XCheckWindowEvent(display, window, KeyPressMask, &event)){
				if (event.type == KeyPress) {
					io_registers[18] = keycode_to_ascii(event);
					if(io_registers[18] == 27){
						break;
					}
					if(io_registers[18] != 0){
						io_registers[24] = 1;
					}
					if(debug){
						printf("Key pressed: %d\n", io_registers[18]);
					}
				}
			}
		}

		if(io_registers[24] == 1 && !in_isr){ // keyboard pressed
			if(io_registers[19]){
				io_registers[7] = pc + 1;
				pc = io_registers[19] - 1;
				in_isr = 1;
			}
			
		}
		


		// checks for Interrupt 2
		if (io_registers[5] == 1) io_registers[5] = 0; //turn off irq2 if irq2 was on last cycles.  

		for(int i = 0; i < IRQ_SIZE; i++){
			if (irq2_addresses[i] == cycles) {
				io_registers[5] = 1; //irq2status = 1
			}
		}
		
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
	
	if(debug){
		// writing into regout.txt
		for (int i = 3; i < 16; i++){
			fprintf(regout_file, "%08X\n", registers[i]);
		}

	}
	// * (monitor + 6) = 250;
	// writing into cycles.txt
	fprintf(cycles_file, "%d", cycles);

	while(1 && io_registers[18] != 27){
		XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, size_x * scale, size_y * scale);
		
		XNextEvent(display, &event);
		if (event.type == KeyPress) {
			break;
		}
	}
	XDestroyImage(image); // Also frees image_data
    XDestroyWindow(display, window);
    XCloseDisplay(display);

	dmemout(local_memory, argv[5]);
	save_disk(disk_out_file, disk_data);
	fclose(disp7seg_file);
	fclose(leds_file);
	fclose(trace_file);
	fclose(regout_file);
	fclose(cycles_file);
	fclose(irq2in_file);
	fclose(hwregtrace_file);
	return 0;
}