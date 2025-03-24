# Computer Organization ISA Project

(This is a placeholder readme in the meantime)

## Structure

Assembler - contains the assembler source code
Simulator - contains the simulator source code
bin - contains binaries for linux & windows

assembly_programs - contains all assembly programs
Example - contains the fibonacci example files

## Usage

On Linux, you can simply point the `nass` script to an assembly file and it will compile and run it.
simply use

```bash
./nass /path/to/file.asm
```

and it will create build and run files.

On Windows, you will need to use the binary files in the `bin` folder as instructed in the instructions PDF. (later I will provide more info)

## Known bugs and future additions

### Add support for additional features:
 - Add a keyboard interrupt in two of the reserved hwregisters
 - Optimizations for the brainfuck interpreter
 - Unify dmem and imem into one big chunk of memory like in actual computers, which will allow you to store programs on disk and run them.
 - maybe even make a basic compiler to allow for easier writing of code?

### Publish and improve documentation
I do not want to publish the documentation as is, so I need to tell an LLM to transcribe it to a more readable markdown format, and then add my own features in the documentation.

### Bugs
I have made a critical mistake writing the disk drive system, and print functions:
- I have miscalculated the size of a sector in the drive, so now it is much smaller from the original instructions. I won't revert it to the original size, because it would warrant a large rewrite of the #.disksector meta-instruction, or will make it ridiculouslyw harder to use. I will instead change the specification to 64 bytes per block, and a delay of 512 clock cycles. #.disksector will be replaced with the similar-functionality #.diskpage which will take 2 location arguments (sector and page) and then 4 words to write a quarter of a sector on disk.
- The typeface for the print function used in the brainfuck interpreter and the new "terminal" program stores 1 byte for every word on disk and memory, which inflates the space and memory requirements by 3x! I will fix this issue before moving on to other things.

### TODO
 - Redo fonts
 - Convert PRINT to the new font system
 - Think of a way to load programs into this shit