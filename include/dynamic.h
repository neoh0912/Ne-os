#ifndef DYNAMIC_H
#define DYNAMIC_H 1

#define DYNAMIC_DEFAULT_CAPACITY 32

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  size_t length;
  size_t capacity;
  size_t size;
  size_t element_size;
} DYNAMIC;

DYNAMIC *dynamic(size_t element_size, size_t initial_capacity);

size_t dynamic_size(DYNAMIC *p_dynamic);
size_t dynamic_capacity(DYNAMIC *p_dynamic);

void dynamic_grow(DYNAMIC **p_dynamic);
void dynamic_insert(DYNAMIC **p_dynamic, void *element);

void *dynamic_get(DYNAMIC *p_dynamic, size_t idx);

void dynamic_shrink_to_fit(DYNAMIC **p_dynamic);

void dynamic_free(DYNAMIC *p_dynamic);

void print_dynamic(const char *format, DYNAMIC *p_dynamic, bool direct);
void dynamic_debug(DYNAMIC *p_dynamic);

#endif
