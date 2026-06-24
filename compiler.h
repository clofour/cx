#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
#include "dynamic_buffer.h"

typedef enum {
    VALUE_NONE,
    VALUE_NUMBER,
    VALUE_STRING
} ValueType;

typedef struct {
    char* name;
    int offset;
} Variable;

typedef struct {
    char* path;
    DynamicBuffer* data;
    DynamicBuffer* text;
    Variable* variables;
    int variableCount;
} Compiler;

void compile(AST ast, char* path);

#endif