#include "parser.h"
#include "scanner.h"
#include "reporter.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

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

    error_line(parser->shared_data->reporter, get_line(parser), message);
    return NULL;
}

static int get_line(Parser* parser) {
    return previous(parser)->line;
}

static Stmt* stmt_create(Parser* parser) {
    Stmt stmt = {0};

    parser->statements[parser->statements_length++] = stmt;
    return &parser->statements[parser->statements_length - 1];
}

static Stmt* stmt_expr_create(Parser* parser, Expr* expr) {
    Stmt* stmt = stmt_create(parser);
    
    stmt->type = STMT_EXPR;

    StmtExpr stmt_expr;
    stmt_expr.expr = expr;
    stmt->value.expr = stmt_expr;

    return stmt;
}

static Stmt* stmt_print_create(Parser* parser, Expr* expr) {
    Stmt* stmt = stmt_create(parser);
    
    stmt->type = STMT_PRINT;

    StmtPrint stmt_print;
    stmt_print.expr = expr;
    stmt->value.print = stmt_print;

    return stmt;
}

static Stmt* stmt_cond_create(Parser* parser, Expr* condition, Stmt* then_body, Stmt* else_body) {
    Stmt* stmt = stmt_create(parser);
    
    stmt->type = STMT_COND;

    StmtCond stmt_cond;
    stmt_cond.condition = condition;
    stmt_cond.then_body = then_body;
    stmt_cond.else_body = else_body;
    stmt->value.cond = stmt_cond;

    return stmt;
}

static Stmt* stmt_while_create(Parser* parser, Expr* condition, Stmt* body) {
    Stmt* stmt = stmt_create(parser);
    
    stmt->type = STMT_LOOP;

    StmtLoop stmt_loop;
    stmt_loop.condition = condition;
    stmt_loop.body = body;
    stmt->value.loop = stmt_loop;

    return stmt;
}

static Stmt* stmt_block_create(Parser* parser, int length, Stmt** statements) {
    Stmt* stmt = stmt_create(parser);
    
    stmt->type = STMT_BLOCK;

    StmtBlock stmt_block;
    stmt_block.length = length;
    stmt_block.statements = statements;
    stmt->value.block = stmt_block;

    return stmt;
}

static Stmt* stmt_var_create(Parser* parser, Token* name, Expr* expr) {
    Stmt* stmt = stmt_create(parser);
    
    stmt->type = STMT_VAR;

    StmtVar stmt_var;
    stmt_var.name = name;
    stmt_var.expr = expr;
    stmt->value.var = stmt_var;

    return stmt;
}

static Expr* expr_create(Parser* parser) {
    Expr expr = {0};
    expr.line = get_line(parser);
    parser->expressions[parser->expressions_length++] = expr;
    return &parser->expressions[parser->expressions_length - 1];
}

static Expr* expr_var_create(Parser* parser, Token* name) {
    Expr* expr = expr_create(parser);

    expr->type = EXPR_VAR;

    ExprVar expr_var;
    expr_var.name = name;
    expr->value.var = expr_var;

    return expr;
}

static Expr* expr_primary_create(Parser* parser, Token* value) {
    Expr* expr = expr_create(parser);

    expr->type = EXPR_PRIMARY;

    ExprPrimary expr_primary;
    expr_primary.value = value;
    expr->value.primary = expr_primary;

    return expr;
}

static Expr* expr_assign_create(Parser* parser, Token* name, Expr* value) {
    Expr* expr = expr_create(parser);

    expr->type = EXPR_ASSIGN;

    ExprAssign expr_assign;
    expr_assign.name = name;
    expr_assign.value = value;
    expr->value.assign = expr_assign;

    return expr;
}

static Expr* expr_unary_create(Parser* parser, Token* operator, Expr* expression) {
    Expr* expr = expr_create(parser);

    expr->type = EXPR_UNARY;

    ExprUnary expr_unary;
    expr_unary.operator = operator;
    expr_unary.expr = expression;
    expr->value.unary = expr_unary;

    return expr;
}

