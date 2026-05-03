#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
// #include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <string.h>
#include <utils.h>

#include <dynamic.h>

typedef struct {
  bool show_hidden;
} FLAGS;

size_t get_stdout_width() {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  return w.ws_col;
}

int read_dir(char *s_dir, DYNAMIC **i_dyn_dir, size_t *max_size,
             DYNAMIC **i_types, FLAGS flags) {

  DIR *dir;
  struct dirent *entry;

  dir = opendir(s_dir);
  if (dir == NULL) {
    printf("ERROR\n");
    return 1;
  }

  *max_size = 0;

  DYNAMIC *dyn_dir = dynamic(sizeof(char) * 256, 0);
  DYNAMIC *types = dynamic(sizeof(char), 0);
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] != '.' || flags.show_hidden == true) {
      size_t size = min(strlen(entry->d_name), 255);
      *max_size = max(*max_size, size);
      dynamic_insert(&dyn_dir, &entry->d_name[0]);
      dynamic_insert(&types, (void *)&entry->d_type);
    }
  }
  closedir(dir);

//  dynamic_shrink_to_fit(&dyn_dir);
//  dynamic_shrink_to_fit(&types);

  *i_dyn_dir = dyn_dir;
  *i_types = types;

  return 0;
}

int print_dir(char *s_dir, FLAGS flags) {
  size_t max_size;
  DYNAMIC *name;
  DYNAMIC *types;
  int err = read_dir(s_dir, &name, &max_size, &types, flags);
  if (err != 0)
    return err;

  if (name == NULL)
    return 1;

  if (types == NULL)
    return 1;

  size_t width = get_stdout_width();
  size_t colums = width / (max_size + 2);
  size_t rows = (dynamic_size(name) + colums - 1) / colums;

  size_t *sizes = calloc(colums, sizeof(size_t));

  for (size_t i = 0; i < dynamic_size(name); i++) {
    size_t idx = i / rows;
    sizes[idx] = max(sizes[idx], strlen(dynamic_get(name, i)));
  }

  for (size_t i = 0; i < dynamic_size(name); i++) {
    size_t idx = rows * (i % colums) + i / colums;
    unsigned char type = *(unsigned char *)dynamic_get(types, idx);
    if (type == DT_REG)
      printf("\x1b[0m");
    else if (type == DT_DIR)
      printf("\x1b[0;34m");
    else {
    }
    printf("%-*s", (int)sizes[i % colums] + 2, (char *)dynamic_get(name, idx));
    if (i % colums == colums - 1)
      printf("\n");
  }

  printf("\x1b[0m\n");

  free(sizes);
  dynamic_free(name);
  return 0;
}

int print_dirs(char **dirv, int dirc, FLAGS flags) {
  int err;

  for (int i = 0; i < dirc; i++) {
    printf("%s:\n", dirv[i]);
    if ((err = print_dir(dirv[i], flags)) != 0)
      return err;
    printf("\n");
  }

  return 0;
}

int main(int argc, char **argv) {
  int err;

  FLAGS flags = {};
  flags.show_hidden = false;

  bool no_paths = true;
  bool multiple_paths = false;
  bool color = true;

  DYNAMIC *dirs = dynamic(sizeof(char **), 0);

  for (int i = 1; i < argc; i++) {
    char *str = argv[i];
    if (str[0] == '-') {
      if (str[1] == '-') {
      } else {
        char c;
        while ((c = *(++str)) != 0) {
          if (c == 'a' || c == 'a')
            flags.show_hidden = true;
          else {
          }
        }
      }
    } else {
      dynamic_insert(&dirs, &str);
      if (multiple_paths == false) {
        if (no_paths == false)
          multiple_paths = true;
        else
          no_paths = false;
      }
    }
  }

  if (no_paths == true) {
    print_dir(".", flags);
  } else if (multiple_paths == false) {
    print_dir(*(char **)dynamic_get(dirs, 0), flags);
  } else {
    print_dirs(dynamic_get(dirs, 0), dynamic_size(dirs), flags);
  }

  dynamic_free(dirs);
  return err;
}
