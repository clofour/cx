#ifndef FEEDBACK_H
#define FEEDBACK_H

#include "scanner.h"

typedef enum {
    FEEDBACK_ERROR,
    FEEDBACK_SUCCESS,
} FeedbackType;

typedef enum {
    COLOR_RED,
    COLOR_GREEN,
} Color;

void error(char* message);
void error_token(Token* token, char* message);
void error_line(int line, char* message);
void success(char* message);

#endif