static Expr* expr_binary_create(Parser* parser, Expr* left_expr, Token* operator, Expr* right_expr) {
    Expr* expr = expr_create(parser);

    expr->type = EXPR_BINARY;

    ExprBinary expr_binary;
    expr_binary.left_expr = left_expr;
    expr_binary.operator = operator;
    expr_binary.right_expr = right_expr;
    expr->value.binary = expr_binary;

    return expr;
}

static Stmt* declaration(Parser* parser);
static Stmt* statement(Parser* parser);
static Expr* expression(Parser* parser);
static Expr* equality(Parser* parser);
static Expr* assignment(Parser* parser);
static Expr* comparison(Parser* parser);
static Expr* term(Parser* parser);
static Expr* factor(Parser* parser);
static Expr* unary(Parser* parser);
static Expr* primary(Parser* parser);

static Expr* primary(Parser* parser) {
    if (match(parser, 6, TOKEN_FALSE, TOKEN_TRUE, TOKEN_NULL, TOKEN_NUMBER, TOKEN_STRING)) {
        return expr_primary_create(parser, previous(parser));
    }

    if (match(parser, 1, TOKEN_IDENTIFIER)) {
        return expr_var_create(parser, previous(parser));
    }

    if (match(parser, 1, TOKEN_LEFT_PARENTHESIS)) {
        Expr* expr = expression(parser);
        consume(parser, TOKEN_RIGHT_PARENTHESIS, "Closing parentheses expected.");
        return expr;
    }

    error_line(parser->shared_data->reporter, get_line(parser), "Expected expression.");
}

static Expr* unary(Parser* parser) {
    if (match(parser, 2, TOKEN_BANG, TOKEN_MINUS)) {
        Token* operator = previous(parser);
        Expr* expr = unary(parser);
        return expr_unary_create(parser, operator, expr);
    }

    return primary(parser);
}

static Expr* factor(Parser* parser) {
    Expr* expr = unary(parser);

    while (match(parser, 3, TOKEN_SLASH, TOKEN_MODULO, TOKEN_STAR)) {
        Token* operator = previous(parser);
        Expr* right = unary(parser);
        expr = expr_binary_create(parser, expr, operator, right);
    }

    return expr;
}

static Expr* term(Parser* parser) {
    Expr* expr = factor(parser);

    while (match(parser, 2, TOKEN_MINUS, TOKEN_PLUS)) {
        Token* operator = previous(parser);
        Expr* right = factor(parser);
        expr = expr_binary_create(parser, expr, operator, right);
    }

    return expr;
}

static Expr* comparison(Parser* parser) {
    Expr* expr = term(parser);

    while (match(parser, 4, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL)) {
        Token* operator = previous(parser);
        Expr* right = term(parser);
        expr = expr_binary_create(parser, expr, operator, right);
    }

    return expr;
}

static Expr* equality(Parser* parser) {
    Expr* expr = comparison(parser);

    while (match(parser, 2, TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL)) {
        Token* operator = previous(parser);
        Expr* right = comparison(parser);
        expr = expr_binary_create(parser, expr, operator, right);
    }

    return expr;
}

static Expr* assignment(Parser* parser) {
    Expr* expr = equality(parser);

    if (match(parser, 1, TOKEN_EQUAL)) {
        Token* equal_sign = previous(parser);
        Expr* value = assignment(parser);

        if (expr->type == EXPR_VAR) {
            ExprVar expr_var = expr->value.var;
            Token* name = expr_var.name;
            return expr_assign_create(parser, name, value);
        }

        error_token(parser->shared_data->reporter, equal_sign, "Invalid assignment target.");
    }

    return expr;
}

static Expr* expression(Parser* parser) {
    return assignment(parser);
}

static Stmt* statement_expr(Parser* parser) {
    Expr* expr = expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Lines must be terminated with semicolons.");

    return stmt_expr_create(parser, expr);
}

static Stmt* statement_print(Parser* parser) {
    Expr* value = expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Lines must be terminated with semicolons.");

    return stmt_print_create(parser, value);
}

