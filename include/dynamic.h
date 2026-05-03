#ifndef DYNAMIC_H
#define DYNAMIC_H 1

#define DYNAMIC_DEFAULT_CAPACITY 32

#include <stddef.h>

typedef struct {
  size_t length;
  size_t capacity;
  size_t size;
} DYNAMIC;

void *dynamic(size_t element_size, size_t initial_capacity);
void dynamic_grow(void *i_dynamic);
void dynamic_shrink_to_fit(void *i_dynamic);
void dynamic_free(void *i_dynamic);
void print_dynamic(const char *format, void *i_dynamic);

#endif
