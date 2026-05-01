#include <unistd.h>
int main(int argc, char **argv) {
    if (argc > 1) {
        chdir(argv[1]);
    } else {
        chdir("/");
    }
    return 0;
}
