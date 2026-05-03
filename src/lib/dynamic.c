#include <dynamic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DYNAMIC *dynamic(size_t element_size, size_t initial_capacity) {
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
  p_dynamic->element_size = element_size;

  return p_dynamic;
}

size_t dynamic_size(DYNAMIC *p_dynamic) { return p_dynamic->length; }
size_t dynamic_capacity(DYNAMIC *p_dynamic) { return p_dynamic->capacity; }

void dynamic_grow(DYNAMIC **i_dynamic) {
  DYNAMIC *p_dynamic = *i_dynamic;
  if (p_dynamic->length == p_dynamic->capacity) {
    p_dynamic->size *= 2;
    p_dynamic->capacity *= 2;
    DYNAMIC * n_dynamic = realloc(p_dynamic, sizeof(DYNAMIC) + p_dynamic->size);
    *i_dynamic = n_dynamic;
    p_dynamic = n_dynamic;
  }
  p_dynamic->length++;
  return;
}

void dynamic_insert(DYNAMIC **i_dynamic, void *element) {
  DYNAMIC *p_dynamic = *i_dynamic;
  size_t offset = p_dynamic->length * p_dynamic->element_size;
  size_t size = p_dynamic->element_size;
  dynamic_grow(i_dynamic);
  p_dynamic = *i_dynamic;
  char *dest = (char *)&p_dynamic[1];
  memcpy(&dest[offset], element, size);
  return;
}

void *dynamic_get(DYNAMIC *p_dynamic, size_t idx) {
  size_t offset = idx * p_dynamic->element_size;
  return &((char *)&p_dynamic[1])[offset];
}

void dynamic_shrink_to_fit(DYNAMIC **i_dynamic) {
  DYNAMIC *p_dynamic = *i_dynamic;
  if (p_dynamic->capacity == p_dynamic->length)
    return;

  p_dynamic->capacity = p_dynamic->length;
  p_dynamic->size = p_dynamic->element_size * p_dynamic->length;
  p_dynamic = realloc(p_dynamic, p_dynamic->size);
  *i_dynamic = p_dynamic;
  return;
}

void dynamic_free(DYNAMIC *p_dynamic) {
  free(p_dynamic);
  return;
}

void print_dynamic(const char *format, DYNAMIC *p_dynamic, bool direct) {
  size_t size = p_dynamic->element_size;
  for (int i = 0; i < p_dynamic->length; i++) {
    size_t offset = i * size;
    char *src = (char *)&p_dynamic[1];
    if (direct)
      printf(format, src[offset]);
    else
      printf(format, &src[offset]);
    printf("\n");
  }
  return;
}

void dynamic_debug(DYNAMIC *p_dynamic) {
  printf("DYNAMIC: length=%ld, capacity=%ld, size=%ld, element_size=%ld\n",
         p_dynamic->length, p_dynamic->capacity, p_dynamic->size,
         p_dynamic->element_size);
  return;
}
