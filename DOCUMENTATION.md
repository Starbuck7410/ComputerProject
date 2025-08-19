# Introduction
In this project we’ll exercise the subjects of ISA, Input/Output, as well as our skills in
the C language. We’ll implement an assembler and a simulator (separate programs),
and write programs in assembly language for a RISC processor named SIMP, which
is similar to the MIPS processor but simpler.

The simulator will simulate the SIMP processor, as well as several input/output
devices: leds, 7-segment-display, monochromatic monitor with 256x256 resolution,
and a disk drive. Every instruction in the processor is executed in one clock cycle.



# Registers

The SIMP processor includes 16 general-purpose registers, each 32 bits wide. The name, number and
role of each register according to the calling conventions is given in the following
table:

| Register number    | Register name | Purpose                      |
|:-------------------|:--------------|:-----------------------------|
|0                   | $zero         | Constant zero                |
|1                   | $imm1         | Sign extended immediate 1    |
|2                   | $imm2         | Sign extended immediate 2    |
|3                   | $v0           | Result value                 |
|4                   | $a0           | Argument register            |
|5                   | $a1           | Argument register            |
|6                   | $a2           | Argument register            |
|7                   | $t0           | Temporary register           |
|8                   | $t1           | Temporary register           |
|9                   | $t2           | Temporary register           |
|10                  | $s0           | Saved register               |
|11                  | $s1           | Saved register               |
|12                  | $s2           | Saved register               |
|13                  | $gp           | Global pointer (static data) |
|14                  | $sp           | Stack pointer                |
|15                  | $ra           | Return address               |



The register names and roles are similar to what we have seen in the lectures and
recitations for the MIPS processor, with one difference: the two registers $imm1,
$imm2 are special registers that can’t be written, and always contain the immediate
ields immediate1, immediate2 (respectively), after performing sign extension, as was
coded in the assembly instruction. The value in register 0 ($zero) by definition equals to 0.
Instructions that write to $zero, $imm1, $imm2 are legal, but don’t change their
values.

# Memory

The memory has a width of 32 bits and a depth of 64k lines (or 65536 lines, which is 4MiB). The PC
register is therefore 16 bits, and consecutive instructions are separated by a difference
of 2 in the PC. 
In contrast to the MIPS processor, the SIMP processor does not support byte or short. Every
access to the data memory reads or writes a 32-bit wide word.

# Intruction set and encoding
The SIMP processor has a single instruction format used to encode all instructions.
Every instruction is 64 bits wide, where the bit numbers of every field are given in the
following table:


| 63:56  | 55:52 | 51:48 | 47:44 | 43:40 | 39:20       | 19:0        |
|--------|-------|-------|-------|-------|-------------|-------------|
| opcode | rd    | rs    | rt    | rm    | immediate 1 | immediate 2 | 

The opcodes supported by the processor and the meaning of each instruction are given
in the following table:

| Opcode Number | Name | Meaning                                                      |
|---------------|------|--------------------------------------------------------------|
| 0             | add  | R[rd] = R[rs] + R[rt] + R[rm]                                |
| 1             | sub  | R[rd] = R[rs] - R[rt] - R[rm]                                |
| 2             | mac  | R[rd] = R[rs] * R[rt] + R[rm]                                |
| 3             | and  | R[rd] = R[rs] & R[rt] & R[rm]                                |
| 4             | or   | R[rd] = R[rs] \| R[rt] \| R[rm]                              |
| 5             | xor  | R[rd] = R[rs] ^ R[rt] ^ R[rm]                                |
| 6             | sll  | R[rd] = R[rs] << R[rt]                                       |
| 7             | sra  | R[rd] = R[rs] >> R[rt], arithmetic shift with sign extension |
| 8             | srl  | R[rd] = R[rs] >> R[rt], logical shift                        |
| 9             | beq  | if (R[rs] == R[rt]) pc = R[rm][low bits 15:0]                |
| 10            | bne  | if (R[rs] != R[rt]) pc = R[rm][low bits 15:0]                |
| 11            | blt  | if (R[rs] < R[rt]) pc = R[rm][low bits 15:0]                 |
| 12            | bgt  | if (R[rs] > R[rt]) pc = R[rm][low bits 15:0]                 |
| 13            | ble  | if (R[rs] <= R[rt]) pc = R[rm][low bits 15:0]                |
| 14            | bge  | if (R[rs] >= R[rt]) pc = R[rm][low bits 15:0]                |
| 15            | jal  | R[rd] = pc + 2 (next instruction address), pc = R[rm][15:0]  |
| 16            | lw   | R[rd] = MEM[R[rs] + R[rt]] + R[rm]                           |
| 17            | sw   | MEM[R[rs] + R[rt]] = R[rm] + R[rd]                           |
| 18            | reti | PC = IORegister[7]                                           |
| 19            | in   | R[rd] = IORegister[R[rs] + R[rt]]                            |
| 20            | out  | IORegister[R[rs] + R[rt]] = R[rm]                            |
| 21            | halt | Halt execution, exit simulator                               |
| 22            | copr | Will be used for co-processor instructions                   |



