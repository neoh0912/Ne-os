#include <stdio.h>

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

int max(int a, int b) {
  if (a > b)
    return a;
  return b;
}
int min(int a, int b) {
  if (a > b)
    return b;
  return a;
}
