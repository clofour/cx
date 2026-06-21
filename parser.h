#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"

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

struct Expr {
    ExprType type;
    union {
        BinaryExpr binary;
        UnaryExpr unary;
        PrimaryExpr primary; 
    } value;
};

typedef enum {
    STMT_VAR,
    STMT_EXPR,
    STMT_PRINT
} StmtType;

typedef struct {
    Token* name;
    Expr* expr;
} StmtVar;

typedef struct {
    Expr* expr;
} StmtExpr;

typedef struct {
    Expr* expr;
} StmtPrint;

typedef struct {
    StmtType type;
    union {
        StmtVar var;
        StmtExpr expr;
        StmtPrint print;
    } value;
} Stmt;

typedef struct {
    int hello;
} Decl;

Expr* parse(Token* tokens);

#endif
