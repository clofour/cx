#include "parser.h"
#include "scanner.h"
#include "feedback.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

static Stmt* create_stmt(Parser* parser) {
    Stmt stmt = {0};

    parser->statements[parser->statements_length++] = stmt;
    return &parser->statements[parser->statements_length - 1];
}

static Stmt* create_stmt_expr(Parser* parser, Expr* expr) {
    Stmt* stmt = create_stmt(parser);
    
    stmt->type = STMT_EXPR;

    StmtExpr stmtExpr;
    stmtExpr.expr = expr;
    stmt->value.expr = stmtExpr;

    return stmt;
}

static Stmt* create_stmt_print(Parser* parser, Expr* expr) {
    Stmt* stmt = create_stmt(parser);
    
    stmt->type = STMT_PRINT;

    StmtPrint stmtPrint;
    stmtPrint.expr = expr;
    stmt->value.print = stmtPrint;

    return stmt;
}

static Stmt* create_stmt_if(Parser* parser, Expr* condition, Stmt* body) {
    Stmt* stmt = create_stmt(parser);
    
    stmt->type = STMT_COND;

    StmtCond stmtCond;
    stmtCond.condition = condition;
    stmtCond.body = body;
    stmt->value.cond = stmtCond;

    return stmt;
}

static Stmt* create_stmt_while(Parser* parser, Expr* condition, Stmt* body) {
    Stmt* stmt = create_stmt(parser);
    
    stmt->type = STMT_LOOP;

    StmtLoop stmtLoop;
    stmtLoop.condition = condition;
    stmtLoop.body = body;
    stmt->value.loop = stmtLoop;

    return stmt;
}

static Stmt* create_stmt_block(Parser* parser, int length, Stmt** statements) {
    Stmt* stmt = create_stmt(parser);
    
    stmt->type = STMT_BLOCK;

    StmtBlock stmtBlock;
    stmtBlock.length = length;
    stmtBlock.statements = statements;
    stmt->value.block = stmtBlock;

    return stmt;
}

static Stmt* create_stmt_var(Parser* parser, Token* name, Expr* expr) {
    Stmt* stmt = create_stmt(parser);
    
    stmt->type = STMT_VAR;

    StmtVar stmtVar;
    stmtVar.name = name;
    stmtVar.expr = expr;
    stmt->value.var = stmtVar;

    return stmt;
}

static Expr* create_expr(Parser* parser) {
    Expr expr = {0};
    parser->expressions[parser->expressions_length++] = expr;
    return &parser->expressions[parser->expressions_length - 1];
}

static Expr* create_var_expr(Parser* parser, Token* name) {
    Expr* expr = create_expr(parser);

    expr->type = EXPR_VAR;

    VarExpr varExpr;
    varExpr.name = name;
    expr->value.var = varExpr;

    return expr;
}

static Expr* create_primary_expr(Parser* parser, Token* value) {
    Expr* expr = create_expr(parser);

    expr->type = EXPR_PRIMARY;

    PrimaryExpr primaryExpr;
    primaryExpr.value = value;
    expr->value.primary = primaryExpr;

    return expr;
}

