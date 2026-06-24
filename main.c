#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "scanner.h"
#include "parser.h"
#include "printer.h"
#include "compiler.h"

Source* read_file(char* path) {
    FILE *file_pointer = fopen(path, "r");

    fseek(file_pointer, 0, SEEK_END);
    long length = ftell(file_pointer);
    fseek(file_pointer, 0, SEEK_SET);

    char *buffer = (char*) calloc(length, sizeof(char));
    fread(buffer, sizeof(char), length, file_pointer);

    fclose(file_pointer);

    Source* source = (Source*) malloc(sizeof(Source));
    source->content = buffer;
    source->length = length;

    return source;
}

void main() {
    Source* source = read_file("samples/complex_arithmetic.cx");
    printf("Source:\n");
    printf(source->content);
    printf("\n");

    Token* tokens = scan(source);

    AST ast = parse(tokens);

    printf("AST:\n");
    print(ast);

    compile(ast, "outputs/output.asm");
}
