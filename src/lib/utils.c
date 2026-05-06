#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

void print_args(int argc, char **argv) {
  printf("argv[%d] = {", argc);
  printf("\"%s\"", argv[0]);
  for (int i = 1; i < argc; i++)
    printf(",\"%s\"", argv[i]);
  printf("}\n");
  return;
}

int count_initial_flags(int argc, char **argv) {
  if (argc <= 1)
    return 0;
  int count = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-')
      return count;
    count++;
  }
  return count;
}

char *concat_path(const char *s1, const char *s2, char *buf) {
  char *result;
  size_t len = strlen(s1);
  if (buf == NULL)
    result = malloc(len + strlen(s2) + 2);
  else
    result = realloc(buf, len + strlen(s2) + 2);
  strcpy(result, s1);
  if ((s1[len - 1] != '/') && (s2[0] != '/') && (len != 0)) {
    strcat(result, "/");
  }
  strcat(result, s2);
  return result;
}
