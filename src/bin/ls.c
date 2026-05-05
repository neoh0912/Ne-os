#define _GNU_SOURCE
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

typedef enum {
  NONE,
  //  size;
  //  time;
  //  version;
  //  extension
  NAME,
  WIDTH

} SORT;

typedef struct {
  bool show_hidden;
  bool reverse;
  SORT sort;
} FLAGS;

size_t get_stdout_width() {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  return w.ws_col;
}

typedef struct {
  DYNAMIC *name;
  int polarity;
} compare_args;

int reverse_order(const void *p_a, const void *p_b) {
  return ((*(size_t *)p_a <= *(size_t *)p_b) ? (*(size_t *)p_a < *(size_t *)p_b)
                                             : -1);
}

int compare_names(const void *p_a, const void *p_b, void *context) {
  compare_args args = *(compare_args *)context;

  DYNAMIC *name = args.name;
  size_t a = *(size_t *)p_a;
  size_t b = *(size_t *)p_b;

  char *name_a = dynamic_get(name, a);
  char *name_b = dynamic_get(name, b);

  return strcmp(name_a, name_b) * args.polarity;
}

int compare_lenghts(const void *p_a, const void *p_b, void *context) {
  compare_args args = *(compare_args *)context;

  DYNAMIC *name = args.name;
  size_t a = *(size_t *)p_a;
  size_t b = *(size_t *)p_b;

  char *name_a = dynamic_get(name, a);
  char *name_b = dynamic_get(name, b);

  size_t len_a = strlen(name_a);
  size_t len_b = strlen(name_b);

  if (len_a == len_b) {
    return strcmp(name_a, name_b) * args.polarity;
  }

  return ((len_a >= len_b) ? len_a > len_b : -1) * args.polarity;
}

void sort(FLAGS flags, DYNAMIC **i_name, size_t **i_map, size_t count) {
  size_t *map = *i_map;
  compare_args args;
  args.name = *i_name;
  args.polarity = (flags.reverse) ? -1 : 1;
  switch (flags.sort) {
  case NONE:
    if (flags.reverse)
      qsort(map, count, sizeof(size_t), reverse_order);
    break;
  case NAME:
    qsort_r(map, count, sizeof(size_t), compare_names, &args);
    break;
  case WIDTH:
    qsort_r(map, count, sizeof(size_t), compare_lenghts, &args);
  }
  return;
}

int read_dir(char *s_dir, DYNAMIC **i_dyn_dir, size_t *max_size,
             DYNAMIC **i_types, DYNAMIC **i_ino, FLAGS flags) {

  DIR *dir;
  struct dirent *entry;

  dir = opendir(s_dir);
  if (dir == NULL) {
    return 1;
  }

  *max_size = 0;

  DYNAMIC *dyn_dir = dynamic(sizeof(char) * 256, 0);
  DYNAMIC *types = dynamic(sizeof(char), 0);
  DYNAMIC *ino = dynamic(sizeof(ino_t), 0);
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] != '.' || flags.show_hidden == true) {
      size_t size = min(strlen(entry->d_name), 255);
      *max_size = max(*max_size, size);
      dynamic_insert(&dyn_dir, &entry->d_name[0]);
      dynamic_insert(&types, (void *)&entry->d_type);
      dynamic_insert(&ino, (void *)&entry->d_ino);
    }
  }
  closedir(dir);

  //  dynamic_shrink_to_fit(&dyn_dir);
  //  dynamic_shrink_to_fit(&types);

  *i_dyn_dir = dyn_dir;
  *i_types = types;
  *i_ino = ino;

  return 0;
}

int print_dir(char *s_dir, FLAGS flags) {
  size_t max_size;
  DYNAMIC *names;
  DYNAMIC *types;
  DYNAMIC *ino;
  int err = read_dir(s_dir, &names, &max_size, &types, &ino, flags);
  if (err != 0)
    return err;

  if (names == NULL)
    return 1;

  if (types == NULL)
    return 1;

  size_t count = dynamic_size(names);

  size_t width = get_stdout_width();
  size_t colums = width / (max_size + 2);
  size_t rows = (count + colums - 1) / colums;

  size_t size = colums * rows;

  {
    unsigned char n = DT_UNKNOWN;
    for (size_t i = count; i < size; i++) {
      dynamic_insert(&names, "");
      dynamic_insert(&types, &n);
    }
  }

  size_t directory_size = dynamic_size(names);

  size_t *map = calloc(directory_size, sizeof(size_t));

  for (size_t i = 0; i < directory_size; i++) {
    map[i] = i;
  }

  sort(flags, &names, &map, count);

  size_t *sizes = calloc(colums, sizeof(size_t));

  for (size_t i = 0; i < directory_size; i++) {
    size_t idx = i / rows;
    sizes[idx] = max(sizes[idx], strlen(dynamic_get(names, map[i])));
  }

  for (size_t i = 0; i < directory_size; i++) {
    size_t idx = rows * (i % colums) + i / colums;
    switch (*(unsigned char *)dynamic_get(types, map[idx])) {
    case DT_DIR:
      printf("\x1b[0;34m");
      break;
    case DT_LNK:
      printf("\x1b[0;96m");
      break;
    default:
      printf("\x1b[0m");
    }
    printf("%-*s", (int)sizes[i % colums] + 2,
           (char *)dynamic_get(names, map[idx]));
    if (i % colums == colums - 1)
      printf("\n");
  }

  printf("\x1b[0m\n");

  free(sizes);
  dynamic_free(names);
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

int compare_prefix(char *str, char *prefix) {
  return (strncmp(str, prefix, strlen(prefix))) == 0;
}

int parse_long_argument(char *str, FLAGS *flags) {
  if (compare_prefix(str, "--sort=")) {
    char *value = &str[sizeof("--sort=") - 1];
    if (strcmp(value, "none") == 0) {
      flags->sort = NONE;
    } else if (strcmp(value, "name") == 0) {
      flags->sort = NAME;
    } else if (strcmp(value, "width") == 0) {
      flags->sort = WIDTH;
    }
  } else if (strcmp(str, "--reverse") == 0) {
    flags->reverse = true;
  } else {
    return 1;
  }
  return 0;
}

int main(int argc, char **argv) {
  int err;

  FLAGS flags = {};
  flags.show_hidden = false;
  flags.sort = NAME;
  flags.reverse = false;

  bool no_paths = true;
  bool multiple_paths = false;
  bool color = true;

  DYNAMIC *dirs = dynamic(sizeof(char **), 0);

  for (int i = 1; i < argc; i++) {
    char *str = argv[i];
    switch (str[0]) {
    case '-':
      switch (str[1]) {
      case '-':

        if ((err = parse_long_argument(str, &flags)) != 0)
          return 1;
        break;

      default:
        char c;
        while ((c = *(++str)) != 0) {
          switch (c) {
          case 'a':
          case 'A':
            flags.show_hidden = true;
            break;
          case 'U':
            flags.sort = NONE;
            break;
          case 'r':
            flags.reverse = true;
            break;
          }
        }
      }
      break;
    default:
      dynamic_insert(&dirs, &str);
      if (multiple_paths == false) {
        if (no_paths == false)
          multiple_paths = true;
        else
          no_paths = false;
      }
    }
  }

  printf("\x1b[?25l");

  if (no_paths == true) {
    print_dir(".", flags);
  } else if (multiple_paths == false) {
    print_dir(*(char **)dynamic_get(dirs, 0), flags);
  } else {
    print_dirs(dynamic_get(dirs, 0), dynamic_size(dirs), flags);
  }

  printf("\x1b[?25h");

  dynamic_free(dirs);
  return err;
}
