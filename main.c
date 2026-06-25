#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "printer.h"
#include "compiler.h"
#include "shared.h"
#include "feedback.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_docs() {
    printf(
        "Usage: cx <input_file> <output_file>\n"
        "\n"
        "Compile cx source files into assembly.\n"
        "\n"
    );
}

void start_stage(char* name) {
    printf("[%s]\n", name);
}

void end_stage() {
    printf("\n");
}

int main(int argc, char* argv[]) {
    if ((argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) || argc != 3) {
        print_docs();
        return EXIT_FAILURE;
    }

    char* input_path = argv[1];
    char* output_path = argv[2];

    start_stage("reader");
    Source* source = read_file(input_path);
    end_stage();

    start_stage("scanner");
    Scanner scanner = scanner_create(source);
    Token* tokens = scan(&scanner);
    end_stage();

    start_stage("parser");
    Parser parser = parser_create(tokens);
    AST ast = parse(&parser);
    end_stage();

    // printf("AST:\n");
    // print(ast);

    start_stage("compiler");
    Compiler compiler = compiler_create(ast, output_path);
    compile(&compiler);
    end_stage();

    start_stage("cleanup");
    compiler_free(&compiler);
    parser_free(&parser);
    scanner_free(&scanner);
    success("Complete!");

    return EXIT_SUCCESS;
}
