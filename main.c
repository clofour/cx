#include "shared.h"
#include "scanner.h"
#include "parser.h"
#include "printer.h"
#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_logo() {
    printf(
        " ██████╗██╗  ██╗    \n"
        "██╔════╝╚██╗██╔╝    \n"
        "██║      ╚███╔╝     \n"
        "██║      ██╔██╗     \n"
        "╚██████╗██╔╝ ██╗    \n"
        " ╚═════╝╚═╝  ╚═╝    \n"
    );
}

void print_docs() {
    printf(
        "Usage: cx <input_file> <output_file>\n"
        "\n"
        "Compile cx source files into assembly.\n"
        "\n"
    );
}

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

void main(int argc, char* argv[]) {
    print_logo();

    if ((argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) || argc != 3) {
        print_docs();
        return;
    }

    char* input_path = argv[1];
    char* output_path = argv[2];

    Source* source = read_file(input_path);
    printf("Source:\n");
    printf(source->content);
    printf("\n");

    Token* tokens = scan(source);

    AST ast = parse(tokens);

    printf("AST:\n");
    print(ast);

    compile(ast, output_path);
}
