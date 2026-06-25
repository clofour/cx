#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

typedef struct {
    int capacity;
    int length;
    char* buffer;
} DynamicBuffer;

DynamicBuffer* dynamic_buffer_create(int init_capacity);
void dynamic_buffer_resize(DynamicBuffer* dynamic_buffer, int new_capacity);
void dynamic_buffer_grow(DynamicBuffer* dynamic_buffer);
void dynamic_buffer_append(DynamicBuffer* dynamic_buffer, char* append_buffer, int length);
void dynamic_buffer_free(DynamicBuffer* dynamic_buffer);

#endif