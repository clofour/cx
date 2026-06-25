#include "compiler.h"
#include "parser.h"
#include "dynamic_buffer.h"
#include "symbol_table.h"
#include "reporter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

static Symbol* lookup_symbol_token(Compiler* compiler, Token* token) {
    if (token->type != TOKEN_IDENTIFIER) {
        error_token(compiler->shared_data->reporter, token, "Unexpected token type");
    }

    Symbol* symbol = symbol_lookup(compiler->symbol_table, token->value.identifier_value);
    if (symbol != NULL) {
        return symbol;
    } else {
        error_token(compiler->shared_data->reporter, token, "Unrecognized symbol.");
    }
}

static bool compare(TokenType reference, int count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        if (reference == va_arg(args, TokenType)) {
            va_end(args);
            return true;
        }
    }

    va_end(args);
    return false;
}

static int allocate_label(Compiler* compiler) {
    return compiler->unique_counter++;
}

void emit(DynamicBuffer* dynamic_buffer, char* string, int length) {
    dynamic_buffer_append(dynamic_buffer, string, length);
}

void emit_inst(DynamicBuffer *dynamic_buffer, char *format, ...)
{
    va_list args;
    va_start(args, format);

    char raw[128];
    char processed[128];
    vsnprintf(raw, 128, format, args);
    int length = snprintf(processed, 128, "  %s\n", raw);
    
    emit(dynamic_buffer, processed, length);

    va_end(args);
}

void emit_label(Compiler* compiler, int label_index, char* value) {
    emit_inst(compiler->text, "%s%d:", value, label_index);
}

int emit_data(Compiler* compiler, char* value, bool new_line) {
    int data_index = compiler->unique_counter++;

    if (new_line) {
        emit_inst(compiler->data, "dat%d db '%s', 0xd, 0xa, 0", data_index, value);
    } else {
        emit_inst(compiler->data, "dat%d db '%s', 0", data_index, value);
    }
    

    return data_index;
}

ValueType compile_token(Compiler *compiler, Token *token)
{
    TokenType type = token->type;
    switch (type)
    {
        case TOKEN_IDENTIFIER: {
            Symbol* variable = lookup_symbol_token(compiler, token);

            emit_inst(compiler->text, "mov rax, [rbp-%d]", variable->offset);
            return variable->type;
        }

        case TOKEN_STRING: {

            char *string_value = token->value.string_value;

            int data_index = emit_data(compiler, string_value, false);
            emit_inst(compiler->text, "lea rax, [dat%d]", data_index);

            return VALUE_STRING;
        }

        case TOKEN_NUMBER: {

            int number_value = token->value.float_value;
            emit_inst(compiler->text, "mov rax, %d", number_value);

            return VALUE_NUMBER;
        }

        case TOKEN_NONE:
        default:
        {
            error_token(compiler->shared_data->reporter, token, "Unrecognized token.");
            break;
        }

    }
}

