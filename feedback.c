#include "feedback.h"
#include "scanner.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

void feedback(char* format, ...) {
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);

    exit(EXIT_FAILURE);
}

void color(Color color, bool state) {

    if (state == true) {
        switch (color) {
            case COLOR_RED:
                printf("%s", "\033[31m");
        }
    } else {
        switch (color) {
            case COLOR_RED:
                printf("%s", "\033[0m");
        }
    }

}

void error(char* message) {
    color(COLOR_RED, true);
    feedback("%s: error: %s", "hello", message);
    color(COLOR_RED, false);
}

void error_token(Token* token, char* message) {
    color(COLOR_RED, true);
    feedback("%s:%d: error: %s\n", "hello", token->line, message);
    color(COLOR_RED, false);
}

void error_line(int line, char* message) {
    color(COLOR_RED, true);
    feedback("%s:%d: error: %s\n", "hello", line, message);
    color(COLOR_RED, false);
}
