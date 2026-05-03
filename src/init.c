#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main() {
  setsid();

  system("mount -t proc proc /proc");
  system("mount -t sysfs sysfs /sys");
  system("mount -t devtmpfs devtmpfs /dev");

  int fd = open("/dev/ttyS0", O_RDWR);
  dup2(fd, 0);
  dup2(fd, 1);
  dup2(fd, 2);
  if (fd > 2)
    close(fd);

  ioctl(0, TIOCSCTTY, 1);
  {
    struct winsize ws = {.ws_row = 24 * 2, .ws_col = 80 * 2};
    ioctl(0, TIOCSWINSZ, &ws);
  }

  execl("/bin/sh", "/bin/sh", (char *)NULL);
}
