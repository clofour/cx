#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef enum {
    VALUE_NONE,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_BOOL
} ValueType;

typedef struct {
    char* name;
    ValueType type;
    int offset;
} Symbol;

typedef struct {
    Symbol* symbols;
    int symbol_count;
    int* symbol_counts;
    int depth;
} SymbolTable;

SymbolTable* create_symbol_table();
void free_symbol_table(SymbolTable* symbol_table);
void enter_scope(SymbolTable* symbol_table);
void exit_scope(SymbolTable* symbol_table);
Symbol* symbol_lookup(SymbolTable* symbol_table, char *name);
int symbol_define(SymbolTable* symbol_table, char *name, ValueType type);

#endif