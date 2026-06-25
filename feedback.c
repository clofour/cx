#include "scanner.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void feedback(char* format, ...) {
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);

    exit(EXIT_FAILURE);
}

void error(char* message) {
    feedback("%s: error: %s", "hello", message);
}

void error_token(Token* token, char* message) {
    feedback("%s:%d: error: %s\n", "hello", token->line, message);
}

void error_line(int line, char* message) {
    feedback("%s:%d: error: %s\n", "hello", line, message);
}