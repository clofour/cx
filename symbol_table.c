#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>

SymbolTable* create_symbol_table() {
    SymbolTable* symbol_table = (SymbolTable*) malloc(sizeof(SymbolTable));
    symbol_table->symbols = (Symbol*) malloc(sizeof(Symbol) * 100);
    symbol_table->symbol_count = 0;
    symbol_table->symbol_counts = (int*) malloc(sizeof(int) * 64);
    symbol_table->depth = 0;

    return symbol_table;
}

void free_symbol_table(SymbolTable* symbol_table) {
    free(symbol_table->symbols);
    free(symbol_table->symbol_counts);
    free(symbol_table);
}

void enter_scope(SymbolTable* symbol_table) {
    symbol_table->symbol_counts[symbol_table->depth] = symbol_table->symbol_count;
    symbol_table->depth++;
}

void exit_scope(SymbolTable* symbol_table) {
    symbol_table->depth--;
    symbol_table->symbol_count = symbol_table->symbol_counts[symbol_table->depth];
}

Symbol* symbol_lookup(SymbolTable* symbol_table, char *name)
{
    for (int i = symbol_table->symbol_count - 1; i >= 0; i--)
    {
        Symbol variable = symbol_table->symbols[i];

        if (strcmp(name, variable.name) == 0)
        {
            return &variable;
        }
    }

    return NULL;
}

int symbol_define(SymbolTable* symbol_table, char *name, ValueType type)
{
    Symbol symbol;
    symbol.name = name;
    symbol.type = type;
    symbol.offset = 8 * (symbol_table->symbol_count + 1);

    symbol_table->symbols[symbol_table->symbol_count] = symbol;
    symbol_table->symbol_count++;

    return symbol.offset;
}