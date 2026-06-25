#ifndef PRINTER_H
#define PRINTER_H

#include "parser.h"

typedef struct {
    SharedData* shared_data;
} Printer;

Printer printer_create(SharedData* shared_data);
void print(Printer* printer, AST ast);

#endif