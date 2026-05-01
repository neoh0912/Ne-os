#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main() {
    setsid();
    int fd = open("/dev/ttyS0", O_RDWR);
    dup2(fd,0);
    dup2(fd,1);
    dup2(fd,2);
    ioctl(0,TIOCSCTTY,1);
    execl("/bin/sh","/bin/sh",(char*)NULL);
    return 0;
}
