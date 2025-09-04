#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// $s0 is the address of the current word the tape is at, and $t0 is the value. 
// $t1 is the last keyboard key pressed for the in command
// $gp is the global pointer, and points to the font data's starting point
// 
int init_code(char * next_block){
    char * init_asm = 
    R"(
#INIT CODE

add $sp, $imm1, $zero, $zero, 512, 0  # Initialize the stack pointer
lw $t0, $zero, $zero, $zero, 0, 0  # Load the word in mem[0] to the correct sector
out $zero, $imm1, $zero, $t0, 15, 0                          # read from sector $t0
mac $gp, $imm1, $imm2, $zero, 512, 8  # calculate memory address of 512*8 (4096)
out $zero, $imm1, $zero, $gp, 16, 0                      # r/w to/from memory addresses 512 * 8
out $zero, $imm1, $zero, $imm2, 14, 1                        # initiate disk read

LOOP:
    WAIT_0:
            in $t1, $imm1, $zero, $zero, 17, 0                       # Read interrupt irq0
            beq $zero, $t1, $imm1, $imm2, 1, WAIT_0                   # Pause here until done writing (interrupt raised)

    add $t0, $t0, $imm1, $zero, 1, 0  # increment sector
    out $zero, $imm1, $zero, $t0, 15, 0                          # read from sector $t0
    add $gp, $gp, $imm1, $zero, 16, 0  # increment memory address by 16
    out $zero, $imm1, $zero, $gp, 16, 0                      # r/w to/from memory addresses 512 * 8 + 16
    out $zero, $imm1, $zero, $imm2, 14, 1                        # initiate disk read
bne $zero, $t0, $imm1, $imm2, 6, LOOP  # if sector != 6, go to LOOP

mac $gp, $imm1, $imm2, $zero, 512, 8  # calculate memory address of 512*8 (last place where sector fits)
add $t0, $zero, $zero, $zero, 0, 0  # reset $t0 for program start
add $a1, $imm1, $zero, $zero, 0, 0
add $a2, $imm1, $zero, $zero, 0, 0
out $zero, $zero, $imm1, $imm2, 19, KEYEVENT # handle keyboard events for the in command
add $s0, $imm1, $zero, $zero, 2047, 0

)";
    strncpy(next_block, init_asm, strlen(init_asm) + 1);

    return 0;
}

int add(char * next_block, int count){
    snprintf(next_block, 150, "lw $t0, $s0, $zero, $zero, 0, 0  # load\n"
        "sw $t0, $s0, $zero, $imm1, %d, 0  # add and store\n"
        , count);
    return 0;
}

int sub(char * next_block, int count){
    snprintf(next_block, 150, "lw $t0, $s0, $zero, $zero, 0, 0  # load\n"
                     "sw $t0, $s0, $zero, $imm1, -%d, 0  # substract and store\n"
                     , count);
    return 0;
}

int right(char * next_block, int count){
    snprintf(next_block, 100, "add $s0, $s0, $imm1, $zero, %d, 0 # Move pointer to the left\n", count);
    return 0;
}

int left(char * next_block, int count){
    snprintf(next_block, 100, "add $s0, $s0, $imm1, $zero, -%d, 0 # Move pointer to the left\n", count);
    return 0;
}

int open_loop(char * next_block, int loop_idx){
    snprintf(next_block, 300, 
        "lw $t0, $s0, $zero, $zero, 0, 0  # load\n"
        "beq $zero, $t0, $zero, $imm2, 0, LOOP_END_%04d  # if $t0 == 0, go to LOOP_END_%04d\n"
        "LOOP_START_%04d: # Start of loop\n"
    , loop_idx, loop_idx, loop_idx);
    return 0;
}

int close_loop(char * next_block, int loop_idx){
    snprintf(next_block, 300, 
        "lw $t0, $s0, $zero, $zero, 0, 0  # load\n"
        "bne $zero, $t0, $zero, $imm2, 0, LOOP_START_%04d  # if $t0 != 0, go to LOOP_NE_%04d\n"
        "LOOP_END_%04d: # End of loop\n"
    , loop_idx, loop_idx, loop_idx);
    return 0;
}


int out(char * next_block){
    char * out_asm = "lw $a0, $s0, $zero, $zero, 0, 0  # load current byte\n"
                     "jal $ra, $zero, $zero, $imm2, 0, PRINT\n";
    strncpy(next_block, out_asm, strlen(out_asm) + 1);
    return 0;
}

