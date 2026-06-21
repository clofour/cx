#include "printer.h"
#include "scanner.h"
#include "parser.h"
#include "shared.h"
#include <stdio.h>
#include <stdlib.h>

static char* keyword_lookup(TokenType type) {
    for (int i = 0; i < keyword_count; i++) {
        Keyword keyword = keywords[i];
        if (type == keyword.type) {
            return keyword.keyword;
        }
    }

    return TOKEN_NONE;
}

void print_offset(int depth) {
    printf("%*s| ", depth, "");
}

void print_string(int depth, char* string) {
    print_offset(depth);
    printf(string);
    printf("\n");
}

void print_token_literal(int depth, Token* token) {
    printf("%.*s", token->length, token->start);
}

void print_token(int depth, Token* token) {
    print_offset(depth);

    TokenType type = token->type;
    switch (type) {
        case TOKEN_IDENTIFIER: print_token_literal(depth, token); break;
        case TOKEN_STRING: print_token_literal(depth, token); break;
        case TOKEN_NUMBER: print_token_literal(depth, token); break;
        case TOKEN_PLUS: printf("+"); break;
        case TOKEN_MINUS: printf("-"); break;
        case TOKEN_SLASH: printf("/"); break;
        case TOKEN_STAR: printf("*"); break;
        case TOKEN_NONE: printf("An error has occurred."); break;
        default: printf(keyword_lookup(type)); break;
    }

   printf("\n");
}

void print_expr(int depth, Expr* expr_pointer) {
    Expr expr = *expr_pointer;

    switch (expr.type) {
        case EXPR_VAR:
            VarExpr varExpr = expr.value.var;

            print_token(depth, varExpr.name);

            return;

        case EXPR_ASSIGN:
            AssignExpr assignExpr = expr.value.assign;

            print_token(depth, assignExpr.name);
            print_expr(depth + 1, assignExpr.value);

            return;

        case EXPR_BINARY:
            BinaryExpr binaryExpr = expr.value.binary;

            print_token(depth, binaryExpr.operator);

            print_expr(depth + 1, binaryExpr.leftExpr);
            print_expr(depth + 1, binaryExpr.rightExpr);

            return;

        case EXPR_UNARY:
            UnaryExpr unaryExpr = expr.value.unary;

            print_token(depth, unaryExpr.operator);
            print_expr(depth + 1, unaryExpr.expr);
            
            return;

        case EXPR_PRIMARY:
            PrimaryExpr primaryExpr = expr.value.primary;

            print_token(depth, primaryExpr.value);
            return;
    }
}

void print_stmt(int depth, Stmt* stmt_pointer) {
    Stmt stmt = *stmt_pointer;

    switch(stmt.type) {
        case STMT_EXPR:
            StmtExpr stmtExpr = stmt.value.expr;
            print_expr(depth, stmtExpr.expr);

            break;

        case STMT_PRINT:
            StmtPrint stmtPrint = stmt.value.print;

            print_string(depth, "print");

            print_expr(depth, stmtPrint.expr);

            break;

        case STMT_VAR:
            StmtVar stmtVar = stmt.value.var;

            print_token(depth, stmtVar.name);
            print_expr(depth + 1, stmtVar.expr);

            break;
    }
}

void print(AST ast) {
    for (int i = 0; i < ast.length; i++) {
        print_stmt(0, &ast.statements[i]);
    }
}