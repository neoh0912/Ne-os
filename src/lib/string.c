#include "string.h"

struct STRING cstring_to_string(char* _str) {
    struct STRING str;
    str.ptr = _str;
    int len = 0;
    {
        char* t = _str;
        while (*t != 0) {
            t++;
            len++;
        }
    }

    str.len = len;
    return str;
}
