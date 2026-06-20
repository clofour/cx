#include "parser.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdarg.h>

typedef struct {
    int current;
    Token* tokens;
} Parser;

Expr* create_expr() {
    Expr expr;
    return &expr;
}

Expr* create_binary_expr(Expr* leftExpr, Token* operator, Expr* rightExpr) {
    Expr* expr = create_expr();

    expr->type = EXPR_BINARY;
    BinaryExpr* binaryExpr;
    binaryExpr->leftExpr = leftExpr;
    binaryExpr->operator = operator;
    binaryExpr->rightExpr = rightExpr;

    expr->value.binary = binaryExpr;

    return expr;
}

bool is_at_end(Parser* parser) {
    Token* next = peek(parser);
    return next->type == TOKEN_EOF;
}

Token* previous(Parser* parser) {
    return parser->tokens[parser->current - 1];
}

Token* advance(Parser* parser) {
    if (is_at_end(parser)) parser->current++;
    return previous(parser);
}

Token* peek(Parser* parser) {
    return parser->tokens[parser->current];
}

bool check(Parser* parser, TokenType type) {
    if (is_at_end(parser)) return false;
    Token* next = peek(parser);
    return next->type == type;
}

bool match(Parser* parser, int count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        TokenType type = va_arg(args, TokenType);
        if (check(parser, type)) {
            advance(parser);

            va_end(args);
            return true;
        }
    }

    va_end(args);
    return false;
}

Expr* comparison(Parser* parser) {

}

Expr* equality(Parser* parser) {
    Expr* expr = comparison(parser);

    while (match(parser, 2, TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL)) {
        Token* operator = previous(parser);
        Expr* right = comparison(parser);
        expr = create_binary_expr(expr, operator, right);
    }

    return expr;
}

Expr* expression(Parser* parser) {
    return equality(parser);
}
