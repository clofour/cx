#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

typedef struct {
    int capacity;
    int length;
    char* buffer;
} DynamicBuffer;

DynamicBuffer create_dynamic_buffer(int init_capacity);
void resize_dynamic_buffer(DynamicBuffer* dynamic_buffer, int new_capacity);
void grow_dynamic_buffer(DynamicBuffer* dynamic_buffer);
void append_dynamic_buffer(DynamicBuffer* dynamic_buffer, char* append_buffer, int length);
void free_dynamic_buffer(DynamicBuffer* dynamic_buffer);

#endif