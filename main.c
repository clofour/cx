#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "printer.h"
#include "compiler.h"
#include "keywords.h"
#include "shared_data.h"
#include "reporter.h"
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
    char* input_file_name = file_get_name(input_path);
    char* output_path = argv[2];

    SharedData shared_data;
    Reporter reporter = reporter_create(input_file_name);
    shared_data.reporter = &reporter;

    start_stage("reader");
    Source source = source_create(shared_data);
    file_read(&shared_data, &source, input_path);
    end_stage();

    start_stage("scanner");
    Scanner scanner = scanner_create(&shared_data, &source);
    Token* tokens = scan(&scanner);
    end_stage();

    start_stage("parser");
    Parser parser = parser_create(&shared_data, tokens);
    AST ast = parse(&parser);
    end_stage();

    // printf("AST:\n");
    // print(ast);

    start_stage("compiler");
    Compiler compiler = compiler_create(&shared_data, ast, output_path);
    compile(&compiler);
    end_stage();

    start_stage("cleanup");
    compiler_free(&compiler);
    parser_free(&parser);
    scanner_free(&scanner);
    source_free(&source);
    
    success(&reporter, "Complete!");

    return EXIT_SUCCESS;
}
