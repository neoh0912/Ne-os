
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
//#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#include <utils.h>
#include <string.h>


int print_dir(char *s_dir) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(s_dir);
    if (dir == NULL) {
        printf("ERROR\n");
        return 1;
    }

    int max_size = 0;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        max_size = max(max_size, min(strlen(entry->d_name),255) );
        count++;
    }


    int colums;
    {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        colums = w.ws_col/(max_size+2);
    }

    int rows = count/colums;

    if (count%colums != 0)
        rows++;

    int *sizes = calloc(colums+1,sizeof(int));

    int i=0;
    rewinddir(dir);
    while ((entry = readdir(dir)) != NULL) {
        sizes[i/rows+1] = max(sizes[i/rows+1],min(strlen(entry->d_name),255)+2);
        i++;
    }

    for (int i=1 ; i<colums ; i++) {
        sizes[i] += sizes[i-1];
    }



    i=0;
    rewinddir(dir);

    while ((entry = readdir(dir)) != NULL) {
        if (sizes[i/rows] != 0)
            printf("\x1b[%dC",sizes[i/rows]);
        printf("%s\n",entry->d_name);
        if (i%rows == rows-1) {
            printf("\x1b[s\x1b[%dA",rows);
        }
        i++;
    }
    printf("\x1b[u");


    free(sizes);

    closedir(dir);
    return 0;
}

int print_dirs(char **dirv, int dirc) {
    int err;

    for (int i=0 ; i<dirc ; i++) {
        printf("%s:\n",dirv[i]);
        if ((err = print_dir(dirv[i])) != 0)
            return err;
        printf("\n");
    }

    return 0;
}

int main(int argc, char **argv) {
    int err;

    if (argc > 1) {
        int flags = count_initial_flags(argc,argv);
        if (flags == 0) {
            if (argc == 2) {
                err = print_dir(argv[1]);
            } else {
                err = print_dirs(&argv[1],argc-1);
            }
        }
    } else {
        err = print_dir(".");
    }

    return err;
}

