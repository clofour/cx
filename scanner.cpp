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
    }
}

Token* scan_source(char* source) {
    Scanner scanner;
    scanner.source = source;
    scanner.start = scanner.current;

    scan_token(&scanner);

    return scanner.tokens;
}