# Input/Output

The processor supports Input/Output using the in and out instructions, which access
an array of “hardware IO registers”, as detailed in the table below. The initial values
of the hardware registers on reset are 0.
 
| IORegister Number | Name          | Bits | Meaning                                                                   |
|-------------------|---------------|------|---------------------------------------------------------------------------|
| 0                 | irq0enable    | 1    | IRQ 0 enabled if set to 1, otherwise disabled                             |
| 1                 | irq1enable    | 1    | IRQ 1 enabled if set to 1, otherwise disabled                             |
| 2                 | irq2enable    | 1    | IRQ 2 enabled if set to 1, otherwise disabled                             |
| 3                 | irq0status    | 1    | IRQ 0 status. Set to 1 when IRQ 0 is triggered                            |
| 4                 | irq1status    | 1    | IRQ 1 status. Set to 1 when IRQ 1 is triggered                            |
| 5                 | irq2status    | 1    | IRQ 2 status. Set to 1 when IRQ 2 is triggered                            |
| 6                 | irqhandler    | 12   | PC of interrupt handler                                                   |
| 7                 | irqreturn     | 12   | PC of interrupt return address                                            |
| 8                 | clks          | 32   | Cyclic clock counter. Starts from 0 and increments every clock            |
| 9                 | reserved      | 32   | Reserved for future use                                                   |
| 10                | reserved      | 32   | Reserved for future use                                                   |
| 11                | timerenable   | 1    | 1: Timer enabled<br>0: Timer disabled                                     |
| 12                | timercurrent  | 32   | Current timer counter                                                     |
| 13                | timermax      | 32   | Max timer value                                                           |
| 14                | diskcmd       | 2    | 0 = No command<br>1 = Read sector<br>2 = Write sector                     |
| 15                | disksector    | 7    | Sector number, starting from 0.                                           |
| 16                | diskbuffer    | 12   | Memory address of buffer for read/write sector using DMA.                 |
| 17                | diskstatus    | 1    | 0 = Free to receive new command<br>1 = Busy handling a read/write command |
| 18                | keycode       | 8    | Keyboard character (in ASCII)                                             |
| 19                | keyhandler    | 12   | Keyboard handler address. If 0, the interrupt is disabled                 |
| 20                | monitoraddr   | 16   | Pixel address in frame buffer                                             |
| 21                | monitordata   | 8    | Pixel luminance (gray) value (0 – 255)                                    |
| 22                | monitorcmd    | 1    | 0 = No command<br>1 = Write pixel to monitor                              |
| 23                | keyboardevent | 1    | Keyboard interrupt register                                               |


# Interrupts

The SIMP processor supports 4 interrupts: irq0, irq1, irq2 and keyboardevent.

Interrupt 0 belongs to the timer, and the assemble code can program the timer to select
how often the interrupt will occur.

Interrupt 1 belongs to the simulated hard disk. Using interrupt 1 the disk notifies the
processer when it finished performing a read or write command.

Interrupt 2 is connected to an external (to the processor) interrupt line, irq2. An input
file to the simulator specifies when the interrupt occurs.

