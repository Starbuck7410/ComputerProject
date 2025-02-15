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

## Future additions

### Add support for additional features:
 - "puts" assembly function to print text onto the screen for brainfuck
 - Add a keyboard interrupt in two of the reserved hwregisters

### Publish and improve documentation
I do not want to publish the documentation as is, so I need to tell an LLM to transcribe it to a more readable markdown format, and then add my own features in the documentation.