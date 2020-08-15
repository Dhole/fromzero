#ifndef ERRORS_H
#define ERRORS_H

typedef enum error_t {
    OK = 0,
    ERR_NO_MEM = 1,
    ERR_STRING_REF = 2,
    ERR_OUT_OF_BOUNDS = 3,
} error_t;

const char *error_string[] = {
    "ok", // 0
    "out of memory", // 1
    "string is reference", // 2
    "out of bounds", // 3
};

#endif // ERRORS_H
