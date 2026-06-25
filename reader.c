#include "reader.h"
#include "feedback.h"
#include <stdlib.h>
#include <stdio.h>

Source* read_file(char* path) {
    FILE *file_pointer = fopen(path, "rb");
    if (file_pointer == NULL) {
        error("File does not exist.");
        exit(EXIT_FAILURE);
    }

    fseek(file_pointer, 0, SEEK_END);
    long length = ftell(file_pointer);
    fseek(file_pointer, 0, SEEK_SET);

    char *buffer = (char*) calloc(length, sizeof(char));
    fread(buffer, sizeof(char), length, file_pointer);

    fclose(file_pointer);

    Source* source = (Source*) malloc(sizeof(Source));
    source->content = buffer;
    source->length = length;

    success("Complete!");

    return source;
}
