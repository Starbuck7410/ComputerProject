# Computer ISA Project


## Structure

Assembler - contains the assembler source code
Simulator - contains the simulator source code
bf-sikp - contains the code for compiling brainfuck code to SIMP assembly
bin - contains binaries for linux & windows
assembly_programs - contains all assembly programs
Example - contains the fibonacci example files

## Usage

On Linux, you can simply point the `nass` script to an assembly file and it will compile and run it.
simply use

```bash
./nass /path/to/file.asm
```
and it will create build files, and automatically run the program.

To compile and run brainfuck code, use the -bf flag:

```bash
./nass -bf /path/to/file.bf
```

And to clean the build and run files, you can use the -c flag:

```bash
./nass  /path/to/file.asm -c
```

You can also use the -h flag for usage options.

On Windows, you will need to use the binary files in the `bin` folder as instructed in the documentation. Note these may not always be updated to the latest commit, but the build files I use are all uploaded here.

## Known bugs and future additions

### Add support for additional features:
 - Optimizations for the brainfuck interpreter
 - Unify dmem and imem into one big chunk of memory like in actual computers, which will allow you to store programs on disk and run them.
 - Think of a way to actually do that first
 - maybe make a basic compiler to allow for easier writing of code?