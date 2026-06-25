# cx

cx is a small programming language written in C. It contains a scanner, parser, printer and Windows x64 code generator.

## Quick Start

To use cx, you will need to:
1. 

Afterwards, you can optionally assemble the assembly files into portable executable format using NASM and MSVC link, like so:
1. Run `nasm -f win64 output.asm -o output.obj`
2. Run `link output.obj /entry:main /subsystem:console /out:output.exe kernel32.lib msvcrt.lib ucrt.lib legacy_stdio_definitions.lib`.

## Knowledge Base

### Syntax