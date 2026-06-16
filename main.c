#include <stdio.h>
#include <stdlib.h>

char* read_file(char* path) {
    FILE *file_pointer = fopen(path, "r");

    fseek(file_pointer, 0, SEEK_END);
    long length = ftell(file_pointer);
    fseek(file_pointer, 0, SEEK_SET);

    char *buffer = (char*) calloc(length, sizeof(char));
    fread(buffer, sizeof(char), length, file_pointer);

    fclose(file_pointer);

    return buffer;
}

void main() {
    char* source = read_file("sample.txt");
}