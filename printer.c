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

void print_offset(Printer* printer) {
    printf("%*s| ", printer->depth, "");
}

void print_string(Printer* printer, char* string) {
    print_offset(printer);
    printf(string);
    printf("\n");
}

void print_token_literal(Printer* printer, Token* token) {
    printf("%.*s", token->length, token->start);
}

void print_token(Printer* printer, Token* token) {
    print_offset(printer);

    TokenType type = token->type;
    switch (type) {
        case TOKEN_IDENTIFIER: print_token_literal(printer, token); break;
        case TOKEN_STRING: print_token_literal(printer, token); break;
        case TOKEN_NUMBER: print_token_literal(printer, token); break;
        case TOKEN_PLUS: printf("+"); break;
        case TOKEN_MINUS: printf("-"); break;
        case TOKEN_SLASH: printf("/"); break;
        case TOKEN_STAR: printf("*"); break;
        case TOKEN_NONE: printf("An error has occurred."); break;
        default: printf(keyword_lookup(type)); break;
    }

   printf("\n");
}

void print_expr(Printer* printer, Expr* expr_pointer) {
    Expr expr = *expr_pointer;

    switch (expr.type) {
        case EXPR_VAR:
            VarExpr varExpr = expr.value.var;

            print_token(printer, varExpr.name);

            return;

        case EXPR_ASSIGN:
            AssignExpr assignExpr = expr.value.assign;

            print_token(printer, assignExpr.name);
            printer->depth++;

            print_expr(printer, assignExpr.value);

            return;

        case EXPR_BINARY:
            BinaryExpr binaryExpr = expr.value.binary;

            print_token(printer, binaryExpr.operator);
            printer->depth++;

            print_expr(printer, binaryExpr.leftExpr);
            print_expr(printer, binaryExpr.rightExpr);

            return;

        case EXPR_UNARY:
            UnaryExpr unaryExpr = expr.value.unary;

            print_token(printer, unaryExpr.operator);
            printer->depth++;

            print_expr(printer, unaryExpr.expr);
            return;

        case EXPR_PRIMARY:
            PrimaryExpr primaryExpr = expr.value.primary;

            print_token(printer, primaryExpr.value);
            return;
    }
}

void print_stmt(Printer* printer, Stmt* stmt_pointer) {
    Stmt stmt = *stmt_pointer;

    switch(stmt.type) {
        case STMT_EXPR:
            StmtExpr stmtExpr = stmt.value.expr;
            print_expr(printer, stmtExpr.expr);

            break;

        case STMT_PRINT:
            StmtPrint stmtPrint = stmt.value.print;

            print_string(printer, "print");

            print_expr(printer, stmtPrint.expr);

            break;

        case STMT_VAR:
            StmtVar stmtVar = stmt.value.var;

            print_token(printer, stmtVar.name);
            printer->depth++;

            print_expr(printer, stmtVar.expr);

            break;
    }
}

void print(Stmt* statements) {
    Printer* printer = (Printer*) malloc(sizeof(Printer));
    printer->depth = 0;

    for (int i = 0; i < 1; i++) {
        print_stmt(printer, &statements[i]);
    }
}