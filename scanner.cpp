#include "scanner.h"
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include "ctype.h"

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

char peek(Scanner *scanner) {
    return scanner->source[scanner->current];
}

char peekNext(Scanner *scanner) {
    return scanner->source[scanner-> current + 1];
}

Token* add_token(Scanner *scanner, TokenType type) {
    Token token;
    token.TokenType = type;
    token.line = scanner->line;
    token.start = &(scanner->source[scanner->start]);

    return &token;
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
            while (peek(scanner) != '"') {
                advance(scanner);
            }
            advance(scanner);

            int length = (scanner->current - 1) - (scanner->start + 1);
            char* value = (char*) malloc(length + 1);
            memcpy(value, scanner-> source + scanner->start + 1, length);
            Token* token = add_token(scanner, TOKEN_STRING);
            token->value.string_value = value;
            break;
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
            while (isdigit(peek(scanner))) advance(scanner);

            if (peek(scanner) == '.' && isdigit(peekNext(scanner))) advance(scanner);

            while (isdigit(peek(scanner))) advance(scanner);
            break;
        case ' ':
            break;
        case '\n':
            scanner->line++;
            break;
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
