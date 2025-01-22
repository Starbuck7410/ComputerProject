#include <stdio.h>

long long hex_string_to_long_long();

int execute_disk(unsigned int * io_registers, FILE * disk_file, int * local_memory){
    unsigned int * disk_cmd = & io_registers[14];
    unsigned int * disk_sector = & io_registers[15];
    unsigned int * disk_buffer = & io_registers[16];
    unsigned int * disk_status = & io_registers[17];
    if (* disk_status){
        printf("Error writing to disk: disk busy\n");
        return 1;
    }
    if (* disk_cmd == 1){
        char line[9];
        printf("Reading sector %d from disk to bufffer at %d\n", *disk_sector, *disk_buffer);
        fseek(disk_file,(*disk_sector) * 512 * 9, SEEK_SET);
        for (int i = 0; i < 512; i++){
            if (fgets(line, 9, disk_file) != NULL) {
                // printf("%s (%d) -> %d\n", line, (int) hex_string_to_long_long(line, 8), (* disk_buffer)+i);
                local_memory[(* disk_buffer) + i] = (int) hex_string_to_long_long(line, 8); 
	        }
            else if (*disk_sector < 128){
                local_memory[(* disk_buffer) + i] = 0;
            }
            fseek(disk_file, 1, SEEK_CUR);
        }
        * disk_cmd = 0;
        * disk_status = 1;
        return 0;
    }
    if (* disk_cmd == 2){
        printf("Writing sector %d to disk from bufffer at %d\n", *disk_sector, *disk_buffer);
        // fseek(disk_file,(*disk_sector) * 512 * 9, SEEK_SET);
        // for (int i = 0; i < 512; i++){
        //     fprintf(disk_file, "%08lX", local_memory[(* disk_buffer) + i]);
        // }
        * disk_cmd = 0;
        * disk_status = 1;
        return 0;
    }
}