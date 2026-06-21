#ifndef COMPILER_H
#define COMPILER_H

typedef struct {
    char* path;
    int dataCapacity;
    int dataLength;
    char* data;
    int textCapacity;
    int textLength;
    char* text;
} Compiler;

#endif