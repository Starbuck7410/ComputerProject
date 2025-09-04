#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "disk.h"
#include "functions.h"
#include "mem.h"
#include "instructions.h"

int load_disk(disk_t * disk){
    if (mem_init(disk->storage, MAX_DISK_SIZE)) return 1;
    if (mem_read_init_state(disk->storage, disk->file)) return 1;
    return 0;
}




int execute_disk(machine_state_t * machine_state, disk_t * disk){
    uint32_t * disk_cmd = & (machine_state->io_registers[14]);
    uint32_t * disk_sector = & (machine_state->io_registers[15]);
    uint32_t * disk_buffer = & (machine_state->io_registers[16]);
    uint32_t * disk_status = & (machine_state->io_registers[17]);

    if (* disk_status){
        error("Error writing to disk: disk busy\n");
        * disk_cmd = 0;
        return 1;
    }
    if (* disk_cmd == 1){
        for (int i = 0; i < 16; i++){ // 1 sector = 64 bytes = 16 words
            int32_t value = mem_read_idx(disk->storage, (* disk_sector) * 16 + i);
            mem_write_idx(machine_state->memory, value, (* disk_buffer) + i);
        }
        * disk_cmd = 0;
        * disk_status = 1;
        return 0;
    }
    if (* disk_cmd == 2){
        for (int i = 0; i < 16; i++){

            int32_t value = mem_read_idx(machine_state->memory, (* disk_buffer) + i);
            mem_write_idx(disk->storage, value, (* disk_sector) * 16 + i);
        }
        * disk_cmd = 0;
        * disk_status = 1;
        return 0;
    }
    return 0;
}
