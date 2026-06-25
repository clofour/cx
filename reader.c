#include "reader.h"
#include "shared_data.h"
#include "reporter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Source source_create() {
    Source source = {0};
    
    return source;
}

void source_free(Source* source) {
    free(source->content);
}

Source* file_read(SharedData* shared_data, Source* source, char* path) {
    FILE *file_pointer = fopen(path, "rb");
    if (file_pointer == NULL) {
        error(shared_data->reporter, "File does not exist.");
    }

    fseek(file_pointer, 0, SEEK_END);
    long length = ftell(file_pointer);
    fseek(file_pointer, 0, SEEK_SET);

    char *buffer = (char*) calloc(length, sizeof(char));
    fread(buffer, sizeof(char), length, file_pointer);

    fclose(file_pointer);

    source->content = buffer;
    source->length = length;

    success(shared_data->reporter, "Complete!");

    return source;
}

char* file_get_name(char* path) {
    char* raw_file_name = strrchr(path, '/');
    return raw_file_name ? raw_file_name + 1 : path;
}
