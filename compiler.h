#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
#include "dynamic_buffer.h"

typedef struct {
    char* path;
    DynamicBuffer data;
    DynamicBuffer text;
} Compiler;

void compile(AST ast, char* path);

#endif