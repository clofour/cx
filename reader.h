#ifndef READER_H
#define READER_H

typedef struct Source {
    char* content;
    int length;
} Source;

Source* read_file(char* path);

#endif