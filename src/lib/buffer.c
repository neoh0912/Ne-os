#include <buffer.h>
#include <stddef.h>
#include <stdlib.h>

BUFFER *buffer(size_t size, int (*call)(const char * restrict, ...) ) {
  if (size <= 0)
    return NULL;
  if (call == NULL)
    return NULL;

  BUFFER *p_buffer = (BUFFER *)calloc(1, sizeof(BUFFER) + size + 1);
  p_buffer->used = 0;
  p_buffer->size = size;
  p_buffer->call = call;

  return p_buffer;
}

int buffer_write(BUFFER *p_buffer, char *str) {
  size_t i = p_buffer->used;
  char *buf = (char *)&p_buffer[1];
  char c;
  for (size_t j = 0; (c = str[j]) != 0; j++) {
    if (i == p_buffer->size) {
      if (p_buffer->call(buf) != 0)
        return 1;
      p_buffer->used = 0;
    }
    buf[i++] = c;
  }

  return 0;
}

int buffer_flush(BUFFER *p_buffer) {
  char *buf = (char *)&p_buffer[1];
  if (p_buffer->call(buf) != 0)
    return 1;
  p_buffer->used = 0;
  return 0;
}
