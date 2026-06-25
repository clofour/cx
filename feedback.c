#include "feedback.h"
#include "scanner.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

void color_output(Color color, bool state) {

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

void feedback(Color color, char* format, ...) {
    va_list args;
    va_start(args, format);

    color_output(color, true);

    vprintf(format, args);

    color_output(color, false);

    va_end(args);

    exit(EXIT_FAILURE);
}

void error(char* message) {
    feedback(COLOR_RED, "%s: error: %s\n", "hello", message);
}

void error_token(Token* token, char* message) {
    feedback(COLOR_RED, "%s:%d: error: %s\n", "hello", token->line, message);
}

void error_line(int line, char* message) {
    feedback(COLOR_RED, "%s:%d: error: %s\n", "hello", line, message);
}
