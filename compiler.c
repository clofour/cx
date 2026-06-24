#include "compiler.h"
#include "parser.h"
#include "dynamic_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static Variable variable_lookup(Compiler* compiler, char* name) {
    for (int i = 0; i < compiler->variableCount; i++) {
        Variable variable = compiler->variables[i];

        if (strcmp(name, variable.name) == 0) {
            return variable;
        }
    }
}

static int variable_define(Compiler* compiler, char* name, ValueType type) {
    Variable variable;
    variable.name = name;
    variable.type = type;
    variable.offset = 8 * (compiler->variableCount + 1);

    compiler->variables[compiler->variableCount] = variable;
    compiler->variableCount += 1;

    return variable.offset;
}

void emit_inst(DynamicBuffer* dynamic_buffer, char* format, ...) {
    va_list args;
    va_start(args, format);

    int buffer_length = 128;
    char buffer[128] = {'\0'};
    strcat(buffer, "  ");
    int length = vsnprintf(buffer + 2, 128 - 3, format, args);
    strcat(buffer, "\n");
    append_dynamic_buffer(dynamic_buffer, buffer, length + 3);

    va_end(args);
}

ValueType compile_token(Compiler* compiler, Token* token) {
    TokenType type = token->type;
    switch (type) {
        case TOKEN_IDENTIFIER:

            char* identifier_value = token->value.identifier_value;
            Variable variable = variable_lookup(compiler, identifier_value);

            emit_inst(compiler->text, "mov rax, [rbp-%d]", variable.offset);
            return variable.type;

        case TOKEN_STRING:

            char* string_value = token->value.string_value;

            emit_inst(compiler->data, "msg db '%s', 0xd, 0xa, 0", string_value);
            emit_inst(compiler->text, "lea rax, [msg]");

            return VALUE_STRING;

        case TOKEN_NUMBER:

            int number_value = token->value.float_value;
            emit_inst(compiler->text, "mov rax, %d", number_value);

            return VALUE_NUMBER;

        case TOKEN_NONE: printf("An error has occurred."); break;
        default: printf("TBD"); break;
    }
}

ValueType compile_expr(Compiler* compiler, Expr* expr_pointer) {
    Expr expr = *expr_pointer;

    switch (expr.type) {
        case EXPR_VAR: {
            VarExpr varExpr = expr.value.var;

            return compile_token(compiler, varExpr.name);
        }

        case EXPR_ASSIGN: {

            AssignExpr assignExpr = expr.value.assign;

            ValueType value = compile_expr(compiler, assignExpr.value);

            char* variable_name = assignExpr.name->value.identifier_value;
            int offset = variable_define(compiler, variable_name, value);
            emit_inst(compiler->text, "mov [rbp-%d], rax", offset);

            return VALUE_NONE;

        }

        case EXPR_BINARY: {
            BinaryExpr binaryExpr = expr.value.binary;

            ValueType rightValue = compile_expr(compiler, binaryExpr.rightExpr);
            emit_inst(compiler->text, "push rax");
            ValueType leftValue = compile_expr(compiler, binaryExpr.leftExpr);
            emit_inst(compiler->text, "pop rcx");

            switch (binaryExpr.operator->type) {
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
            }

            return rightValue == leftValue && rightValue;
        }

        case EXPR_UNARY: {
            UnaryExpr unaryExpr = expr.value.unary;

            ValueType value = compile_token(compiler, unaryExpr.operator);
            compile_expr(compiler, unaryExpr.expr);
            
            return value;
        }

        case EXPR_PRIMARY: {

                        PrimaryExpr primaryExpr = expr.value.primary;

            return compile_token(compiler, primaryExpr.value);
        }
    }
}

void compile_stmt(Compiler* compiler, Stmt* stmt_pointer) {
    Stmt stmt = *stmt_pointer;

    switch(stmt.type) {
        case STMT_EXPR: {
            StmtExpr stmtExpr = stmt.value.expr;
            compile_expr(compiler, stmtExpr.expr);

            break;

        }

        case STMT_PRINT: {
            StmtPrint stmtPrint = stmt.value.print;

            ValueType value = compile_expr(compiler, stmtPrint.expr);
            emit_inst(compiler->text, "mov rdx, rax");
            switch (value) {
                case VALUE_NUMBER:
                    emit_inst(compiler->data, "format_string db '%s', 0xd, 0xa, 0", "%d");
                    break;
                case VALUE_STRING:
                    emit_inst(compiler->data, "format_string db '%s', 0xd, 0xa, 0", "%s");
                    break;
            }
            emit_inst(compiler->text, "lea rcx, [format_string]");
            emit_inst(compiler->text, "call printf");

            break;
        }

        case STMT_VAR: {
            StmtVar stmtVar = stmt.value.var;

            ValueType value = compile_expr(compiler, stmtVar.expr);

            char* variable_name = stmtVar.name->value.identifier_value;
            int offset = variable_define(compiler, variable_name, value);
            emit_inst(compiler->text, "mov [rbp-%d], rax", offset);

            break;
        }
    }
}

void write_asm(Compiler* compiler) {
    FILE* file_pointer = fopen(compiler->path, "w");
    fprintf(file_pointer,
        "bits 64\n"
        "default rel\n"
    );

    fprintf(file_pointer,
        "segment .data\n"
    );
    fprintf(file_pointer, "%s", compiler->data->buffer);

    fprintf(file_pointer,
        "segment .text\n"
        "global main\n"
        "extern printf\n"
        "extern ExitProcess\n"
    );

    fprintf(file_pointer,
        "main:\n"
        "  push rbp\n"
        "  mov rbp, rsp\n"
        "  sub rsp, %d\n",
        32 + 8 * (compiler->variableCount)
    );
    fprintf(file_pointer, "%s", compiler->text->buffer);
    fprintf(file_pointer,
        "  xor rax, rax\n"
        "  call ExitProcess\n"
    );
}

void compile(AST ast, char* path) {
    Compiler compiler;
    compiler.path = path;
    compiler.data = create_dynamic_buffer(100);
    compiler.text = create_dynamic_buffer(100);
    compiler.variables = malloc(sizeof(Variable) * 256);
    compiler.variableCount = 0;

    Compiler* compiler_pointer = &compiler;

    for (int i = 0; i < ast.length; i++) {
        compile_stmt(compiler_pointer, &ast.statements[i]);
    }

    write_asm(compiler_pointer);
}
