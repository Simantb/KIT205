#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} IntArray;

void int_array_init(IntArray *array);
void int_array_clear(IntArray *array);
void int_array_free(IntArray *array);
int int_array_push_back(IntArray *array, int value);
int int_array_insert_sorted_unique(IntArray *array, int value);
void int_array_sort(IntArray *array);
int int_array_equals(const IntArray *array, const int *expected, size_t count);
void int_array_print(FILE *out, const IntArray *array);
int int_compare(const void *left, const void *right);

#endif
