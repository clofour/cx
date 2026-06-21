#include "parser.h"

#ifndef PRINTER_H
#define PRINTER_H

typedef struct {
    int depth;
} Printer;

void print(Stmt* statements);

#endif