keyboardevent is connected to the keyboard, and is called whenever a key on the keyboard
is pressed. See more in the [keyboard section.](#keyboard)

In the clock cycle in which the interrupt is received, the processor turns on one of the
registers irq0status, irq1status, irq2status (respectively). In case several interrupt
requests arrive in the same clock cycle, multiple bits can be set to one simultaneously.

Every clock cycle, the processor computes and checks the logical signal:

``` irq = (irq0enable & irq0status) | (irq1enable & irq1status) | (irq2enable & irq2status) ```

In case ``` irq == 1``` , and provided the CPU is not currently inside the interrupt service
routine (ISR), the processor jumps to the ISR, whose address in memory is specified
in the hardware register irqhandler. That is to say, in this clock cycle the instruction at
address ```PC = irqhandler``` is executed instead of the instruction in the original PC. In
the same clock cycle, the original PC is saved in the hardware register irqreturn.
[No nested irq support] On the other hand, in case ```irq == 1``` but the CPU is still in the
ISR for the previous interrupt request (meaning still has not run the reti instruction),
the CPU will ignore the new irq, not jump to the ISR again, and continue to run the
code as usual in the old ISR (when the processor returns from the ISR, it’ll check irq
again and if set, will jump again to the ISR).
The assembly code of the ISR should check the bits of irqstatus, and after servicing
the interrupt, clear the bits for the interrupts that were serviced.
Return from the ISR happens using the reti instruction, that will set ```PC = irqreturn```.


# Timer

The SIMP processor supports a 32-bit timer, connected to interrupt irq0. It is enabled
when timerenable = 1.

The current value of the timer is stored in the hardware register timercurrent. In every
clock cycle in which the timer is enable, the timercurrent register is incremented by
one.

In the clock cycle in which timercurrent == timermax, the timer hardware sets
irqstatus0. In that clock cycle, instead of incrementing timercurrent, it is reset back to
zero.


# Monitor

The SIMP processor is connector to a gray monitor (can display only shades of
white/black, no color) with a resolution of 256x256 pixels. Every pixel is represented
by 8 bits which represent the luminance (gray level) of the pixel, where the value 0
corresponds to a black pixel, and the value 255 corresponds to a white pixel. Any
other value in-between represents a gray level between black and white, in a linear
fashion.
The monitor has an internal frame buffer with the size 256x256, which represents the
pixel values that are currently displayed. At the beginning the frame buffer contains
zeroes. The frame buffer contains lines of 256 bytes each that correspond to raster
scan of the display from the top to the bottom. Meaning line 0 in the buffer contains
the pixels of the top line in the display. In every line, the pixel scanning proceeds
from left to right.
Register monitoraddr contains the offset in the buffer of the pixel which the processor
wants to write.
Register monitordata contains the pixel value that the processor wishes to write.
Register monitorcmd is used to write a pixel. In the clock cycle in which a pixel is
written, an out command is being used to set ```monitorcmd = 1```, and then the pixel
value specified by monitordata is written to the display.
A read from monitorcmd using the in instruction will return the value 0.



# Disk Drive

The SIMP processor is connected to a disk drive of variable size, which is
composed of 64 byte (16 words)  sectors.
There isn't a required "partition table" or "file system" but for my personal convenience I will define SFS (Simp File System) later on in this document.

## Operation
The disk is connected to interrupt number 1, irq1, and uses DMA to copy the sector contents from memory to the disk or vice versa. The contents of the disk drive is given in the input file disk.txt, and the contents of the disk at the end of the simulation should be written to the same file. The assembly code can check that the disk drive is ready to receive a new command by checking the value of the hardware register diskstatus. Assuming the disk is ready, the assembly code writes to the hardware register disksector the sector number that we wish to read or write, and to the hardware register diskbuffer the memory address of the buffer in main memory. Once those two registers are initialized, a write or read command can be started by writing to the hardware register diskcmd. The service time of the disk drive for a read or write command is 512 clock cycles. During this time, the disk copies, using DMA, the contents of the memory buffer to the disk in case of a write command, or vice versa in case of a read command. As long as 512 clock cycles have not passed since receiving the command, diskstatus will mark that the disk is busy. After 512 clock cycles, in the same cycle diskcmd and diskstatus will be changed to the value 0 to mark that the disk is now free, and the disk will notify an interrupt by turning on irqstatus1.

## SFS
SFS is a primitive file system, based on the idea of linked lists. It's comprised of items, which can be files or directories, which include a reference to the next item, and to it's parent. 


An item starts with a header, which is 1 sectors, or 64 bytes.
the first 12 bytes are called the item head and store info about the item, like its type (file/directory), it's parent's start sector and it's own end sector. the following 52 bytes are for the name (ascii encoded). 

```
|XX|XX|XXXX|XXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|
 ^^ Type (Executable, folder, link...)
    ^^ Reserved for future use
       ^^^^ Parent item's head sector
            ^^^^ End sector
       Item name ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  
 
```
### Types:
- 0x00 - Null type (Empty space)
- 0x01 - Directory
- 0x02 - Text / data file
- 0x03 - Executable 
- 0x04 - Link
- ... Maybe more to come


# The Simulator

The simulator simulates the fetch-decode-execute loop. At the beginning of the run
PC = 0. Each iteration we bring the next instruction from address PC, decode the
instruction according to the encoding, and afterwards execute the instruction. At the
end of the instruction we update PC to the value PC + 1 (unless it was updated to
another value for example by a jump command or an interrupt). The simulation ends
and we exit the simulator once the HALT command is executed.

The simulator will be written in the C programming language and compiled into a
command line application that received 14 command line parameters in accordance
with the following command line:

```bash
./sim memin.txt disk.txt irq2in.txt [ memout.txt regout.txt trace.txt hwregtrace.txt cycles.txt debug_flag ]
```

## Ouput files


The file <b>memin.txt</b> is an input file in text format that contains the memory
contents at the start of the run. Every line in the file contains the contents of a line in
the data memory, starting from address 0, with a format of 8 hexadecimal letters. In
case the number of lines in the file is smaller than 64k, the assumption is that the rest
of the memory above the last address that was initialized in the file, is initialized to 0.
It can be assumed that the syntax of the input file is valid.

The file <b>disk.txt</b> is an input/output file in text format that contains the disk contents during the run.
Every line in the file contains the contents of a line in the disk,
starting from address 0, with a format of 8 hexadecimal letters (such that each sector
is 64 lines).

The file <b>irq2in.txt</b> is an input file in text format that contains the clock numbers in
which the external interrupt line 2, irq2, was raised to 1, each such cycle in a separate
line in ascending order. The interrupt line is set to 1 for a duration of a single clock
cycle, and then goes back to 0 (unless in the input file there is an additional line that
sets it to 1 in the next clock cycle as well).

The input files should be present even if in your assembly code they are not being
used (for example even for an assembly code that does not use the disk drive, an input
file diskin.txt should exist. You are allowed to leave its contents empty).

The file <b>memout.txt</b> is an output text file, in the same format as memin.txt, that
contains the contents of the memory at the end of the run.

### Debug files - These files will not be written if the debug flag isn't specified

The file <b>regout.txt</b> is an output text file, that contains the values of the registers R3-
R15 at the end of the run (note that the constants R0-R2 should not be printed in this
file). Every line will be written in 8 hexadecimal letters.

The file <b>trace.txt</b> is an output text file, that contains a line of text for every instruction
executed by the processor, in the following format:
```
PC INST R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 R10 R11 R12 R13 R14 R15
```

The file <b>hwregtrace.txt</b> is an output file that contains a line of text for each read or
write to a hardware register (using in and out instructions) in the following format:
```
CYCLE READ/WRITE NAME DATA
```

Where the field CYCLE is the clock cycle number in decimal.
The next field contains READ or WRITE depending on whether a read or write was
performed to the hardware register.
The field NAME contains the hardware register name as detailed in the table of I/O
registers.
The field DATA contains the value that was written or read in 8 hexadecimal digits.
The output file cycles.txt contains the number of clock cycles it took the program to
run.


## Monitor

The monitor is represented by an interactive window, that persists during the oprogram's operation,
and displays the framebuffer directly in grayscale.

## Keyboard

The keyboard is connected to the simulator through a framework of your choosing.
At every keypress, the keycode register will be set to the ASCII code of the character pressed,
and the keyboardevent interrupt will be raised.
When the keyboardevent interrupt is raised, and the keyhandler register is not set to 0, the
keyboard handler routine will be called to handle the keypress (add it to a buffer, print it to the screen, etc.)

# The Assembler

To be able to conveniently program the CPU and generate the initial memory images
in the files imemin.txt and dmemin.txt, we’ll also write an assembler. The assembler
will be written in the C programming language, and will program the assemble
program (written in text) into SIMP assembly language. It can be assumed that the
input file syntax is valid.
Similar to the simulator, the assembler is a command line application, with the
following run command line:

```bash
./asm program.asm memin.txt irq2in.txt disk.txt
```
<b>program.asm</b> is an input file containing the assembly program.

<b>memin.txt</b> is an output file containing the initial memory image.

<b>irq2in.txt</b> is an output file containing the interrupt times for interrupt 2, and 

<b>disk.txt</b> is an output file containing the initial state of the disk, and 

The output files of the assembler
are later used as input files for the simulator.


Each line of code in the assemble file contains all the 7 parameters in the encoding of
the instruction, where the first parameter is the opcode, and afterwards the parameters
are separated by comma. After the last parameters it is allowed to add the symbol #
and then add a comment on the right side, for example:

```asm
# opcode, rd, rs, rt, rm, imm1, imm2
add $t3, $t2, $t1, $t0, 0, 0 # $t3 = $t2 + $t1 + $t0
add $t1, $t1, $imm1, $zero, 2, 0 # $t1 = $t1 + 2
add $t1, $imm1, $imm1, $imm1, 2, 0 # $t1 = 2 + 2 + 2 = 6
sub $t1, $imm1, $imm2, $imm2, 2, 3 # $t1 = 2 – 3 – 3 = -4
```




In each instruction, there are 3 options for the immediate fields:
- A decimal number, positive or negative.
- A hexadecimal number that begins with 0x and then contains hexadecimal digits.
- A label, which is a symbolic name starting with a letter and ending with :


Examples:
```
   bne $zero, $t0, $t1, $imm1, L1, 0 # if ($t0 != $t1) goto L1
                                    # ($imm1 = address of L1)

   add $t2, $t2, $imm1, $zero, 1, 0 # $t2 = $t2 + 1 ($imm1 = 1)
   beq $zero, $zero, $zero, $imm1, L2, 0 # jump to L2 (reg1 = address L2)
L1:
   sub $t2, $t2, $imm1, $zero, 1, 0 # $t2 = $t2 – 1 ($imm1 = 1)
L2:
   add $t1, $zero, $imm1, $zero, L3, 0 # $t1 = address of L3
   beq $zero, $zero, $zero, $t1, 0, 0 # jump to the address specified in $t1
L3:
   jal $ra, $zero, $zero, $imm1, L4, 0 # function call L4, save return addr in $ra

L4:
   halt $zero, $zero, $zero, $zero, 0, 0 # halt execution
   beq $zero, $zero, $zero, $ra, 0, 0 # return from function in address in $ra
```


To support labels, the assembler performs two passes on the code. In the first pass we
remember the addresses of all labels, and in the second pass in each place that was a
use of a label in the immediate field, we replace it with the actual address of the label
as calculated in the first pass.

Take note of the use of the special registers $imm1, $imm2 and $zero in the various
instructions. For example the beq instruction in the example jumps in case zero is
equal to zero. This condition is always true and therefore it is a method to implement
unconditional jump.

## Directives

In addition to the instructions of the code, the assembler supports pseudo-instructions 
called directives that allows to set the contents of all the input files for the simulator, 
allowing assembly from one assembly file to a fully usable machine state.

### .word
The ```.word``` directive is used to set an initial condition for a word in memory.

Usage:
```
.word address data
```

Where address is the address of the word and data its value. Each one of the fields can
be in decimal, or hexadecimal when preceded with 0x, for example:
```
.word 256 1 # set MEM[256] = 1
.word 0x100 0x1234ABCD # MEM[0x100] = MEM[256] = 0x1234ABCD
```

### .diskpage
The diskpage directive is used to set the initial state of the disk from the assembly file.

Usage:
```
.diskpage sector page word1 word2 word3 word4
```

Each sector is divided into 4 pages (each page is 4 32 bit words long), numbered 0-3.
A sector can be set fully with 4 .diskpage directives or partially with less, with the
missing pages being full of 0s. For example, to set half a sector:

```
.diskpage 0 0 0x1234ABCD 0x11111111 0xB00B5    11
.diskpage 0 1 0xAAAA     0xF00D     0          -123
```


### .interrupt
The interrupt directive is used to set the clock numbers for which irq2 will be raised.
Usage:
```
.interrupt cycle
```

This directive only creates entries in the irq2in.txt file at assembly time.


## Additional assumptions

- Any level-headed assumption is welcome.
- The label format starts with a letter, and then all the letters and numbers are
allowed.
- Whitespaces such as space or tab or empty lines should be ignored.
- Hexadecimal numbers in the input file should be supported both in lower case
and upper case.
