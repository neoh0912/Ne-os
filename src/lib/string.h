#ifndef string_h
#define string_h

struct STRING {
    char* ptr;
    int len;
};

struct STRING cstring_to_string(char*);

#endif
