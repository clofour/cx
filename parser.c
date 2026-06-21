#include "parser.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct {
    int current;
    Token* tokens;
    Expr* expressions;
    int expressionsLength;
    int expressionsCapacity;
} Parser;

static Expr* create_expr(Parser* parser) {
    Expr expr = {0};
    parser->expressions[parser->expressionsLength++] = expr;
    return &parser->expressions[parser->expressionsLength - 1];
}

static Expr* create_primary_expr(Parser* parser, Token* value) {
    Expr* expr = create_expr(parser);

    expr->type = EXPR_PRIMARY;

    PrimaryExpr primaryExpr;
    primaryExpr.value = value;
    expr->value.primary = primaryExpr;

    return expr;
}

static Expr* create_unary_expr(Parser* parser, Token* operator, Expr* expression) {
    Expr* expr = create_expr(parser);

    expr->type = EXPR_UNARY;

    UnaryExpr unaryExpr;
    unaryExpr.operator = operator;
    unaryExpr.expr = expression;
    expr->value.unary = unaryExpr;

    return expr;
}

static Expr* create_binary_expr(Parser* parser, Expr* leftExpr, Token* operator, Expr* rightExpr) {
    Expr* expr = create_expr(parser);

    expr->type = EXPR_BINARY;

    BinaryExpr binaryExpr;
    binaryExpr.leftExpr = leftExpr;
    binaryExpr.operator = operator;
    binaryExpr.rightExpr = rightExpr;
    expr->value.binary = binaryExpr;

    return expr;
}

static Token* peek(Parser* parser) {
    return &parser->tokens[parser->current];
}

static bool is_at_end(Parser* parser) {
    Token* next = peek(parser);
    return next->type == TOKEN_EOF;
}

static Token* previous(Parser* parser) {
    return &parser->tokens[parser->current - 1];
}

static Token* advance(Parser* parser) {
    if (!is_at_end(parser)) parser->current++;
    return previous(parser);
}

static bool check(Parser* parser, TokenType type) {
    if (is_at_end(parser)) return false;
    Token* next = peek(parser);
    return next->type == type;
}

static bool match(Parser* parser, int count, ...) {
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

static Token* consume(Parser* parser, TokenType type, char* message) {
    if (check(parser, type)) return advance(parser);

    printf(message);
    return NULL;
}

static Expr* expression(Parser* parser);
static Expr* equality(Parser* parser);
static Expr* comparison(Parser* parser);
static Expr* term(Parser* parser);
static Expr* factor(Parser* parser);
static Expr* unary(Parser* parser);
static Expr* primary(Parser* parser);

static Expr* primary(Parser* parser) {
    if (match(parser, 5, TOKEN_FALSE, TOKEN_TRUE, TOKEN_NULL, TOKEN_NUMBER, TOKEN_STRING)) {
        return create_primary_expr(parser, previous(parser));
    }

    if (match(parser, 1, TOKEN_LEFT_PARENTHESIS)) {
        Expr* expr = expression(parser);
    }

    printf("Expected expression.");
    return NULL;
}

static Expr* unary(Parser* parser) {
    if (match(parser, 2, TOKEN_BANG, TOKEN_MINUS)) {
        Token* operator = previous(parser);
        Expr* expr = unary(parser);
        return create_unary_expr(parser, operator, expr);
    }

    return primary(parser);
}

static Expr* factor(Parser* parser) {
    Expr* expr = unary(parser);

    while (match(parser, 2, TOKEN_SLASH, TOKEN_STAR)) {
        Token* operator = previous(parser);
        Expr* right = unary(parser);
        expr = create_binary_expr(parser, expr, operator, right);
    }

    return expr;
}

static Expr* term(Parser* parser) {
    Expr* expr = factor(parser);

    while (match(parser, 2, TOKEN_MINUS, TOKEN_PLUS)) {
        Token* operator = previous(parser);
        Expr* right = factor(parser);
        expr = create_binary_expr(parser, expr, operator, right);
    }

    return expr;
}

static Expr* comparison(Parser* parser) {
    Expr* expr = term(parser);

    while (match(parser, 4, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL)) {
        Token* operator = previous(parser);
        Expr* right = term(parser);
        expr = create_binary_expr(parser, expr, operator, right);
    }

    return expr;
}

static Expr* equality(Parser* parser) {
    Expr* expr = comparison(parser);

    while (match(parser, 2, TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL)) {
        Token* operator = previous(parser);
        Expr* right = comparison(parser);
        expr = create_binary_expr(parser, expr, operator, right);
    }

    return expr;
}

static Expr* expression(Parser* parser) {
    return equality(parser);
}

Expr* parse(Token* tokens) {
    Parser parser;
    parser.current = 0;
    parser.tokens = tokens;
    parser.expressionsCapacity = 100;
    parser.expressions = (Expr*) malloc(sizeof(Expr) * parser.expressionsCapacity);
    parser.expressionsLength = 0;

    Expr* expressions = expression(&parser);
    return expressions;
}
