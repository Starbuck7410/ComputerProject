#include <stdio.h>

typedef struct files_t {
    FILE * memin;
    FILE * disk;
    FILE * irq2in;
    // debugs
    FILE * memout;
    FILE * regout;
    FILE * trace;
    FILE * hwregtrace;
    FILE * cycles;
} files_t;

int files_load_from_args(files_t *files, char **argv, int debug);

int files_close(files_t *files);
