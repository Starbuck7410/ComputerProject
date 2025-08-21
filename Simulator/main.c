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
#include "instructions.h"
#include "disk.h"

#define HALT_OP 21
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
int slow = 10000; // larger values run faster but are choppier, default is 30000

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

	
	if (argc == 10 && (eq_str(argv[9], "-d") || eq_str(argv[9], "--debug"))) {
		debug = 1;
	}

	if (argc != 4 && !debug){
		error("Wrong number of arguments. Use the -h flag for more info.\n");
		return 1;
	}

	files_t files;
	if(files_load_from_args(&files, argv, debug)) return 1;

	int64_t cycles = 0;
	int64_t instruction_code;



	
	mem_t memory;
	mem_init(&memory, MAX_MEM_SIZE);
	machine_state_t machine_state = { 0 };
	machine_state.memory = &memory;
	
	mem_t storage;
	disk_t disk = { 0 };
	disk.storage = &storage;
	disk.file = files.disk;
	load_disk(&disk);

	int irq2_addresses[IRQ_SIZE];
	int irq = 0;
	char IOReg_name[20];


	
	mem_read_init_state(&memory, files.memin);
	irq2_load(files.irq2in, irq2_addresses);


	int size_x = 256;
    int size_y = 256;
    char * monitor = create_screen(size_x, size_y);
	// initialize monitor array and set to 0's.
	for (int k = 0; k < 256 * 256 * 4 * scale * scale; k++) monitor[k] = 0;
	XEvent event;
	XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, size_x * scale, size_y * scale);


	while(1){ // main run loop

		// ------- STAGE: Fetch -------

		instruction_code = (uint64_t) memory.data[machine_state.PC] << 32 | memory.data[machine_state.PC + 1];
		
		// ------- STAGE: Decode -------

		instruction_t instruction = { 0 };
		decode_instruction(instruction_code, &instruction);
		
		machine_state.registers[1] = instruction.immediates[0];
        machine_state.registers[2] = instruction.immediates[1];


		// ------- STAGE: Traces -------
		if(debug){
				trace_out(files.trace, machine_state.PC, instruction_code, machine_state.registers);
			
			
			if ((instruction.opcode == 19) || (instruction.opcode == 20)) {
				get_IO_reg_name(instruction.registers, machine_state.registers, IOReg_name);
				fprintf(files.hwregtrace, "%d ", cycles);
				if (instruction.opcode == 19){ 
					fprintf(files.hwregtrace, "READ "); // Read
				} else {
					fprintf(files.hwregtrace, "WRITE "); // Write
				}

				fprintf(files.hwregtrace, "%s ", IOReg_name);

				if (instruction.opcode == 19){
					fprintf(files.hwregtrace, "%08X\n", machine_state.io_registers[
						machine_state.registers[instruction.registers[1]] + machine_state.registers[instruction.registers[2]]
					]); 
				} else { 
					fprintf(files.hwregtrace, "%08X\n", machine_state.registers[instruction.registers[3]]);
				}
			}
		}
		// ------- STAGE: Execute -------
		if (execute_instruction(instruction, &machine_state, debug)){
			error("Error in execute\n");
			return 1;
		}

		
		// ------- STAGE: I/O -------

		// if monitor command is 1 write to monitor array, data from I/O to adress given by I/O.
		if (machine_state.io_registers[22]) {
			int x = machine_state.io_registers[20] % 256;
			int y = machine_state.io_registers[20] / 256;
			
			char * monitor_ptr = monitor + scale * (x + y * 256 * scale) * 4;
			for(int i = 0; i < scale * scale; i++){
				int xscale = i % scale * 4;
				int yscale = (i / scale) * 256 * scale * 4;
				* (monitor_ptr + xscale + yscale) = machine_state.io_registers[21] & 0xFF;
				* (monitor_ptr + xscale + yscale + 1) = machine_state.io_registers[21] & 0xFF;
				* (monitor_ptr + xscale + yscale + 2)  = machine_state.io_registers[21] & 0xFF;
			}
			machine_state.io_registers[22] = 0;
		}
		
		

		

		// doom counter is the counter for the clock cycles since calling the diskcmd
		if (machine_state.io_registers[14]) {
			if (debug){
				printf("Instruction: %d\n", instruction.opcode);
				printf("Disk command: %d\n", machine_state.io_registers[14]);
			} 
			if(execute_disk(&machine_state, &disk)){
				error("Error in execute_disk\n");
				return 1;
			}
			disk.busy_timeout = 512; 

		}
		if (disk.busy_timeout) {
			disk.busy_timeout--;
			if (!disk.busy_timeout) {
				machine_state.io_registers[17] = 0;
				machine_state.io_registers[3] = 1;
				// raise the interrupt
			}
		}

		// ------- STAGE: Interrupts -------

		// Timer
		if(machine_state.io_registers[11]){
			if (machine_state.io_registers[12] - machine_state.io_registers[13]) {
				machine_state.io_registers[3] = 0;
				machine_state.io_registers[12]++;
			}else{
				machine_state.io_registers[3] = 1;
				machine_state.io_registers[12] = 0;
				
			}
		}
		
		if(machine_state.io_registers[23]){
			machine_state.io_registers[23] = 0;
		}

		if(cycles % slow == 0){
			XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, size_x * scale, size_y * scale);	
			if(XCheckWindowEvent(display, window, KeyPressMask, &event)){
				if (event.type == KeyPress) {
					machine_state.io_registers[18] = keycode_to_ascii(event);
					if(machine_state.io_registers[18] == 27){
						break;
					}
					if(machine_state.io_registers[18] != 0){
						machine_state.io_registers[23] = 1;
					}
					if(debug){
						printf("Key pressed: %d\n", machine_state.io_registers[18]);
					}
				}
			}
		}

		if(machine_state.io_registers[23] == 1 && !machine_state.in_isr){ // keyboard pressed
			if(machine_state.io_registers[19] > 1){
				machine_state.io_registers[7] = machine_state.PC + 2;
				machine_state.PC = machine_state.io_registers[19] - 2;
				machine_state.in_isr = 1;
			}
			
		}
		


		// checks for Interrupt 2
		if (machine_state.io_registers[5] == 1) machine_state.io_registers[5] = 0; //turn off irq2 if irq2 was on last cycles.  

		for(int i = 0; i < IRQ_SIZE; i++){
			if (irq2_addresses[i] == cycles) {
				machine_state.io_registers[5] = 1; //irq2status = 1
			}
		}
		
		// executing interrupts
		irq = (machine_state.io_registers[0] && machine_state.io_registers[3]) 
		   || (machine_state.io_registers[1] && machine_state.io_registers[4]) 
		   || (machine_state.io_registers[2] && machine_state.io_registers[5]);

		if (irq && !machine_state.in_isr) {
			printf("It's Morbin' time\n");
			machine_state.io_registers[7] = machine_state.PC + 2;
			machine_state.PC = machine_state.io_registers[6] - 2;
			machine_state.in_isr = 1;
		}

		cycles++;
		machine_state.io_registers[8] = cycles;
		// halt instruction
		if (instruction.opcode == HALT_OP || machine_state.PC >= machine_state.memory->max_size){ 	
			printf("Opcode: %d | PC: %d | machine_state.io_registers[6]: %d\n", instruction.opcode, machine_state.PC, machine_state.io_registers[6]);
			printf("Halted successfully!\n");
			break;
		}
		machine_state.PC += 2;
	}
	
	if(debug){
		// writing into regout.txt
		for (int i = 3; i < 16; i++){
			fprintf(files.regout, "%08X\n", machine_state.registers[i]);
		}
		// writing into cycles.txt
		fprintf(files.cycles, "%llu", cycles);
		mem_write_file(machine_state.memory, files.memout);
	}


	while(machine_state.io_registers[18] != 27){
		XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, size_x * scale, size_y * scale);
		
		XNextEvent(display, &event);
		if (event.type == KeyPress) {
			break;
		}
	}
	XDestroyImage(image); // Also frees image_data
    XDestroyWindow(display, window);
    XCloseDisplay(display);
	
	
	mem_write_file(disk.storage, disk.file);
	free(machine_state.memory->data);
	free(disk.storage->data);
	
	files_close(&files, debug);
	return 0;
}