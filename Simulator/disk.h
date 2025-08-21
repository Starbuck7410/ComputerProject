#include <stdio.h>
#include <stdint.h>
#include "mem.h"
#include "instructions.h"
#define MAX_DISK_SIZE 2097152

typedef struct disk_t {
    mem_t * storage;
    uint32_t busy_timeout;
    FILE * file;
} disk_t;

int load_disk(disk_t *disk);

int execute_disk(machine_state_t *machine_state, disk_t *disk);

int save_disk(disk_t *disk);
