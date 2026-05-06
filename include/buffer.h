#ifndef BUFFER_H
#define BUFFER_H 1

#include <stddef.h>

typedef struct {
  size_t used;
  size_t size;
  int (*call)(const char * restrict, ...);
} BUFFER;

BUFFER *buffer(size_t size, int (*call)(const char * restrict, ...));

int buffer_write(BUFFER *p_buffer, char *str);
int buffer_flush(BUFFER *p_buffer);

void buffer_resize(BUFFER *p_buffer, size_t size);

void buffer_free(BUFFER *p_buffer);

#endif