int in(char * next_block, int in_dex){
    snprintf(next_block, 500, 
        "IN_%04d:\n"
        "    beq $zero, $t1, $zero, $imm2, 0, IN_%04d  # wait for the interrupt to be raised\n"
        "    bne $zero, $t1, $imm1, $imm2, 13, NOPE_%04d  # check if enter key was pressed\n"
        "    add $t1, $imm1, $zero, $zero, 10, 0  # $t1 = LF\n"
        "    NOPE_%04d:\n"
        "    sw $t1, $s0, $zero, $zero, 0, 0  # store the key event\n"
        "    add $t1, $zero, $zero, $zero, 0, 0  # reset $t1\n"
    , in_dex, in_dex, in_dex, in_dex);
    return 0;
}



int halt(char * next_block){
    char * halt_asm = "# PROGRAM END \n"
    "halt $zero, $zero, $zero, $zero, 0, 0 # Stop the program\n";
    strncpy(next_block, halt_asm, strlen(halt_asm) + 1);
    return 0;
}

int typeface(char * next_block){
    char * typeface_asm = R"(# LOAD TYPEFACE
.word 0 0  # at mem[0] i will store which sector is the typeface sector
.diskpage 0 0 0x00000000 0x02222020 0x05500000 0x0AFAAFA0
.diskpage 0 1 0x02763720 0x05122450 0x08B8CAC0 0x02200000
.diskpage 0 2 0x01222210 0x08444480 0x05250000 0x00027200
.diskpage 0 3 0x00000048 0x00007000 0x00000040 0x01122440

.diskpage 1 0 0x069BD960 0x02622270 0x069168F0 0x06921960
.diskpage 1 1 0x026AF220 0x0F8E1960 0x068E9960 0x0F124440
.diskpage 1 2 0x06969960 0x06997160 0x00020200 0x00020220
.diskpage 1 3 0x00124210 0x000E0E00 0x00842480 0x0E164040

.diskpage 2 0 0x069BB870 0x0699F990 0x0E9E99E0 0x06988960
.diskpage 2 1 0x0E9999E0 0x0F8E88F0 0x0F8E8880 0x0698B960
.diskpage 2 2 0x099F9990 0x07222270 0x01111960 0x09ACCA90
.diskpage 2 3 0x088888F0 0x09FF9990 0x09DB9990 0x06999960

.diskpage 3 0 0x0E99E880 0x06999A50 0x0699EA90 0x078611E0
.diskpage 3 1 0x07222220 0x09999960 0x09999660 0x0999FF90
.diskpage 3 2 0x09966990 0x05552220 0x0F1248F0 0x03222230
.diskpage 3 3 0x04422110 0x0C4444C0 0x04A00000 0x000000F0

.diskpage 4 0 0x04200000 0x00079970 0x088E99E0 0x00069870
.diskpage 4 1 0x01179970 0x0006F870 0x01272220 0x0007971E
.diskpage 4 2 0x088E9990 0x02022220 0x00101196 0x0889AE90
.diskpage 4 3 0x02222210 0x000EF990 0x000E9990 0x00069960

.diskpage 5 0 0x000E99E8 0x00079971 0x000E9880 0x0007C3E0
.diskpage 5 1 0x00272210 0x00099970 0x00099660 0x00099F70
.diskpage 5 2 0x00096690 0x0009971E 0x000F24F0 0x01262210
.diskpage 5 3 0x02222220 0x08464480 0x0005A000 0x00000000
                        
)";

    strncpy(next_block, typeface_asm, strlen(typeface_asm) + 1);
    return 0;
}

