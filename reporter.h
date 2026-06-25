#ifndef REPORTER_H
#define REPORTER_H

typedef struct Token Token;
typedef struct Expr Expr;

typedef enum {
    FEEDBACK_ERROR,
    FEEDBACK_SUCCESS,
} FeedbackType;

typedef enum {
    COLOR_RED,
    COLOR_GREEN,
} Color;

typedef struct {
    char* file_name;
} Reporter;


Reporter reporter_create(char* file_name);
void error(Reporter* reporter, char* message);
void error_token(Reporter* reporter, Token* token, char* message);
void error_expr(Reporter* reporter, Expr* expr, char* message);
void error_line(Reporter* reporter, int line, char* message);
void success(Reporter* reporter, char* message);

#endif