// That's just the function declerations for better aesthetics
#include <X11/Xlib.h>
#include <X11/Xutil.h>

void print_help();
void error(char * text);
void warn(char * text);
int eq_str(char str1[], char str2[]);
void trace_out(FILE* trace_file, int PC, long long inst, int registers[]);
int32_t hex_string_to_int32(char number[], int len);
int get_IO_reg_name(int regs_array[], int registers[], char IOReg[]);
void error(char* text);
long long pow_int(int a, int b);
int keycode_to_ascii(XEvent event);
int irq2_load(FILE *irq2in_file, int *addresses);