int print_proc(char * next_block){

    char * print_proc_asm = R"(

# PRINT PROCEDURE

PRINT:

    blt $zero, $a1, $imm1, $imm2, 250, NOPE_3  # Make sure were not at the end of the screen...
    add $a1, $zero, $zero, $zero, 0, 0 # Reset x
    add $a2, $a2, $imm1, $zero, 8, 0 # Next line

    NOPE_3:

    bne $zero, $a0, $imm1, $imm2, 13, NOPE_4  # Newline handling
    add $a1, $zero, $zero, $zero, 0, 0 # Reset x
    add $a2, $a2, $imm1, $zero, 8, 0 # Next line
    beq $zero, $zero, $zero, $ra, 0, 0  # return to caller

    NOPE_4:


    # Push everything to the stack
    add $sp, $sp, $imm1, $zero, -5, 0  
    sw $t0, $sp, $imm1, $zero, 0, 0
    sw $t1, $sp, $imm1, $zero, 1, 0
    sw $t2, $sp, $imm1, $zero, 2, 0
    sw $s0, $sp, $imm1, $zero, 3, 0
    sw $s1, $sp, $imm1, $zero, 4, 0

    bne $zero, $a0, $imm1, $imm2, 8, NOPE_5  # Backspace handling
    add $a1, $a1, $imm1, $zero, -5, 0 # Move x back
    add $s1, $zero, $zero, $zero, 0, 0 # set character to 0
    beq $zero, $zero, $zero, $imm2, 0, SKIP_5  # skip the loading of the character
    NOPE_5:

    add $s0, $a0, $imm1, $zero, -32, 0  # Handle ascii printable character offset of 32 chars
    lw $s1, $gp, $s0, $zero, 0, 0  # Load the character from memory

    SKIP_5:
    sll $t0, $imm1, $imm2, $zero, 1, 27  # set the bit mask = 0x08000000


    LOOP_WORD:
        add $t1, $imm1, $zero, $zero, 0, 0  # reset $t1
        and $t1, $t0, $s1, $imm1, -1, 0  # $t1 = $t0 & $s1   
        mac $t2, $a2, $imm1, $a1, 256, 0  # $t2 = $a1 + $a2*256 
        out $zero, $imm1, $zero, $t2, 20, 0  # set address
        beq $zero, $t1, $zero, $imm2, 0, NOPE_1  # if $t1 == 0, go to NOPE_1 (print a black pixel)

        add $t1, $imm1, $zero, $zero, 255, 0  # $t1 = 255 # set color to white

    NOPE_1:
        out $zero, $imm1, $zero, $t1 , 21, 0  # set color
        out $zero, $imm1, $zero, $imm2, 22, 1  # call interrupt 22
        srl $t0, $t0, $imm1, $zero, 1, 0  # $t0 = $t0 >> 1 mask shift

        add $a1, $a1, $imm1, $zero, 1, 0  # $a1++, next pixel


        # create mask $t2 = 0x08888888
        add $t2, $imm1, $zero, $zero, 0x008, 0 # $t2 = 0x00000008
        sll $t2, $t2, $imm1, $zero, 8, 0  # $t2 = 0x00000800
        add $t2, $imm1, $t2, $zero, 0x088, 0 # $t2 = 0x00000888
        sll $t2, $t2, $imm1, $zero, 8, 0  # $t2 = 0x00088800
        add $t2, $imm1, $t2, $zero, 0x088, 0 # $t2 = 0x00088888
        sll $t2, $t2, $imm1, $zero, 8, 0  # $t2 = 0x08888800
        add $t2, $imm1, $t2, $zero, 0x088, 0 # $t2 = 0x08888888

        and $t2, $t0, $t2, $imm1, -1, 0
        
        beq $zero, $t2, $zero, $imm2, 0, NOPE_2  # if $t0 != 0x08888888, go to NOPE_2 (Check if we finished a row of 4 pixels)

        add $a1, $a1, $imm1, $zero, -4, 0  # $a1 -= 4, return to the beginning of the line
        add $a2, $a2, $imm1, $zero, 1, 0  # $a2++, start new row

    NOPE_2:
        bne $zero, $t0, $imm1, $imm2, 0, LOOP_WORD  # if $t0 != 0, go to LOOP_WORD and print another bit to the screen


        # Go to the start of the next character
        add $a1, $a1, $imm1, $zero, 1, 0  # $a1++
        bne $zero, $a0, $imm1, $imm2, 8, NOPE_6  # Backspace handling again

        lw $zero, $a0, $zero, $zero, 0, 0  # debug
        add $a1, $a1, $imm1, $zero, -5, 0 # Move x back again
        

        NOPE_6:

        add $a2, $a2, $imm1, $zero, -6, 0  # $a2 -= 6

        # Pop the stack
        lw $t0, $sp, $imm1, $zero, 0, 0  # pop $t0
        lw $t1, $sp, $imm1, $zero, 1, 0  # pop $t1
        lw $t2, $sp, $imm1, $zero, 2, 0  # pop $t2
        lw $s0, $sp, $imm1, $zero, 3, 0  # pop $s0
        lw $s1, $sp, $imm1, $zero, 4, 0  # pop $s1
        add $sp, $sp, $imm1, $zero, 5, 0  # return the stack pointer to its original position
        beq $zero, $zero, $zero, $ra, 0, 0  # return to caller

# Keyboard interrupt handler

KEYEVENT:
    in $t1, $imm1, $zero, $zero, 18, 0  # read the key event
    reti $zero, $zero, $zero, $zero, 0, 0  # return to caller
    )";

    strncpy(next_block, print_proc_asm, strlen(print_proc_asm) + 1);
    return 0;
}