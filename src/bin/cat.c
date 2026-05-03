#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>

#define bufsize 4096

int main(int argc, char **argv) {
  if (argc < 2) {
    return 1;
  }

  int fd = open(argv[1], 0);
  if (fd < 0) {
    return 2;
  }

  char buf[bufsize];

  while (1) {
    int n_read = read(fd, &buf[0], bufsize);
    if (n_read < 0)
      return 3;
    if (n_read == 0)
      return 0;
    write(1, &buf[0], n_read);
  }
}
