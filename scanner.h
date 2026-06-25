#ifndef SCANNER_H
#define SCANNER_H

#include "reader.h"

typedef enum TokenType
{
    TOKEN_NONE,

    TOKEN_LEFT_PARENTHESIS,
    TOKEN_RIGHT_PARENTHESIS,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_SLASH,
    TOKEN_MODULO,
    TOKEN_STAR,

    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,

    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,

    TOKEN_AND,
    TOKEN_CLASS,
    TOKEN_ELSE,
    TOKEN_FALSE,
    TOKEN_FUNCTION,
    TOKEN_FOR,
    TOKEN_IF,
    TOKEN_NULL,
    TOKEN_OR,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_TRUE,
    TOKEN_VAR,
    TOKEN_WHILE,

    TOKEN_EOF
} TokenType;

typedef struct {
    char* keyword;
    TokenType type;
} Keyword;

typedef struct
{
    TokenType type;

    char *start;
    int length;
    int line;
    int column;

    union
    {
        long int_value;
        double float_value;
        char* string_value;
        char* identifier_value;
    } value;
} Token;

typedef struct {
    char* source;
    int source_length;
    int line;
    int start;
    int current;
    Token* tokens;
    int token_capacity;
    int token_count;
} Scanner;

Scanner scanner_create(Source* source);
void scanner_free(Scanner* scanner);
Token* scan(Scanner* scanner);

#endif
