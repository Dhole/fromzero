#ifndef ERRORS_H
#define ERRORS_H

typedef enum error_t {
    OK = 0,
    ERR_NO_MEM = 1,
    ERR_STRING_REF = 2,
    ERR_OUT_OF_BOUNDS = 3,
} error_t;

extern const char *error_string[];

#endif // ERRORS_H