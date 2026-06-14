#include "scanner.h"

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
    }
}

Token* scan_source(char* source) {
    Scanner scanner;
    scanner.source = source;
    scanner.start = scanner.current;

    scan_token(&scanner);

    return scanner.tokens;
}
