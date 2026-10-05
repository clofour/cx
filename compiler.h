#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
#include "dynamic_array.h"
#include "symbol_table.h"

typedef struct {
    SharedData* shared_data;
    AST ast;
    char* path;
    DynamicArray* data;
    DynamicArray* text;
    SymbolTable* symbol_table;
    int unique_counter;
} Compiler;

Compiler compiler_create(SharedData* shared_data, AST ast, char *path);
void compiler_free(Compiler* compiler);
void compile(Compiler* compiler);

#endif