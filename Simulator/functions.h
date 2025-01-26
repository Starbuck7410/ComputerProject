// That's just the function declerations for better aesthetics
void print_help();
void error(char * text);
void warn(char * text);
int eq_str(char str1[], char str2[]);
int execute(int op_code, int* inst_regs, int* imms, int* registers, int* P_PC, int* local_memory, unsigned int * io_registers, int * in_isr);
int decode(long long input, int regs[], int imm[]);
long long fetch(FILE* imemin_file, int PC);
int fill_llarray_from_file(int mem[], char* dmemin_file_path);
int dmemout(int * local_mem, char* arg5);
void execute_interrupt(unsigned int* io_registers, int* PC, int* in_isr);
int monitor_out(char* arg13, char* arg14, unsigned char monitor[]);
void trace_out(FILE* trace_file, int PC, long long inst, int registers[]);
void irq2_check(FILE* irq2in_file, int pc, int io_registers[]);
int get_IO_reg_name(int regs_array[], int registers[], char IOReg[]);
int execute_disk(unsigned int * io_registers, int * disk_file, int * local_memory);
int* load_disk(FILE * disk_file);
int save_disk(FILE * disk_file, int * disk_data);