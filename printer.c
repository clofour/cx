#include "printer.h"
#include "scanner.h"
#include "parser.h"
#include "feedback.h"
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
        case TOKEN_MODULO: printf("%"); break;
        case TOKEN_STAR: printf("*"); break;
        case TOKEN_EQUAL_EQUAL: printf("=="); break;
        case TOKEN_BANG_EQUAL: printf("!="); break;
        case TOKEN_GREATER: printf(">"); break;
        case TOKEN_GREATER_EQUAL: printf(">="); break;
        case TOKEN_LESS: printf("<"); break;
        case TOKEN_LESS_EQUAL: printf("<="); break;
        case TOKEN_NONE: error_token(token, "Unrecognized token."); break;
        default: printf(keyword_lookup(type)); break;
    }

   printf("\n");
}

void print_expr(int depth, Expr* expr_pointer) {
    Expr expr = *expr_pointer;

    switch (expr.type) {
        case EXPR_VAR:
            VarExpr var_expr = expr.value.var;

            print_token(depth, var_expr.name);

            return;

        case EXPR_ASSIGN:
            AssignExpr assign_expr = expr.value.assign;

            print_token(depth, assign_expr.name);
            print_expr(depth + 1, assign_expr.value);

            return;

        case EXPR_BINARY:
            BinaryExpr binary_expr = expr.value.binary;

            print_token(depth, binary_expr.operator);

            print_expr(depth + 1, binary_expr.leftExpr);
            print_expr(depth + 1, binary_expr.rightExpr);

            return;

        case EXPR_UNARY:
            UnaryExpr unary_expr = expr.value.unary;

            print_token(depth, unary_expr.operator);
            print_expr(depth + 1, unary_expr.expr);
            
            return;

        case EXPR_PRIMARY:
            PrimaryExpr primary_expr = expr.value.primary;

            print_token(depth, primary_expr.value);
            return;
    }
}

void print_stmt(int depth, Stmt* stmt_pointer) {
    Stmt stmt = *stmt_pointer;

    switch(stmt.type) {
        case STMT_BLOCK:
            StmtBlock stmt_block = stmt.value.block;

            for (int i = 0; i < stmt_block.length; i++) {
                print_stmt(depth, stmt_block.statements[i]);
            }

            break;

        case STMT_COND:
            StmtCond stmt_cond = stmt.value.cond;

            print_string(depth, "if");

            print_expr(depth + 1, stmt_cond.condition);
            print_stmt(depth + 1, stmt_cond.body);

            break;

        case STMT_LOOP:
            StmtLoop stmt_loop = stmt.value.loop;

            print_string(depth, "while");

            print_expr(depth + 1, stmt_loop.condition);
            print_stmt(depth + 1, stmt_loop.body);

            break;

        case STMT_EXPR:
            StmtExpr stmt_expr = stmt.value.expr;
            print_expr(depth, stmt_expr.expr);

            break;

        case STMT_PRINT:
            StmtPrint stmt_print = stmt.value.print;

            print_string(depth, "print");

            print_expr(depth + 1, stmt_print.expr);

            break;

        case STMT_VAR:
            StmtVar stmt_var = stmt.value.var;

            print_token(depth, stmt_var.name);
            print_expr(depth + 1, stmt_var.expr);

            break;
    }
}

void print(AST ast) {
    for (int i = 0; i < ast.length; i++) {
        print_stmt(0, ast.nodes[i]);
    }
    
    success("Complete!");
}