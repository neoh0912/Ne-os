#include <dynamic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void *dynamic(size_t element_size, size_t initial_capacity) {
  if (element_size < 1 || element_size == 0)
    return NULL;

  size_t count;
  if (initial_capacity == 0)
    count = DYNAMIC_DEFAULT_CAPACITY;
  else
    count = initial_capacity;

  DYNAMIC *p_dynamic = malloc(sizeof(DYNAMIC) + count * element_size);
  p_dynamic->length = 0;
  p_dynamic->capacity = count;
  p_dynamic->size = count * element_size;

  return (void *)&p_dynamic[1];
}

void dynamic_grow(void *i_dynamic) {
  DYNAMIC *p_dynamic = &((DYNAMIC *)i_dynamic)[-1];
  if (p_dynamic->length == p_dynamic->capacity) {
    p_dynamic->size *= 2;
    p_dynamic->capacity *= 2;
    p_dynamic = realloc(p_dynamic, sizeof(DYNAMIC) + p_dynamic->size);
  }
  p_dynamic->length++;
  return;
}

void dynamic_shrink_to_fit(void *i_dynamic) {
  DYNAMIC *p_dynamic = &((DYNAMIC *)i_dynamic)[-1];
  size_t size = p_dynamic->size / p_dynamic->capacity;
  p_dynamic->capacity = p_dynamic->length;
  p_dynamic->size = size * p_dynamic->length;
  p_dynamic = realloc(p_dynamic, p_dynamic->size);
  return;
}

void dynamic_free(void *i_dynamic) {
  free(&((DYNAMIC *)i_dynamic)[-1]);
  return;
}

void print_dynamic(const char *format, void *i_dynamic) {
  DYNAMIC *p_dynamic = &((DYNAMIC *)i_dynamic)[-1];

  size_t size = p_dynamic->size / p_dynamic->capacity;

  for (int i = 0; i < p_dynamic->length; i++) {
    int offset = i * size;
    printf(format, i_dynamic + offset);
    printf("\n");
  }

  return;
}
