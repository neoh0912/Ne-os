#include "io.h"
#include <unistd.h>
#include <string.h>


void print(struct STRING str) {
    syscall(1,1,str.ptr,str.len);
    return;
}
