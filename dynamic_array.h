#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

typedef struct {
    void* array;
    size_t capacity;
    size_t length;
    size_t element_size;
} DynamicArray;

DynamicArray* dynamic_array_create(int init_capacity, int element_size);
void dynamic_array_resize(DynamicArray* dynamic_array, int new_capacity);
void dynamic_array_grow(DynamicArray* dynamic_array);
void dynamic_array_append(DynamicArray* dynamic_array, void* element);
void dynamic_array_free(DynamicArray* dynamic_array);

#endif