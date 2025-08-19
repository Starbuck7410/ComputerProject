#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mem.h"

#define INIT_ALLOC 64
#define MAX_MEM_ALLOC 65536


int mem_scale(mem_t * mem){
    if((* mem).allocated * 2 > MAX_MEM_ALLOC){
        return 1;
    }
    int32_t * old_data = (* mem).data;

    (* mem).data = (int * ) malloc((* mem).allocated * 2 * sizeof(int32_t));
    if(!(* mem).data) return 1;
    memcpy((* mem).data, old_data, (* mem).allocated * sizeof(int32_t));

    free(old_data);
    (* mem).allocated *= 2;
    return 0;
}

int mem_append(mem_t * mem, int32_t value){
    return mem_write_idx(mem, value, (* mem).length);
}

int mem_init(mem_t * mem){
    if((* mem).data) free((* mem).data);
    (* mem).data = (int * ) malloc(INIT_ALLOC * sizeof(int32_t));
    if(!(* mem).data) return 1;

    (* mem).allocated = INIT_ALLOC;
    (* mem).length = 0;
    return 0;
}

int mem_write_idx(mem_t * mem, int32_t value, size_t idx){
    while ((* mem).allocated < idx){
        if(mem_scale(mem)) return 1;
    }
    (* mem).data[idx] = value;
    (* mem).length = ((* mem).length <= idx) ? idx + 1 : (* mem).length;
    return 0;
}

int mem_write_file(mem_t * mem, FILE * file){
    for (int i = 0; i < (* mem).length; i++){
        fprintf(file, "%08lX\n", (* mem).data[i]);
    }
    return 0;
}

int mem_clean(mem_t * mem){
    free((* mem).data);
}

int mem_read_init_state(mem_t * mem, FILE * mem_file){
    int i = 0;
	char data[9];
	while (fscanf(mem_file, "%[^\n]\n", data) != EOF && i < MAX_MEM_ALLOC) { //read a line from dmem and put it in 'data'
		mem_append(mem, (hex_string_to_int32(data, 8) & 0xFFFFFFFF)); //store it in memory as hex
		i++;
	}
	return 0;
}