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
  bool color;
  bool list;
  bool recursive;
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

long int ls(char *path, DYNAMIC *dirs, FLAGS flags);

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
  case NAME:
    qsort_r(map, count, sizeof(size_t), compare_names, &args);
    break;
  case NONE:
    if (flags.reverse)
      qsort(map, count, sizeof(size_t), reverse_order);
    break;
  case WIDTH:
    qsort_r(map, count, sizeof(size_t), compare_lenghts, &args);
  }
  return;
}

long int read_dir(char *path, DYNAMIC **i_names, size_t *max_size,
                  DYNAMIC **i_types, DYNAMIC **i_ino, DYNAMIC **i_dirs,
                  FLAGS flags) {

  DIR *dir;
  struct dirent *entry;

  dir = opendir(path);
  if (dir == NULL) {
    return -1;
  }

  *max_size = 0;

  DYNAMIC *names = dynamic(sizeof(char) * 256, 0);
  DYNAMIC *types = dynamic(sizeof(char), 0);
  DYNAMIC *ino = dynamic(sizeof(ino_t), 0);
  DYNAMIC *dirs = dynamic(sizeof(char **), 0);
  size_t i = 0;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] != '.' || flags.show_hidden == true) {
      size_t size = min(strlen(entry->d_name), 255);
      *max_size = max(*max_size, size);
      dynamic_insert(&names, &entry->d_name[0]);
      dynamic_insert(&types, (void *)&entry->d_type);
      dynamic_insert(&ino, (void *)&entry->d_ino);
      if (entry->d_type == DT_DIR && entry->d_name[0] != '.' ) {
        void *p = dynamic_get(names, i);
        dynamic_insert(&dirs, &p);
      }
      i++;
    }
  }
  closedir(dir);

  //  dynamic_shrink_to_fit(&dyn_dir);
  //  dynamic_shrink_to_fit(&types);

  *i_names = names;
  *i_types = types;
  *i_ino = ino;
  *i_dirs = dirs;

  return 0;
}

long int print_dir(char *s_dir, FLAGS flags) {
  size_t max_size;
  DYNAMIC *names;
  DYNAMIC *types;
  DYNAMIC *ino;
  DYNAMIC *dirs;
  long int err = read_dir(s_dir, &names, &max_size, &types, &ino, &dirs, flags);
  if (err > 0)
    return 10101;
  if (err == -1)
    return 0;

  if (names == NULL)
    return 69;

  if (types == NULL)
    return 420;

  size_t count = dynamic_size(names);

  if (count == 0) {
    dynamic_free(types);
    dynamic_free(ino);
    dynamic_free(names);
    dynamic_free(dirs);
    return 0;
  }

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

  printf("\x1b[0m");

  if (directory_size != colums)
    printf("\n");

  free(sizes);
  dynamic_free(types);
  dynamic_free(ino);

  if (flags.recursive == true && dynamic_size(dirs) > 0) {
    if ((err = ls(s_dir, dirs, flags)) != 0)
      return err * 10;
  }

  dynamic_free(names);
  dynamic_free(dirs);

  return 0;
}

int compare_prefix(char *str, char *prefix) {
  return (strncmp(str, prefix, strlen(prefix))) == 0;
}

long int parse_long_argument(char *str, FLAGS *flags) {
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

long int ls_list(char *path, DYNAMIC *dirs, FLAGS flags) { return 69; }

long int ls(char *path, DYNAMIC *dirs, FLAGS flags) {

  long int err = 0;

  if (flags.list == true) {
    return ls_list(path, dirs, flags);
  }

  if (dynamic_size(dirs) == 0) {
    char *i = ".";
    dynamic_insert(&dirs, &i);
  }

  size_t count = dynamic_size(dirs);

  printf("count = %ld\n", count);

  if (count == 1) {
    char *full_path = concat_path(path, *(char **)dynamic_get(dirs, 0), NULL);
    if ((err = print_dir(full_path, flags)) != 0)
      return err;
    free(full_path);
  } else {
    char *full_path = NULL;
    for (size_t i = 0; i < count; i++) {
      full_path = concat_path(path, *(char **)dynamic_get(dirs, i), full_path);
      printf("%s:\n", full_path);
      if ((err = print_dir(full_path, flags)) != 0)
        return err;
      printf("\n");
    }
    free(full_path);
  }
  return 0;
}

long int main(int argc, char **argv) {
  long int err;

  FLAGS flags = {};
  flags.show_hidden = false;
  flags.sort = NAME;
  flags.reverse = false;

  flags.list = false;
  flags.recursive = false;

  flags.color = true;

  DYNAMIC *paths = dynamic(sizeof(void *), 0);

  for (long int i = 1; i < argc; i++) {
    char *str = argv[i];
    switch (str[0]) {
    case '-':
      switch (str[1]) {
      case '-':

        if ((err = parse_long_argument(str, &flags)) != 0)
          return 7;
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
          case 'R':
            flags.recursive = true;
            break;
          }
        }
      }
      break;
    default:
      dynamic_insert(&paths, &str);
    }
  }

  printf("\x1b[?25l");

  err = ls("", paths, flags);

  if (err != 0)
    printf("ERROR: %d\n", err);

  printf("\x1b[?25h");

  dynamic_free(paths);
  return err;
}
