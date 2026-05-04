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
  SORT sort;
} FLAGS;

size_t get_stdout_width() {
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  return w.ws_col;
}

int compare_names(const void *p_a, const void *p_b, void *context) {
  DYNAMIC *name = *(DYNAMIC **)context;
  size_t a = *(size_t *)p_a;
  size_t b = *(size_t *)p_b;

//  printf("hi\n");

  char *name_a = dynamic_get(name, a);
  char *name_b = dynamic_get(name, b);

//  printf("%ld %ld\t%s %s\n", a, b, name_a, name_b);
  return strcmp(name_a,name_b);
}

void print_array(size_t*a,size_t size) {
  printf("{");
  if (size != 0)
    printf("%ld",a[0]);
  for (size_t i=1; i < size; i++) {
    printf(",%ld",a[i]);
    if (i%20 == 18)
      printf("\n");
  }
  printf("}\n");

}

void sort_by_name(DYNAMIC **i_name, size_t **i_map) {
  size_t *map = *i_map;

  print_array(map,dynamic_size(*i_name));

  qsort_r(map, dynamic_size(*i_name), sizeof(size_t), compare_names, i_name);

  print_array(map,dynamic_size(*i_name));

//  printf("\nend\n\n");

  return;
}

void sort_by_width(DYNAMIC **name, size_t **map) { return; }

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
  DYNAMIC *names;
  DYNAMIC *types;
  int err = read_dir(s_dir, &names, &max_size, &types, flags);
  if (err != 0)
    return err;

  if (names == NULL)
    return 1;

  if (types == NULL)
    return 1;

  size_t directory_size = dynamic_size(names);

  size_t width = get_stdout_width();
  size_t colums = width / (max_size + 2);
  size_t rows = (directory_size + colums - 1) / colums;

  size_t size = colums*rows;

  dynamic_resize(&names,size);
  dynamic_resize(&types,size);

  directory_size = dynamic_size(names);

  size_t *map = calloc(directory_size, sizeof(size_t));

  for (size_t i = 0; i < directory_size; i++) {
    map[i] = i;
  }

  switch (flags.sort) {
  case NONE:
    break;
  case NAME:
    sort_by_name(&names, &map);
    break;
  case WIDTH:
    sort_by_width(&names, &map);
    break;
  }



  size_t *sizes = calloc(colums, sizeof(size_t));

  for (size_t i = 0; i < directory_size; i++) {
    size_t idx = i / rows;
    sizes[idx] = max(sizes[idx], strlen(dynamic_get(names, map[i])));
  }

  printf("\n");

  printf("%ld %ld\n",colums,rows);

  for (size_t i = 0; i < directory_size; i++) {
    size_t idx = rows * (i % colums) + i / colums;
    printf("%ld %ld\n",idx,map[idx]);
    unsigned char type = *(unsigned char *)dynamic_get(types, map[idx]);
    if (type == DT_REG)
      printf("\x1b[0m");
    else if (type == DT_DIR)
      printf("\x1b[0;34m");
    else {
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

int main(int argc, char **argv) {
  int err;

  FLAGS flags = {};
  flags.show_hidden = false;
  flags.sort = NAME;

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
        break;
      default:
        char c;
        while ((c = *(++str)) != 0) {
          if (c == 'a' || c == 'a')
            flags.show_hidden = true;
          else {
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
