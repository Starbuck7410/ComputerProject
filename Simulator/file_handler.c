#include <stdio.h>
#include "file_handler.h"

int files_load_from_args(files_t * files, char ** argv, int debug){


    FILE * mem_file = file_open(argv[1], "r");
    if (mem_file == NULL) { 
        return 1;
    }

	FILE * disk_file = file_open(argv[2], "w");
    if (disk_file == NULL) { 
        return 1;
    }
	
	FILE * irq2_file = file_open(argv[3], "r");
    if (irq2_file == NULL) { 
        return 1;
    }
	
    FILE * memout_file = NULL;
    FILE * regout_file = NULL;
    FILE * trace_file = NULL;
    FILE * hwregtrace_file = NULL;
    FILE * cycles_file = NULL;
	if (debug){
        memout_file = file_open(argv[4], "w");
        if (memout_file == NULL) { 
            return 1;
        }
		regout_file = file_open(argv[5], "w");
        if (regout_file == NULL) { 
            return 1;
        }
		trace_file = file_open(argv[6], "w");
        if (trace_file == NULL) { 
            return 1;
        }
		hwregtrace_file = file_open(argv[7], "w");
        if (hwregtrace_file == NULL) { 
            return 1;
        }
		cycles_file = file_open(argv[8], "w");
        if (cycles_file == NULL) { 
            return 1;
        }
	}
    (* files) = (files_t) {
        .memin = mem_file,
        .disk = disk_file,
        .irq2in = irq2_file,
        .memout = memout_file,
        .regout = regout_file,
        .trace = trace_file,
        .hwregtrace = hwregtrace_file,
        .cycles = cycles_file
    };
    return 0;
}

FILE * file_open(char * filename, char * mode){
    FILE * opened_file = fopen(filename, mode);
	if (opened_file == NULL){
		error("Failed to load file: ");
		printf("%s\n", filename);
		perror("");	
	}
    return opened_file;
}

int files_close(files_t * files){
    fclose((* files).memin);
    fclose((* files).disk);
    fclose((* files).irq2in);
    fclose((* files).memout);
    fclose((* files).regout);
    fclose((* files).trace);
    fclose((* files).hwregtrace);
    fclose((* files).cycles);
}