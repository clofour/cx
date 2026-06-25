#include "reporter.h"
#include "scanner.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

Reporter reporter_create(char* file_name) {
    Reporter reporter;
    reporter.file_name = file_name;

    return reporter;
}

void color_output(Color color, bool state) {

    if (state == true) {

        switch (color) {
            case COLOR_RED: printf("%s", "\033[31m"); break;
            case COLOR_GREEN: printf("%s", "\033[32m"); break;
        }

    } else {

        printf("%s", "\033[0m");

    }

}

void feedback(FeedbackType feedback_type, char* format, ...) {
    va_list args;
    va_start(args, format);

    Color color;
    switch (feedback_type) {
        case FEEDBACK_ERROR: color = COLOR_RED; break;
        case FEEDBACK_SUCCESS: color = COLOR_GREEN; break;
    }

    color_output(color, true);
    vprintf(format, args);
    color_output(color, false);

    va_end(args);

    if (feedback_type == FEEDBACK_ERROR) {
        exit(EXIT_FAILURE);
    }
}

void error(Reporter* reporter, char* message) {
    feedback(FEEDBACK_ERROR, "%s: error: %s\n", reporter->file_name, message);
}

void error_token(Reporter* reporter, Token* token, char* message) {
    feedback(FEEDBACK_ERROR, "%s:%d: error: %s\n", reporter->file_name, token->line, message);
}

void error_expr(Reporter* reporter, Expr* expr, char* message) {
    feedback(FEEDBACK_ERROR, "%s:%d: error: %s\n", reporter->file_name, expr->line, message);
}

void error_line(Reporter* reporter, int line, char* message) {
    feedback(FEEDBACK_ERROR, "%s:%d: error: %s\n", reporter->file_name, line, message);
}

void success(Reporter* reporter, char* message) {
    feedback(FEEDBACK_SUCCESS, "%s: success: %s\n", reporter->file_name, message);
}
