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

int print_token(Printer* printer, Token* token) {
    printf("%*s| ", printer->depth, "");

    TokenType type = token->type;
    switch (type) {
        case TOKEN_IDENTIFIER: printf(token->value.string_value); break;
        case TOKEN_STRING: printf(token->value.string_value); break;
        case TOKEN_NUMBER: printf("%f", token->value.float_value); break;
        case TOKEN_PLUS: printf("+"); break;
        case TOKEN_MINUS: printf("-"); break;
        case TOKEN_SLASH: printf("/"); break;
        case TOKEN_STAR: printf("*"); break;
        case TOKEN_NONE: printf("An error has occurred."); break;
        default: printf(keyword_lookup(type)); break;
    }

    printf("\n");
}

void print_node(Printer* printer, Expr* expr_pointer) {
    Expr expr = *expr_pointer;

    switch (expr.type) {
        case EXPR_BINARY:
            BinaryExpr binaryExpr = expr.value.binary;

            print_token(printer, expr.value.binary.operator);
            printer->depth++;

            print_node(printer, binaryExpr.leftExpr);
            print_node(printer, binaryExpr.rightExpr);

            return;

        case EXPR_UNARY:
            UnaryExpr unaryExpr = expr.value.unary;

            print_token(printer, unaryExpr.operator);
            printer->depth++;

            print_node(printer, unaryExpr.expr);
            return;

        case EXPR_PRIMARY:
            PrimaryExpr primaryExpr = expr.value.primary;

            print_token(printer, primaryExpr.value);
            return;
    }
}

void print(Expr* expr) {
    Printer* printer = (Printer*) malloc(sizeof(Printer));
    printer->depth = 0;
    print_node(printer, expr);
}