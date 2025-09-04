#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define INIT_ALLOC 64
#define MAX_MEM_SIZE 65536 // Max word count in memory

typedef struct mem_t {
    size_t length;
    size_t allocated;
    size_t max_size;
    int32_t * data;
} mem_t;

int mem_scale(mem_t *mem);
int mem_append(mem_t *mem, int32_t value);

int mem_write_idx(mem_t *mem, int32_t value, size_t idx);

int mem_write_file(mem_t *mem, FILE *file);

int mem_init(mem_t *mem, size_t max_size);

int mem_read_init_state(mem_t *mem, FILE *mem_file);

int read_int_array_from_file(int mem[], char* dmemin_file_path);

int32_t mem_read_idx(mem_t *mem, size_t idx);
