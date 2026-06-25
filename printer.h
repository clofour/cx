#ifndef PRINTER_H
#define PRINTER_H

#include "parser.h"

typedef struct {
    int depth;
} Printer;

void print(AST ast);

#endif