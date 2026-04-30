#include "../lib/io.h"
#include <string.h>

int main() {
    char* _str = "Hello world!\n";
    struct STRING str = cstring_to_string(_str);
    print(str);
    return 0;
}
