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

typedef struct Stmt Stmt;

typedef enum {
    STMT_COND,
    STMT_VAR,
    STMT_EXPR,
    STMT_PRINT
} StmtType;

typedef struct {
    Expr* condition;
    Stmt* body;
} StmtCond;

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

struct Stmt {
    StmtType type;
    union {
        StmtCond cond;
        StmtVar var;
        StmtExpr expr;
        StmtPrint print;
    } value;
};

typedef struct {
    int current;
    Token* tokens;
    Expr* expressions;
    int expressionsLength;
    int expressionsCapacity;
    Stmt* statements;
    int statementsLength;
    int statementsCapacity;
    Stmt** program;
    int programLength;
    int programCapacity;
} Parser;

typedef struct {
    Stmt** nodes;
    int length;
} AST;

AST parse(Token* tokens);

#endif
