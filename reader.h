#ifndef READER_H
#define READER_H

typedef struct Source {
    char* content;
    int length;
} Source;

Source source_create();
void source_free(Source* source);
Source* read_file(Source* source, char* path);

#endif