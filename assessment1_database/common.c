#include "common.h"

#include <stdlib.h>
#include <string.h>

static int int_array_reserve(IntArray *array, size_t new_capacity) {
    if (new_capacity <= array->capacity) {
        return 1;
    }

    int *new_data = realloc(array->data, new_capacity * sizeof(int));
    if (new_data == NULL) {
        return 0;
    }

    array->data = new_data;
    array->capacity = new_capacity;
    return 1;
}

void int_array_init(IntArray *array) {
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
}

void int_array_clear(IntArray *array) {
    array->size = 0;
}

void int_array_free(IntArray *array) {
    free(array->data);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
}

int int_array_push_back(IntArray *array, int value) {
    if (array->size == array->capacity) {
        size_t new_capacity = array->capacity == 0 ? 8 : array->capacity * 2;
        if (!int_array_reserve(array, new_capacity)) {
            return 0;
        }
    }

    array->data[array->size++] = value;
    return 1;
}

int int_array_insert_sorted_unique(IntArray *array, int value) {
    size_t left = 0;
    size_t right = array->size;

    while (left < right) {
        size_t middle = left + (right - left) / 2;
        if (array->data[middle] == value) {
            return 1;
        }
        if (array->data[middle] < value) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }

    if (array->size == array->capacity) {
        size_t new_capacity = array->capacity == 0 ? 8 : array->capacity * 2;
        if (!int_array_reserve(array, new_capacity)) {
            return 0;
        }
    }

    if (left < array->size) {
        memmove(&array->data[left + 1], &array->data[left], (array->size - left) * sizeof(int));
    }
    array->data[left] = value;
    array->size++;
    return 1;
}

void int_array_sort(IntArray *array) {
    if (array->size > 1) {
        qsort(array->data, array->size, sizeof(int), int_compare);
    }
}

int int_array_equals(const IntArray *array, const int *expected, size_t count) {
    if (array->size != count) {
        return 0;
    }

    for (size_t i = 0; i < count; i++) {
        if (array->data[i] != expected[i]) {
            return 0;
        }
    }
    return 1;
}

void int_array_print(FILE *out, const IntArray *array) {
    for (size_t i = 0; i < array->size; i++) {
        fprintf(out, "%d", array->data[i]);
        if (i + 1 < array->size) {
            fputs(", ", out);
        }
    }
}

int int_compare(const void *left, const void *right) {
    int a = *(const int *)left;
    int b = *(const int *)right;

    if (a < b) {
        return -1;
    }
    if (a > b) {
        return 1;
    }
    return 0;
}
