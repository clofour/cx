#include "dynamic_array.h"
#include <stdlib.h>
#include <string.h>

DynamicArray* dynamic_array_create(int init_capacity, int element_size) {
    DynamicArray* dynamic_array = malloc(sizeof(DynamicArray));
    dynamic_array->capacity = init_capacity;
    dynamic_array->length = 0;
    dynamic_array->element_size = element_size;
    dynamic_array->array = malloc(dynamic_array->capacity * dynamic_array->element_size);

    return dynamic_array;
}

void dynamic_array_resize(DynamicArray* dynamic_array, int new_capacity) {
    dynamic_array->capacity = new_capacity;
    dynamic_array->array = realloc(dynamic_array->array, dynamic_array->capacity * dynamic_array->element_size);
}

void dynamic_array_grow(DynamicArray* dynamic_array) {
    dynamic_array_resize(dynamic_array, dynamic_array->capacity * 2);
}

void dynamic_array_append(DynamicArray* dynamic_array, void* element) {
    if (dynamic_array->length + 1 > dynamic_array->capacity) {
        dynamic_array_grow(dynamic_array);
    }

    memcpy(dynamic_array->array + (dynamic_array->length * dynamic_array->element_size), element, dynamic_array->element_size);
    dynamic_array->length += 1;
}

void dynamic_array_free(DynamicArray* dynamic_array) {
    free(dynamic_array->array);
    free(dynamic_array);
}
