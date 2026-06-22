#include "dynamic_buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

DynamicBuffer* create_dynamic_buffer(int init_capacity) {
    DynamicBuffer* dynamic_buffer = (DynamicBuffer*) malloc(sizeof(DynamicBuffer));
    dynamic_buffer->capacity = init_capacity;
    dynamic_buffer->length = 0;
    dynamic_buffer->buffer = (char*) malloc(sizeof(char) * dynamic_buffer->capacity);

    return dynamic_buffer;
}

void resize_dynamic_buffer(DynamicBuffer* dynamic_buffer, int new_capacity) {
    dynamic_buffer->capacity = new_capacity;
    dynamic_buffer->buffer = realloc(dynamic_buffer->buffer, sizeof(char) * dynamic_buffer->capacity);
}

void grow_dynamic_buffer(DynamicBuffer* dynamic_buffer) {
    resize_dynamic_buffer(dynamic_buffer, dynamic_buffer->capacity * 2);
}

void append_dynamic_buffer(DynamicBuffer* dynamic_buffer, char* append_buffer, int length) {
    if (dynamic_buffer->length + length + 1 > dynamic_buffer->capacity) {
        grow_dynamic_buffer(dynamic_buffer);
    }

    memcpy(&dynamic_buffer->buffer[dynamic_buffer->length], append_buffer, length);
    dynamic_buffer->length += length;
    dynamic_buffer->buffer[dynamic_buffer->length] = '\0';
}

void free_dynamic_buffer(DynamicBuffer* dynamic_buffer) {
    free(dynamic_buffer->buffer);
    free(dynamic_buffer);
}