static Expr* create_assign_expr(Parser* parser, Token* name, Expr* value) {
    Expr* expr = create_expr(parser);

    expr->type = EXPR_ASSIGN;

    AssignExpr assignExpr;
    assignExpr.name = name;
    assignExpr.value = value;
    expr->value.assign = assignExpr;

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

    error_line(peek(parser)->line, message);
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
    if (match(parser, 6, TOKEN_FALSE, TOKEN_TRUE, TOKEN_NULL, TOKEN_NUMBER, TOKEN_STRING)) {
        return create_primary_expr(parser, previous(parser));
    }

    if (match(parser, 1, TOKEN_IDENTIFIER)) {
        return create_var_expr(parser, previous(parser));
    }

    if (match(parser, 1, TOKEN_LEFT_PARENTHESIS)) {
        Expr* expr = expression(parser);
        consume(parser, TOKEN_RIGHT_PARENTHESIS, "Closing parentheses expected.");
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

    while (match(parser, 3, TOKEN_SLASH, TOKEN_MODULO, TOKEN_STAR)) {
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

static Expr* assignment(Parser* parser) {
    Expr* expr = equality(parser);

    if (match(parser, 1, TOKEN_EQUAL)) {
        Token* equal_sign = previous(parser);
        Expr* value = assignment(parser);

        if (expr->type == EXPR_VAR) {
            VarExpr varExpr = expr->value.var;
            Token* name = varExpr.name;
            return create_assign_expr(parser, name, value);
        }

        error_token(equal_sign, "Invalid assignment target.");
    }

    return expr;
}

static Expr* expression(Parser* parser) {
    return assignment(parser);
}

static Stmt* declaration(Parser* parser);
static Stmt* statement(Parser* parser);

static Stmt* expr_statement(Parser* parser) {
    Expr* expr = expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Lines must be terminated with semicolons.");

    return create_stmt_expr(parser, expr);
}

static Stmt* print_statement(Parser* parser) {
    Expr* value = expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Lines must be terminated with semicolons.");

    return create_stmt_print(parser, value);
}

static Stmt* if_statement(Parser* parser) {
    consume(parser, TOKEN_LEFT_PARENTHESIS, "'(' expected after conditional.");
    Expr* condition = expression(parser);
    consume(parser, TOKEN_RIGHT_PARENTHESIS, "')' expected after conditional.");
    Stmt* body = statement(parser);

    return create_stmt_if(parser, condition, body);
}

static Stmt* while_statement(Parser* parser) {
    consume(parser, TOKEN_LEFT_PARENTHESIS, "'(' expected after loop.");
    Expr* condition = expression(parser);
    consume(parser, TOKEN_RIGHT_PARENTHESIS, "')' expected after loop.");
    Stmt* body = statement(parser);

    return create_stmt_while(parser, condition, body);
}

static Stmt* block_statement(Parser* parser) {
    Stmt** statements = (Stmt**) malloc(sizeof(Stmt*) * 100);
    int length = 0;

    while (!check(parser, TOKEN_RIGHT_BRACE) && !is_at_end(parser)) {
        statements[length++] = declaration(parser);
    }

    consume(parser, TOKEN_RIGHT_BRACE, "'}' expected to terminate block.");

    return create_stmt_block(parser, length, statements);
}

static Stmt* statement(Parser* parser) {
    if (match(parser, 1, TOKEN_IF)) return if_statement(parser);
    if (match(parser, 1, TOKEN_WHILE)) return while_statement(parser);
    if (match(parser, 1, TOKEN_PRINT)) return print_statement(parser);
    if (match(parser, 1, TOKEN_LEFT_BRACE)) return block_statement(parser);

    return expr_statement(parser);
}

static Stmt* var_declaration(Parser* parser) {
    Token* name = consume(parser, TOKEN_IDENTIFIER, "Variable name was expected.");

    Expr* initializer = NULL;
    if (match(parser, 1, TOKEN_EQUAL)) {
        initializer = expression(parser);
    }

    consume(parser, TOKEN_SEMICOLON, "Lines must be terminated with semicolons.");
    return create_stmt_var(parser, name, initializer);
}

static Stmt* declaration(Parser* parser) {
    if (match(parser, 1, TOKEN_VAR)) return var_declaration(parser);

    return statement(parser);
}

static void program(Parser* parser) {
    while (!is_at_end(parser)) {
        parser->program[parser->program_length++] = declaration(parser);
    }
}

AST parse(Token* tokens) {
    Parser parser;
    parser.current = 0;
    parser.tokens = tokens;
    parser.expressions_capacity = 100;
    parser.expressions = (Expr*) malloc(sizeof(Expr) * parser.expressions_capacity);
    parser.expressions_length = 0;
    parser.statements_capacity = 100;
    parser.statements = (Stmt*) malloc(sizeof(Stmt) * parser.statements_capacity);
    parser.statements_length = 0;
    parser.program_capacity = 100;
    parser.program = (Stmt**) malloc(sizeof(Stmt*) * parser.program_capacity);
    parser.program_length = 0;

    program(&parser);

    success("Complete!");

    AST ast;
    ast.nodes = parser.program;
    ast.length = parser.program_length;
    return ast;
}
