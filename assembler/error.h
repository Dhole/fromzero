#ifndef ERRORS_H
#define ERRORS_H

typedef enum error_t {
    OK = 0,
    ERR_NO_MEM = 1,
    ERR_STRING_REF = 2,
    ERR_OUT_OF_BOUNDS = 3,
    ERR_NUM_DEC = 4,
    ERR_NUM_LONG = 5,
    ERR_EMPTY = 6,
    ERR_WRITE = 7,
    ERR_UNK = 8,
    ERR_EOF = 9,
    ERR_EVAL = 10,
    ERR_EVAL_HEADNOSYM = 11,
    ERR_EVAL_ARGSLEN = 12,
    ERR_EVAL_BADTYPE = 13,
} error_t;

extern const char *error_string[];

#endif // ERRORS_H
