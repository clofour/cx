#include "dynamic_buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

DynamicBuffer* dynamic_buffer_create(int init_capacity) {
    DynamicBuffer* dynamic_buffer = (DynamicBuffer*) malloc(sizeof(DynamicBuffer));
    dynamic_buffer->capacity = init_capacity;
    dynamic_buffer->length = 0;
    dynamic_buffer->buffer = (char*) malloc(sizeof(char) * dynamic_buffer->capacity);

    return dynamic_buffer;
}

void dynamic_buffer_resize(DynamicBuffer* dynamic_buffer, int new_capacity) {
    dynamic_buffer->capacity = new_capacity;
    dynamic_buffer->buffer = realloc(dynamic_buffer->buffer, sizeof(char) * dynamic_buffer->capacity);
}

void dynamic_buffer_grow(DynamicBuffer* dynamic_buffer) {
    dynamic_buffer_resize(dynamic_buffer, dynamic_buffer->capacity * 2);
}

void dynamic_buffer_append(DynamicBuffer* dynamic_buffer, char* append_buffer, int length) {
    if (dynamic_buffer->length + length + 1 > dynamic_buffer->capacity) {
        dynamic_buffer_grow(dynamic_buffer);
    }

    memcpy(&dynamic_buffer->buffer[dynamic_buffer->length], append_buffer, length);
    dynamic_buffer->length += length;
    dynamic_buffer->buffer[dynamic_buffer->length] = '\0';
}

void dynamic_buffer_free(DynamicBuffer* dynamic_buffer) {
    free(dynamic_buffer->buffer);
    free(dynamic_buffer);
}
