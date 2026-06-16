#include "scanner.h"
#include "ctype.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char* source;
    int line;
    int start;
    int current;
    Token* tokens;
    int tokenCount;
} Scanner;

typedef struct {
    char* keyword;
    TokenType type;
} Keyword;

Keyword keywords[] = {
    { "and", TOKEN_AND },
    { "class", TOKEN_CLASS },
    { "else", TOKEN_ELSE },
    { "false", TOKEN_FALSE },
    { "for", TOKEN_FOR },
    { "function", TOKEN_FUNCTION },
    { "if", TOKEN_IF },
    { "null", TOKEN_NULL },
    { "or", TOKEN_OR },
    { "print", TOKEN_PRINT },
    { "return", TOKEN_RETURN },
    { "true", TOKEN_TRUE },
    { "var", TOKEN_VAR },
    { "while", TOKEN_WHILE },
};
int keyword_count = sizeof(keywords) / sizeof(Keyword);

TokenType keyword_lookup(char* keyword) {
    for (int i = 0; i < keyword_count; i++) {
        if (strcmp(keyword, keywords[i].keyword) == 0) {
            return keywords[i].type;
        }
    }
}

char* substring(Scanner *scanner, int start, int end) {
    int length = end - start;
    char* value = (char*) malloc(length + 1);
    memcpy(value, scanner-> source + start, length);

    return value;
}

char advance(Scanner *scanner) {
    return scanner->source[scanner->current++];
}

bool match(Scanner *scanner, char reference) {
    if (scanner->source[scanner->current] != reference) {
        return false;
    }

    scanner->current++;
    return true;
}

char peek(Scanner *scanner) {
    return scanner->source[scanner->current];
}

char peekNext(Scanner *scanner) {
    return scanner->source[scanner->current + 1];
}

Token* add_token(Scanner *scanner, TokenType type) {
    Token token;
    token.type = type;
    token.line = scanner->line;
    token.start = &(scanner->source[scanner->start]);

    scanner->tokens[scanner->tokenCount++] = token;

    return &scanner->tokens[scanner->tokenCount - 1];
}

Token* number(Scanner *scanner) {
    while (isdigit(peek(scanner))) advance(scanner);

    if (peek(scanner) == '.' && isdigit(peekNext(scanner))) advance(scanner);

    while (isdigit(peek(scanner))) advance(scanner);

    char* value = substring(scanner, scanner->start, scanner->current);
    Token* token = add_token(scanner, TOKEN_NUMBER);
    token->value.float_value = strtod(value, &value);

    return token;
}

Token* identifier(Scanner *scanner) {
    while (isalnum(peek(scanner))) advance(scanner);

    char* value = substring(scanner, scanner->start, scanner->current);
    TokenType type = keyword_lookup(value);
    if (type == NULL) type = TOKEN_IDENTIFIER;
    Token* token = add_token(scanner, type);

    return token;
}

Token* string(Scanner *scanner) {
    while (peek(scanner) != '"') {
        advance(scanner);
    }
    advance(scanner);

    char* value = substring(scanner, scanner->current - 1, scanner->start + 1);
    Token* token = add_token(scanner, TOKEN_STRING);
    token->value.string_value = value;

    return token;
}

Token* scan_token(Scanner *scanner) {
    char character = advance(scanner);
    switch (character) {
        case '(': return add_token(scanner, TOKEN_LEFT_PARENTHESIS);
        case ')': return add_token(scanner, TOKEN_RIGHT_PARENTHESIS);
        case '{': return add_token(scanner, TOKEN_LEFT_BRACE);
        case '}': return add_token(scanner, TOKEN_RIGHT_BRACE);
        case ',': return add_token(scanner, TOKEN_COMMA);
        case '.': return add_token(scanner, TOKEN_DOT);
        case '-': return add_token(scanner, TOKEN_MINUS);
        case '+': return add_token(scanner, TOKEN_PLUS);
        case ';': return add_token(scanner, TOKEN_SEMICOLON);
        case '/':
            if (match(scanner, '/')) { // Comment
                while (peek(scanner) != '\n') advance(scanner);
            }
            else {
                return add_token(scanner, TOKEN_SLASH);
            }
            break;
        case '*': return add_token(scanner, TOKEN_STAR);
        case '!': return add_token(scanner, match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=': return add_token(scanner, match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<': return add_token(scanner, match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return add_token(scanner, match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"':
            return string(scanner);
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return number(scanner);
        case ' ':
            break;
        case '\n':
            scanner->line++;
            break;
        default:
            if (isalpha(character)) {
                return identifier(scanner);
            }
            else {
                printf("ERROR");
            }
    }
}

Token* scan_source(char* source) {
    Scanner scanner;
    scanner.source = source;
    scanner.line = 0;
    scanner.current = 0;
    scanner.tokens = (Token*) malloc(sizeof(Token) * 100);

    while (true) {
        scanner.start = scanner.current;
        scan_token(&scanner);
    }

    return scanner.tokens;
}
