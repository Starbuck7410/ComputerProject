#include <stdint.h>
#include "mem.h"


typedef struct machine_state_t {
    int32_t registers[16];
    size_t PC;
    mem_t * memory;
    uint32_t io_registers[24];
    int in_isr;
} machine_state_t;

typedef struct instruction_t {
    int32_t opcode;
    int registers[4];
    int32_t immediates[2];
} instruction_t;

int execute_instruction(instruction_t instruction, machine_state_t *machine_state, int debug);
void decode_instruction(uint64_t input, instruction_t *instruction);

uint64_t pack_uint32(uint32_t hi, uint32_t lo);
