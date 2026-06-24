#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
#include "dynamic_buffer.h"
#include "symbol_table.h"

typedef struct {
    char* path;
    DynamicBuffer* data;
    DynamicBuffer* text;
    SymbolTable* symbol_table;
    int unique_counter;
} Compiler;

void compile(AST ast, char* path);

#endif