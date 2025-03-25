// Those are just the function declerations
long long find_instruction();
long long find_register();
long long pow_int(int a, int b);
int eq_str(char str1[], char str2[]);
int dec_string_to_int();
int hex_string_to_int();
int str_to_int(char * text);
void error(char * text);
void warn(char * text);
void print_help();
long long get_component(char * line, char * component, int start);
int fpeek(FILE *stream);
int clean_string(char * line);