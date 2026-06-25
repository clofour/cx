#include "keywords.h"
#include "reporter.h"
#include "scanner.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static TokenType keyword_lookup(char* keyword) {
    for (int i = 0; i < keyword_count; i++) {
        if (strcmp(keyword, keywords[i].keyword) == 0) {
            return keywords[i].type;
        }
    }

    return TOKEN_NONE;
}

static char* substring(Scanner *scanner, int start, int end) {
    int length = end - start;
    char* value = (char*) malloc(length + 1);
    memcpy(value, scanner-> source + start, length);
    value[length] = '\0';

    return value;
}

static bool is_at_end(Scanner *scanner) {
    return scanner->current >= scanner->source_length;
}

static char advance(Scanner *scanner) {
    return scanner->source[scanner->current++];
}

static bool match(Scanner *scanner, char reference) {
    if (is_at_end(scanner)) return false;
    if (scanner->source[scanner->current] != reference) {
        return false;
    }

    scanner->current++;
    return true;
}

static char peek(Scanner *scanner) {
    if (is_at_end(scanner)) return '\0';
    return scanner->source[scanner->current];
}

static char peek_next(Scanner *scanner) {
    if (scanner->current + 1 >= scanner->source_length) return '\0';
    return scanner->source[scanner->current + 1];
}

static Token* add_token(Scanner *scanner, TokenType type) {
    Token token;
    token.type = type;
    token.line = scanner->line;
    token.start = &(scanner->source[scanner->start]);
    token.length = scanner->current - scanner->start;

    scanner->tokens[scanner->token_count++] = token;

    return &scanner->tokens[scanner->token_count - 1];
}

static Token* number(Scanner *scanner) {
    while (isdigit(peek(scanner))) advance(scanner);

    if (peek(scanner) == '.' && isdigit(peek_next(scanner))) advance(scanner);

    while (isdigit(peek(scanner))) advance(scanner);

    char* string_value = substring(scanner, scanner->start, scanner->current);
    char* number_value;
    Token* token = add_token(scanner, TOKEN_NUMBER);
    token->value.float_value = strtod(string_value, &number_value);
    free(string_value);

    return token;
}

static Token* identifier(Scanner *scanner) {
    while (isalnum(peek(scanner))) advance(scanner);

    char* value = substring(scanner, scanner->start, scanner->current);
    TokenType type = keyword_lookup(value);
    if (type == TOKEN_NONE) type = TOKEN_IDENTIFIER;
    Token* token = add_token(scanner, type);
    if (type == TOKEN_IDENTIFIER) {
        token->value.identifier_value = value;
    } else {
        free(value);
    };

    return token;
}

static Token* string(Scanner *scanner) {
    while (peek(scanner) != '"' && peek(scanner) != '\n' && !is_at_end(scanner)) {
        advance(scanner);
    }

    if (is_at_end(scanner) || !match(scanner, '"')) {
        error_line(scanner->shared_data->reporter, scanner->line, "Unterminated string.");
    }

    char* value = substring(scanner, scanner->start + 1, scanner->current - 1);
    Token* token = add_token(scanner, TOKEN_STRING);
    token->value.string_value = value;

    return token;
}

static Token* scan_token(Scanner *scanner) {
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
                while (peek(scanner) != '\n' && !is_at_end(scanner)) advance(scanner);
            }
            else {
                return add_token(scanner, TOKEN_SLASH);
            }
            break;
        case '%': return add_token(scanner, TOKEN_MODULO);
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
        case '\r':
        case '\t':
            break;
        case '\n':
            scanner->line++;
            break;
        default:

            if (isalpha(character)) {
                return identifier(scanner);
            }
            else {
                error_line(scanner->shared_data->reporter, scanner->line, "Unrecognized character.");
            }

    }

    return NULL;
}

Scanner scanner_create(SharedData* shared_data, Source* source) {
    Scanner scanner;
    scanner.shared_data = shared_data;
    scanner.source = source->content;
    scanner.source_length = source->length;
    scanner.line =  1;
    scanner.current = 0;
    scanner.token_capacity = 100;
    scanner.tokens = (Token*) malloc(sizeof(Token) * scanner.token_capacity);
    scanner.token_count = 0;

    return scanner;
}

void scanner_free(Scanner* scanner) {
    free(scanner->tokens);
}

Token* scan(Scanner* scanner) {
    while (!is_at_end(scanner)) {
        scanner->start = scanner->current;
        scan_token(scanner);

        if (scanner->token_count > scanner->token_capacity - 10) {
            scanner->token_capacity = scanner->token_capacity * 2;
            scanner->tokens = realloc(scanner->tokens, sizeof(Token) * scanner->token_capacity);
        }
    }
    add_token(scanner, TOKEN_EOF);

    success(scanner->shared_data->reporter, "Complete!");

    return scanner->tokens;
}
