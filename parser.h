#ifndef PARSER_H
#define PARSER_H

typedef struct Token Token;

typedef enum {
    EXPR_EXPRESSION,
    EXPR_EQUALITY,
    EXPR_COMPARISON,
    EXPR_TERM,
    EXPR_FACTOR,
    EXPR_UNARY,
    EXPR_PRIMARY
} ExprType;

typedef struct Expr Expr;

typedef struct {
    Expr* leftExpr;
    Token* operator;
    Expr* rightExpr;
} EqualityExpr;

typedef struct {
    Expr* leftExpr;
    Token* operator;
    Expr* rightExpr;
} ComparisonExpr;

typedef struct {
    Expr* leftExpr;
    Token* operator;
    Expr* rightExpr;
} TermExpr;

typedef struct {
    Expr* leftExpr;
    Token* operator;
    Expr* rightExpr;
} FactorExpr;

typedef struct {
    Token* operator;
    Expr* expr;
} UnaryExpr;

typedef struct {
    Token* value;
} PrimaryExpr;

typedef struct {
    ExprType type;
    union {
        EqualityExpr equality;
        ComparisonExpr comparison;
        TermExpr term;
        FactorExpr factor;
        UnaryExpr unary;
        PrimaryExpr primary; 
    };
} Expr;

#endif
