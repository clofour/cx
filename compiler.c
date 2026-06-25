#include "compiler.h"
#include "parser.h"
#include "dynamic_buffer.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

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
    append_dynamic_buffer(dynamic_buffer, string, length);
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

    return label_index;
}

int emit_data(Compiler* compiler, char* value) {
    int data_index = compiler->unique_counter++;

    emit_inst(compiler->data, "dat%d db '%s', 0xd, 0xa, 0", data_index, value);

    return data_index;
}

ValueType compile_token(Compiler *compiler, Token *token)
{
    TokenType type = token->type;
    switch (type)
    {
        case TOKEN_IDENTIFIER: {
            char *identifier_value = token->value.identifier_value;
            Symbol variable = symbol_lookup(compiler->symbol_table, identifier_value);

            emit_inst(compiler->text, "mov rax, [rbp-%d]", variable.offset);
            return variable.type;
        }

        case TOKEN_STRING: {

            char *string_value = token->value.string_value;

            int data_index = emit_data(compiler, string_value);
            emit_inst(compiler->text, "lea rax, [dat%d]", data_index);

            return VALUE_STRING;
        }

        case TOKEN_NUMBER: {

            int number_value = token->value.float_value;
            emit_inst(compiler->text, "mov rax, %d", number_value);

            return VALUE_NUMBER;
        }

        case TOKEN_NONE: {
            printf("An error has occurred.");
            break;
        }

        default: {
            printf("TBD");
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

            char *variable_name = assign_expr.name->value.identifier_value;
            Symbol variable = symbol_lookup(compiler->symbol_table, variable_name);
            emit_inst(compiler->text, "mov [rbp-%d], rax", variable.offset);

            return VALUE_NONE;
        }

        case EXPR_BINARY:
        {
            BinaryExpr binary_expr = expr.value.binary;

            ValueType right_value = compile_expr(compiler, binary_expr.rightExpr);
            emit_inst(compiler->text, "push rax");
            ValueType left_value = compile_expr(compiler, binary_expr.leftExpr);
            emit_inst(compiler->text, "pop rcx");

            TokenType operator = binary_expr.operator->type;

            if (compare(operator, 5, TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_MODULO)) {
                switch (operator)
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

                return right_value == left_value && right_value;
            }

            if (compare(operator, 6, TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL)) {
                emit_inst(compiler->text, "cmp rax, rcx");
                
                switch (operator)
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

            break;
        }

        case EXPR_UNARY:
        {
            UnaryExpr unary_expr = expr.value.unary;

            ValueType value = compile_token(compiler, unary_expr.operator);
            compile_expr(compiler, unary_expr.expr);

            return value;
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
                data_index = emit_data(compiler, "%d");
                break;
            case VALUE_STRING:
                data_index = emit_data(compiler, "%s");
                break;
            case VALUE_BOOL:
                data_index = emit_data(compiler, "%d");
                break;
            }
            emit_inst(compiler->text, "lea rcx, [dat%d]", data_index);
            emit_inst(compiler->text, "call printf");

            break;
        }

        case STMT_COND:
        {
            StmtCond stmt_cond = stmt.value.cond;

            int end_label_index = allocate_label(compiler);

            compile_expr(compiler, stmt_cond.condition);
            emit_inst(compiler->text, "cmp rax, 0");
            emit_inst(compiler->text, "je end%d", end_label_index);
            
            compile_stmt(compiler, stmt_cond.body);
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

            enter_scope(compiler->symbol_table);
            for (int i = 0; i < stmt_block.length; i++)
            {
                compile_stmt(compiler, stmt_block.statements[i]);
            }
            exit_scope(compiler->symbol_table);

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
}

void compile(AST ast, char *path)
{
    Compiler compiler;
    compiler.path = path;
    compiler.data = create_dynamic_buffer(100);
    compiler.text = create_dynamic_buffer(100);
    compiler.symbol_table = create_symbol_table();
    compiler.unique_counter = 0;

    Compiler *compiler_pointer = &compiler;

    for (int i = 0; i < ast.length; i++)
    {
        compile_stmt(compiler_pointer, ast.nodes[i]);
    }

    write_asm(compiler_pointer);
}
