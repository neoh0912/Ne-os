#ifndef UTILS_H
#define UTILS_H

void print_args(int argc, char **argv);
int count_initial_flags(int argc, char **argv);

#define max(x,y) ((x) > (y) ? (x) : (y))
#define min(x,y) ((x) < (y) ? (x) : (y))

#endif
