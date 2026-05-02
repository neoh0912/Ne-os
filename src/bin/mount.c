#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>

#include <utils.h>

int main(int argc, char **argv) {

    int count = count_initial_flags(argc,argv);

    if (count == 0) {
        return 1;
    } else if (count == 1) {
        if (argv[1][1] == 't' && argv[1][2] == 0) {
            if (argc < 4)
                return 1;
            int err = mount(argv[3],argv[4],argv[2],0,NULL);
            return err;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}
