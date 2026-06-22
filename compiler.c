#include "compiler.h"
#include "parser.h"
#include "dynamic_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void emit(DynamicBuffer dynamic_buffer, char* format, ...) {
    va_list args;
    va_start(args, format);

    int buffer_length = 128;
    char buffer[128];
    int length = vsnprintf(buffer, 128, format, args);
    append_dynamic_buffer(&dynamic_buffer, buffer, length);

    va_end(args);
}

void compile_token(Compiler* compiler, Token* token) {
    TokenType type = token->type;
    switch (type) {
        case TOKEN_IDENTIFIER:
            printf("TBD");
            break;
        case TOKEN_STRING:
            char* string_value = token->value.string_value;
            emit(compiler->data, "msg db '%s', 0xd, 0xa, 0\n", string_value);
            break;
        case TOKEN_NUMBER:
            int number_value = token->value.float_value;
            emit(compiler->text, "mov rax, %d", number_value);
            break;
        case TOKEN_PLUS: printf("+"); break;
        case TOKEN_MINUS: printf("-"); break;
        case TOKEN_SLASH: printf("/"); break;
        case TOKEN_STAR: printf("*"); break;
        case TOKEN_NONE: printf("An error has occurred."); break;
        default: printf("TBD"); break;
    }

   printf("\n");
}

void compile_expr(Compiler* compiler, Expr* expr_pointer) {
    Expr expr = *expr_pointer;

    switch (expr.type) {
        case EXPR_VAR:
            VarExpr varExpr = expr.value.var;

            compile_token(compiler, varExpr.name);

            return;

        case EXPR_ASSIGN:
            AssignExpr assignExpr = expr.value.assign;

            compile_token(compiler, assignExpr.name);
            print_expr(compiler, assignExpr.value);

            return;

        case EXPR_BINARY:
            BinaryExpr binaryExpr = expr.value.binary;

            compile_token(compiler, binaryExpr.operator);

            print_expr(compiler, binaryExpr.leftExpr);
            print_expr(compiler, binaryExpr.rightExpr);

            return;

        case EXPR_UNARY:
            UnaryExpr unaryExpr = expr.value.unary;

            compile_token(compiler, unaryExpr.operator);
            print_expr(compiler, unaryExpr.expr);
            
            return;

        case EXPR_PRIMARY:
            PrimaryExpr primaryExpr = expr.value.primary;

            compile_token(compiler, primaryExpr.value);
            return;
    }
}

void compile_stmt(Compiler* compiler, Stmt* stmt_pointer) {
    Stmt stmt = *stmt_pointer;

    switch(stmt.type) {
        case STMT_EXPR:
            StmtExpr stmtExpr = stmt.value.expr;
            compile_expr(compiler, stmtExpr.expr);

            break;

        case STMT_PRINT:
            StmtPrint stmtPrint = stmt.value.print;

            compile_expr(compiler, stmtPrint.expr);
            emit(compiler->text, 
                "lea rcx, [msg]" "call printf\n"
            );

            break;

        case STMT_VAR:
            StmtVar stmtVar = stmt.value.var;

            compile_token(compiler, stmtVar.name);
            print_expr(compiler, stmtVar.expr);

            break;
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
    fprintf(file_pointer, compiler->data.buffer);

    fprintf(file_pointer,
        "segment .text\n"
        "global main\n"
        "extern printf\n"
        "extern ExitProcess\n"
    );

    fprintf(file_pointer,
        "main:\n"
        "push rbp\n"
        "mov rbp, rsp\n"
        "sub rsp, 32\n"
    );
    fprintf(file_pointer, compiler->data.buffer);
    fprintf(file_pointer,
        "xor rax, rax\n"
        "call ExitProcess\n"
    );
}

void compile(AST ast, char* path) {
    Compiler compiler;
    compiler.path = path;
    compiler.data = create_dynamic_buffer(100);
    compiler.text = create_dynamic_buffer(100);

    Compiler* compiler_pointer = &compiler;

    for (int i = 0; i < 2; i++) {
        compile_stmt(compiler_pointer, &ast.statements[i]);
    }

    write_asm(compiler_pointer);
}
