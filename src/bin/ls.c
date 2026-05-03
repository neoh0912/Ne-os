#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
// #include <sys/stat.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <string.h>
#include <utils.h>

#include <dynamic.h>

int print_dir(char *s_dir) {
  DIR *dir;
  struct dirent *entry;

  dir = opendir(s_dir);
  if (dir == NULL) {
    printf("ERROR\n");
    return 1;
  }

  int max_size = 0;

  char *dyn_dirs = dynamic(sizeof(char) * 256, 0);

  while ((entry = readdir(dir)) != NULL) {
    size_t size = min(strlen(entry->d_name), 255);
    max_size = max(max_size, size);
    dynamic_grow(dyn_dirs);
    strncpy(dyn_dirs, entry->d_name, 255);
  }

  dynamic_shrink_to_fit(dyn_dirs);

  print_dynamic("%s",dyn_dirs);

  closedir(dir);
  return 0;
}

int print_dirs(char **dirv, int dirc) {
  int err;

  for (int i = 0; i < dirc; i++) {
    printf("%s:\n", dirv[i]);
    if ((err = print_dir(dirv[i])) != 0)
      return err;
    printf("\n");
  }

  return 0;
}

int main(int argc, char **argv) {
  int err;

  if (argc > 1) {
    int flags = count_initial_flags(argc, argv);
    if (flags == 0) {
      if (argc == 2) {
        err = print_dir(argv[1]);
      } else {
        err = print_dirs(&argv[1], argc - 1);
      }
    }
  } else {
    err = print_dir(".");
  }

  return err;
}
