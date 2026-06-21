#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

typedef struct {
    int capacity;
    int length;
    char* buffer;
} DynamicBuffer;

#endif