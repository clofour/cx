#ifndef READER_H
#define READER_H

#include "shared_data.h"

typedef struct Source {
    SharedData* shared_data;
    char* content;
    int length;
} Source;

Source source_create();
void source_free(Source* source);
Source* file_read(SharedData* shared_data, Source* source, char* path);
char* file_get_name(char* path);

#endif