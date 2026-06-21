#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"

typedef enum {
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_ASSIGN,
    EXPR_PRIMARY,
    EXPR_VAR
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
    Token* name;
    Expr* value;
} AssignExpr;

typedef struct {
    Token* value;
} PrimaryExpr;

typedef struct {
    Token* name;
} VarExpr;

struct Expr {
    ExprType type;
    union {
        BinaryExpr binary;
        UnaryExpr unary;
        PrimaryExpr primary;
        VarExpr var;
        AssignExpr assign;
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
    int current;
    Token* tokens;
    Expr* expressions;
    int expressionsLength;
    int expressionsCapacity;
    Stmt* statements;
    int statementsLength;
    int statementsCapacity;
} Parser;

typedef struct {
    Stmt* statements;
    int length;
} AST;

AST parse(Token* tokens);

#endif
