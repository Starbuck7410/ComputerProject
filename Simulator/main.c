#define _CRT_SECURE_NO_WARNINGS
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mem.h"
#include "file_handler.h"
#include "functions.h"

#define HALT_OP 21
#define MAX_PC 4096
#define IRQ_SIZE 100
// argv[0] = sim.exe,      argv[1] = memin.txt,      argv[2] = disk.txt,   
// argv[3] = irq2in.txt,   argv[4] = memout.txt      argv[5] = regout.txt, 
// argv[6] = trace.txt,    argv[7] = hwregtrace.txt  argv[8] = cycles.txt,
// argv[9] = debug flag



int scale = 3;
XImage * image;
Display * display;
Window window;
int screen;
int slow = 30000; // larger values run faster but are choppier, default is 30000

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
	if(argc == 2 && (eq_str(argv[1], "-h"))){
		print_help();
		return 0;
	}

	
	if (argc == 10 && (eq_str(argv[9], "-d") || eq_str(argv[12], "--debug"))) {
		debug = 1;
	}

	if (argc != 4 && !debug){
		error("Wrong number of arguments. Use the -h flag for more info.\n");
	}

	files_t files;
	if(files_load_from_args(&files, argv, debug)) return 1;

	size_t pc = 0;
	int64_t cycles = 0;
	int64_t instruction;
	// The initial values of the local and IO registers on reset are 0.
	int32_t registers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	uint32_t io_registers[24] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	
	
	mem_t memory;
	mem_init(&memory);
	int irq2_addresses[IRQ_SIZE];
	int hdd_doom_counter = 0;
	int in_isr = 0; //if in ISR then 1, else 0.
	int irq = 0;
	char IOReg_name[20];


	
	mem_read_init_state(&memory, files.memin);
	irq2_load(files.irq2in, irq2_addresses);

	int * disk_data = load_disk(files.disk);

	int size_x = 256;
    int size_y = 256;
    char * monitor = create_screen(size_x, size_y);
	// initialize monitor array and set to 0's.
	for (int k = 0; k < 256 * 256 * 4 * scale * scale; k++) monitor[k] = 0;
	XEvent event;
	XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, size_x * scale, size_y * scale);


	while(1){ // main run loop

		// ------- STAGE: Fetch -------

		instruction = memory.data[pc] << 32 | memory.data[pc + 1];
		
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
				trace_out(files.trace, pc, instruction, registers);
			
			
			if ((opcode == 19) || (opcode == 20)) {
				get_IO_reg_name(inst_regs, registers, IOReg_name);
				fprintf(files.hwregtrace, "%d ", cycles);
				if (opcode == 19){ 
					fprintf(files.hwregtrace, "READ "); // Read
				} else {
					fprintf(files.hwregtrace, "WRITE "); // Write
				}

				fprintf(files.hwregtrace, "%s ", IOReg_name);

				if (opcode == 19){
					fprintf(files.hwregtrace, "%08X\n", io_registers[registers[inst_regs[1]] + registers[inst_regs[2]]]); 
				} else { 
					fprintf(files.hwregtrace, "%08X\n", registers[inst_regs[3]]);
				}
			}
		}
		// ------- STAGE: Execute -------
		if (execute(opcode, inst_regs, imm, registers, &pc, local_memory, io_registers, &in_isr, debug)){
			error("Error in execute\n");
			return 1;
		}

		
		// ------- STAGE: I/O -------

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
			doom_counter = 512; 

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
		
		if(io_registers[23]){
			io_registers[23] = 0;
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
						io_registers[23] = 1;
					}
					if(debug){
						printf("Key pressed: %d\n", io_registers[18]);
					}
				}
			}
		}

		if(io_registers[23] == 1 && !in_isr){ // keyboard pressed
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
	save_disk(disk_file, disk_data);
	fclose(disp7seg_file);
	fclose(leds_file);
	fclose(trace_file);
	fclose(regout_file);
	fclose(cycles_file);

	fclose(hwregtrace_file);
	return 0;
}