ValueType compile_expr(Compiler *compiler, Expr *expr_pointer)
{
    Expr expr = *expr_pointer;

    switch (expr.type)
    {
        case EXPR_VAR:
        {
            VarExpr var_expr = expr.value.var;

            return compile_token(compiler, var_expr.name);
        }

        case EXPR_ASSIGN:
        {

            AssignExpr assign_expr = expr.value.assign;

            ValueType value = compile_expr(compiler, assign_expr.value);

            Symbol* variable = lookup_symbol_token(compiler, assign_expr.name);
            emit_inst(compiler->text, "mov [rbp-%d], rax", variable->offset);

            return VALUE_NONE;
        }

        case EXPR_BINARY:
        {
            BinaryExpr binary_expr = expr.value.binary;

            ValueType right_value = compile_expr(compiler, binary_expr.right_expr);
            emit_inst(compiler->text, "push rax");
            ValueType left_value = compile_expr(compiler, binary_expr.left_expr);
            emit_inst(compiler->text, "pop rcx");

            Token* operator = binary_expr.operator;
            TokenType operator_type = operator->type;

            if (compare(operator_type, 5, TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_MODULO)) {
                if (!(right_value == VALUE_NUMBER && left_value == VALUE_NUMBER)) {
                    error_token(compiler->shared_data->reporter, operator, "Incorrect types for arithmetic binary operation.");
                }

                switch (operator_type)
                {
                    case TOKEN_PLUS:
                        emit_inst(compiler->text, "add rax, rcx");
                        break;
                    case TOKEN_MINUS:
                        emit_inst(compiler->text, "sub rax, rcx");
                        break;
                    case TOKEN_STAR:
                        emit_inst(compiler->text, "imul rax, rcx");
                        break;
                    case TOKEN_SLASH:
                        emit_inst(compiler->text, "cqo");
                        emit_inst(compiler->text, "idiv rcx");
                        break;
                    case TOKEN_MODULO:
                        emit_inst(compiler->text, "cqo");
                        emit_inst(compiler->text, "idiv rcx");
                        emit_inst(compiler->text, "mov rax, rdx");

                }

                return VALUE_NUMBER;
            }

            if (compare(operator_type, 6, TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL)) {
                emit_inst(compiler->text, "cmp rax, rcx");
                
                switch (operator_type)
                {
                    case TOKEN_EQUAL_EQUAL:
                        emit_inst(compiler->text, "sete al");
                        break;
                    case TOKEN_BANG_EQUAL:
                        emit_inst(compiler->text, "setne al");
                        break;
                    case TOKEN_LESS:
                        emit_inst(compiler->text, "setl al");
                        break;
                    case TOKEN_LESS_EQUAL:
                        emit_inst(compiler->text, "setle al");
                        break;
                    case TOKEN_GREATER:
                        emit_inst(compiler->text, "setg al");
                        break;
                    case TOKEN_GREATER_EQUAL:
                        emit_inst(compiler->text, "setge al");
                        break;
                }

                emit_inst(compiler->text, "movzx eax, al");

                return VALUE_BOOL;
            }

            error_token(compiler->shared_data->reporter, operator, "Unexpected operator.");
            break;
        }

        case EXPR_UNARY:
        {
            UnaryExpr unary_expr = expr.value.unary;

            compile_expr(compiler, unary_expr.expr);

            switch (unary_expr.operator->type) {
                case TOKEN_BANG: {
                    emit_inst(compiler->text, "cmp rax, 0");
                    emit_inst(compiler->text, "sete al");
                    emit_inst(compiler->text, "movzx rax, al");

                    return VALUE_BOOL;
                }
                case TOKEN_MINUS: {
                    emit_inst(compiler->text, "neg rax");

                    return VALUE_NUMBER;
                }
            }

            return VALUE_NONE;
        }

        case EXPR_PRIMARY:
        {
            PrimaryExpr primary_expr = expr.value.primary;

            return compile_token(compiler, primary_expr.value);
        }
    }
}

