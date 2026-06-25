#ifndef FEEDBACK_H
#define FEEDBACK_H

#include "scanner.h"

typedef enum {
    COLOR_RED,
} Color;

void error(char* message);
void error_token(Token* token, char* message);
void error_line(int line, char* message);

#endif