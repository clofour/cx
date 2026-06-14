#include "scanner.h"
#include <stdio.h>

typedef struct {
    char* source;
    int line;
    int start;
    int current;
    Token* tokens;
} Scanner;

char advance(Scanner *scanner) {
    return scanner->source[scanner->current++];
}

char match(Scanner *scanner, char reference) {
    if (scanner->source[scanner->current] != reference) {
        return false;
    }

    scanner->current++;
    return true;
}

Token* add_token(Scanner *scanner, TokenType type) {
    Token token;
    token.TokenType = type;
    token.line = scanner->line;
    token.start = &(scanner->source[scanner->start]);
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
        case '/': return add_token(scanner, TOKEN_SLASH);
        case '*': return add_token(scanner, TOKEN_STAR);
        case '!': return add_token(scanner, match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=': return add_token(scanner, match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<': return add_token(scanner, match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return add_token(scanner, match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        default:
            printf("ERROR");
    }
}

Token* scan_source(char* source) {
    Scanner scanner;
    scanner.source = source;
    scanner.start = scanner.current;

    scan_token(&scanner);

    return scanner.tokens;
}