void compile_stmt(Compiler *compiler, Stmt *stmt_pointer)
{
    Stmt stmt = *stmt_pointer;

    switch (stmt.type)
    {
        case STMT_EXPR:
        {
            StmtExpr stmt_expr = stmt.value.expr;
            compile_expr(compiler, stmt_expr.expr);

            break;
        }

        case STMT_PRINT:
        {
            StmtPrint stmt_print = stmt.value.print;

            ValueType value = compile_expr(compiler, stmt_print.expr);
            emit_inst(compiler->text, "mov rdx, rax");

            int data_index;
            switch (value)
            {
                case VALUE_NUMBER:
                    data_index = emit_data(compiler, "%d", true);
                    break;
                case VALUE_STRING:
                    data_index = emit_data(compiler, "%s", true);
                    break;
                case VALUE_BOOL:
                    data_index = emit_data(compiler, "%d", true);
                    break;
                case VALUE_NONE:
                    error_expr(compiler->shared_data->reporter, stmt_print.expr, "Unrecognized value.");
                    break;
            }
            emit_inst(compiler->text, "lea rcx, [dat%d]", data_index);
            emit_inst(compiler->text, "call printf");

            break;
        }

        case STMT_COND:
        {
            StmtCond stmt_cond = stmt.value.cond;

            bool has_else = stmt_cond.else_body != NULL;

            int else_label_index = allocate_label(compiler);
            int end_label_index = allocate_label(compiler);

            compile_expr(compiler, stmt_cond.condition);
            emit_inst(compiler->text, "cmp rax, 0");
            if (has_else) {
                emit_inst(compiler->text, "je else%d", else_label_index);
            } else {
                emit_inst(compiler->text, "je end%d", end_label_index);
            }
            
            compile_stmt(compiler, stmt_cond.then_body);
            if (has_else) {
                emit_inst(compiler->text, "jmp end%d", end_label_index);
                emit_label(compiler, else_label_index, "else");
                compile_stmt(compiler, stmt_cond.else_body);
            }
            emit_label(compiler, end_label_index, "end");

            break;
        }

        case STMT_LOOP:
        {
            StmtLoop stmt_loop = stmt.value.loop;

            int body_label_index = allocate_label(compiler);
            int test_label_index = allocate_label(compiler);

            emit_inst(compiler->text, "jmp test%d", test_label_index);

            emit_label(compiler, body_label_index, "body");
            compile_stmt(compiler, stmt_loop.body);

            emit_label(compiler, test_label_index, "test");
            compile_expr(compiler, stmt_loop.condition);
            emit_inst(compiler->text, "cmp rax, 0");
            emit_inst(compiler->text, "jne body%d", body_label_index);

            break;
        }

        case STMT_BLOCK:
        {
            StmtBlock stmt_block = stmt.value.block;

            symbol_table_scope_enter(compiler->symbol_table);
            for (int i = 0; i < stmt_block.length; i++)
            {
                compile_stmt(compiler, stmt_block.statements[i]);
            }
            symbol_table_scope_exit(compiler->symbol_table);

            break;
        }

        case STMT_VAR:
        {
            StmtVar stmt_var = stmt.value.var;

            ValueType value = compile_expr(compiler, stmt_var.expr);

            char *variable_name = stmt_var.name->value.identifier_value;
            int offset = symbol_define(compiler->symbol_table, variable_name, value);
            emit_inst(compiler->text, "mov [rbp-%d], rax", offset);

            break;
        }
    }
}

void write_asm(Compiler *compiler)
{
    int stack_space = 32 + 8 * (compiler->symbol_table->symbol_count);
    if (stack_space % 16 != 0) {
        stack_space = 16 * (stack_space / 16 + 1);
    }

    FILE *file_pointer = fopen(compiler->path, "w");
    fprintf(file_pointer,
            "bits 64\n"
            "default rel\n");

    fprintf(file_pointer,
            "segment .data\n");
    fprintf(file_pointer, "%s", compiler->data->buffer);

    fprintf(file_pointer,
            "segment .text\n"
            "global main\n"
            "extern printf\n"
            "extern ExitProcess\n");

    fprintf(file_pointer,
            "main:\n"
            "  push rbp\n"
            "  mov rbp, rsp\n"
            "  sub rsp, %d\n",
            stack_space);
    fprintf(file_pointer, "%s", compiler->text->buffer);
    fprintf(file_pointer,
            "  xor rax, rax\n"
            "  call ExitProcess\n");

    fclose(file_pointer);
}

Compiler compiler_create(SharedData* shared_data, AST ast, char *path) {
    Compiler compiler;
    compiler.shared_data = shared_data;
    compiler.ast = ast;
    compiler.path = path;
    compiler.data = dynamic_buffer_create(100);
    compiler.text = dynamic_buffer_create(100);
    compiler.symbol_table = symbol_table_create();
    compiler.unique_counter = 0;

    return compiler;
}

void compiler_free(Compiler* compiler) {
    dynamic_buffer_free(compiler->data);
    dynamic_buffer_free(compiler->text);
    symbol_table_free(compiler->symbol_table);
}

void compile(Compiler* compiler) {
    AST ast = compiler->ast;
    for (int i = 0; i < ast.length; i++)
    {
        compile_stmt(compiler, ast.nodes[i]);
    }

    write_asm(compiler);

    success(compiler->shared_data->reporter, "Complete!");
}
