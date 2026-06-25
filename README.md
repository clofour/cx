# cx

cx is a small programming language written in C. It contains a scanner, parser, printer and Windows x64 code generator.

## Quick Start

To use cx, you will need to:
1. Compile the program into assembly by running `cx <cx_input_file> <asm_output_file>`.

You can use the sample files in the `samples/` directory for testing if you wish.

Afterwards, you can optionally assemble the assembly files into portable executable format using NASM and MSVC link, like so:
1. Assemble the program by running `nasm -f win64 <asm_output_file> -o <obj_output_file>`.
2. Link necessary libraries by running `link <obj_output_file> /entry:main /subsystem:console /out:<exe_output_file> kernel32.lib msvcrt.lib ucrt.lib legacy_stdio_definitions.lib`.

## Knowledge Base

### Syntax