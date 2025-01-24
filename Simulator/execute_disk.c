#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long long hex_string_to_long_long();
void error(char * text);

int* load_disk(FILE * disk_file){
    int * disk_data = (int*) malloc(512 * 128); // 512 sectors of 128 bytes
    char line[9];
    int i = 0;
    while (fscanf(disk_file, "%[^\n]\n", line) != EOF){
        disk_data[i] = (int) hex_string_to_long_long(line, 8);
        i++;
    }
    while (i < 512 * 128 / 4){
        disk_data[i] = 0;
        i++;
    }
    return disk_data;
}



int execute_disk(unsigned int * io_registers, int * disk_data, int * local_memory){
    unsigned int * disk_cmd = & io_registers[14];
    unsigned int * disk_sector = & io_registers[15];
    unsigned int * disk_buffer = & io_registers[16];
    unsigned int * disk_status = & io_registers[17];
    if (* disk_status){
        error("Error writing to disk: disk busy\n");
        * disk_cmd = 0;
        return 1;
    }
    if (* disk_cmd == 1){
        printf("Reading sector %d from disk to bufffer at %d\n", *disk_sector, *disk_buffer);

        for (int i = 0; i < 16; i++){
            // printf("%s (%d) -> %d\n", line, (int) hex_string_to_long_long(line, 8), (* disk_buffer)+i);
            local_memory[(* disk_buffer) + i] = disk_data[(* disk_sector) * 16 + i]; 
        }
        * disk_cmd = 0;
        * disk_status = 1;
        return 0;
    }
    if (* disk_cmd == 2){
        printf("Writing sector %d to disk from bufffer at %d\n", *disk_sector, *disk_buffer);
        for (int i = 0; i < 16; i++){
            // printf("%s (%d) -> %d\n", line, (int) hex_string_to_long_long(line, 8), (* disk_buffer)+i);
            disk_data[(* disk_sector) * 16 + i] = local_memory[(* disk_buffer) + i];
        }
        * disk_cmd = 0;
        * disk_status = 1;
        return 0;
    }
}

int save_disk(FILE * disk_file, int * disk_data){
    for (int i = 0; i < 512 * 128 / 4; i++){
        fprintf(disk_file, "%08X\n", disk_data[i]);
    }
    free(disk_data);
    fclose(disk_file);
    return 0;
}