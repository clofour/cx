#ifndef PARSER_H
#define PARSER_H

typedef struct Token Token;

typedef enum {
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_PRIMARY
} ExprType;

typedef struct Expr Expr;

typedef struct {
    Expr* leftExpr;
    Token* operator;
    Expr* rightExpr;
} BinaryExpr;

typedef struct {
    Token* operator;
    Expr* expr;
} UnaryExpr;

typedef struct {
    Token* value;
} PrimaryExpr;

typedef struct Expr {
    ExprType type;
    union {
        BinaryExpr binary;
        UnaryExpr unary;
        PrimaryExpr primary; 
    };
} Expr;

#endif
