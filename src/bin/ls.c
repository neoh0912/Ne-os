#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    DIR *dir;
    struct dirent *entry;

    if (argc > 1) {
        dir = opendir(argv[1]);
    } else {
        dir = opendir(".");
    }

    if (dir == NULL) {
        printf("ERROR");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_type,entry->d_name);
    }

    closedir(dir);
    return 0;
}