static Stmt* statement_cond(Parser* parser) {
    consume(parser, TOKEN_LEFT_PARENTHESIS, "'(' expected after conditional.");
    Expr* condition = expression(parser);
    consume(parser, TOKEN_RIGHT_PARENTHESIS, "')' expected after conditional.");
    Stmt* then_body = statement(parser);

    Stmt* else_body = NULL;
    if (match(parser, 1, TOKEN_ELSE)) {
        else_body = statement(parser);
    }

    return stmt_cond_create(parser, condition, then_body, else_body);
}

static Stmt* statement_while(Parser* parser) {
    consume(parser, TOKEN_LEFT_PARENTHESIS, "'(' expected after loop.");
    Expr* condition = expression(parser);
    consume(parser, TOKEN_RIGHT_PARENTHESIS, "')' expected after loop.");
    Stmt* body = statement(parser);

    return stmt_while_create(parser, condition, body);
}

static Stmt* statement_block(Parser* parser) {
    Stmt** statements = (Stmt**) malloc(sizeof(Stmt*) * 100);
    parser->blocks[parser->blocks_length++] = statements;

    int length = 0;

    while (!check(parser, TOKEN_RIGHT_BRACE) && !is_at_end(parser)) {
        statements[length++] = declaration(parser);
    }

    consume(parser, TOKEN_RIGHT_BRACE, "'}' expected to terminate block.");

    return stmt_block_create(parser, length, statements);
}

static Stmt* statement(Parser* parser) {
    if (match(parser, 1, TOKEN_IF)) return statement_cond(parser);
    if (match(parser, 1, TOKEN_WHILE)) return statement_while(parser);
    if (match(parser, 1, TOKEN_PRINT)) return statement_print(parser);
    if (match(parser, 1, TOKEN_LEFT_BRACE)) return statement_block(parser);

    return statement_expr(parser);
}

static Stmt* declaration_var(Parser* parser) {
    Token* name = consume(parser, TOKEN_IDENTIFIER, "Variable name was expected.");

    Expr* initializer = NULL;
    if (match(parser, 1, TOKEN_EQUAL)) {
        initializer = expression(parser);
    }

    consume(parser, TOKEN_SEMICOLON, "Lines must be terminated with semicolons.");
    return stmt_var_create(parser, name, initializer);
}

static Stmt* declaration(Parser* parser) {
    if (match(parser, 1, TOKEN_VAR)) return declaration_var(parser);

    return statement(parser);
}

static void program(Parser* parser) {
    while (!is_at_end(parser)) {
        parser->program[parser->program_length++] = declaration(parser);
    }
}

Parser parser_create(SharedData* shared_data, Token* tokens) {
    Parser parser;
    parser.shared_data = shared_data;
    parser.current = 0;
    parser.tokens = tokens;
    parser.expressions_capacity = 100;
    parser.expressions = (Expr*) malloc(sizeof(Expr) * parser.expressions_capacity);
    parser.expressions_length = 0;
    parser.statements_capacity = 100;
    parser.statements = (Stmt*) malloc(sizeof(Stmt) * parser.statements_capacity);
    parser.statements_length = 0;
    parser.blocks_capacity = 100;
    parser.blocks = (Stmt***) malloc(sizeof(Stmt**) * parser.blocks_capacity);
    parser.blocks_length = 0;
    parser.program_capacity = 100;
    parser.program = (Stmt**) malloc(sizeof(Stmt*) * parser.program_capacity);
    parser.program_length = 0;

    return parser;
}

void parser_free(Parser* parser) {
    free(parser->expressions);

    free(parser->statements);

    for (int i = 0; i < parser->blocks_length; i++) {
        free(parser->blocks[i]);
    }
    free(parser->blocks);

    free(parser->program);
}

AST parse(Parser* parser) {
    program(parser);

    success(parser->shared_data->reporter, "Complete!");

    AST ast;
    ast.nodes = parser->program;
    ast.length = parser->program_length;
    return ast;
}
