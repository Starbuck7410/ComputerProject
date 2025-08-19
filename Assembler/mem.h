#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct mem_t {
    size_t length;
    size_t allocated;
    int32_t * data;
} mem_t;

int mem_scale(mem_t *mem);
int mem_append(mem_t *mem, int32_t value);
int mem_init(mem_t *mem);

int mem_write_idx(mem_t *mem, int32_t value, size_t idx);

int mem_write_file(mem_t *mem, FILE *file);


