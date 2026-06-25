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

SymbolTable* symbol_table_create();
void symbol_table_free(SymbolTable* symbol_table);
void symbol_table_scope_enter(SymbolTable* symbol_table);
void symbol_table_scope_exit(SymbolTable* symbol_table);
Symbol* symbol_lookup(SymbolTable* symbol_table, char *name);
int symbol_define(SymbolTable* symbol_table, char *name, ValueType type);

#endif