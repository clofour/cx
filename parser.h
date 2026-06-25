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
    int line;
};

typedef struct Stmt Stmt;

typedef enum {
    STMT_COND,
    STMT_LOOP,
    STMT_BLOCK,
    STMT_VAR,
    STMT_EXPR,
    STMT_PRINT
} StmtType;

typedef struct {
    Stmt** statements;
    int length;
} StmtBlock;

typedef struct {
    Expr* condition;
    Stmt* then_body;
    Stmt* else_body;
} StmtCond;

typedef struct {
    Expr* condition;
    Stmt* body;
} StmtLoop;

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
        StmtLoop loop;
        StmtBlock block;
        StmtVar var;
        StmtExpr expr;
        StmtPrint print;
    } value;
};

typedef struct {
    SharedData* shared_data;
    int current;
    Token* tokens;
    Expr* expressions;
    int expressions_length;
    int expressions_capacity;
    Stmt* statements;
    int statements_length;
    int statements_capacity;
    Stmt*** blocks;
    int blocks_length;
    int blocks_capacity;
    Stmt** program;
    int program_length;
    int program_capacity;
} Parser;

typedef struct {
    Stmt** nodes;
    int length;
} AST;

Parser parser_create(SharedData* shared_data, Token* tokens);
void parser_free(Parser* parser);
AST parse(Parser* parser);

#endif
