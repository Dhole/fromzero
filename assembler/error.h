#ifndef ERRORS_H
#define ERRORS_H

#define TRY(expr) \
    err = expr; \
    if (err != OK) { \
        return err; \
    }

#define TRY_GOTO(expr, label) \
    err = expr; \
    if (err != OK) { \
        goto label; \
    }

#define TRY_BREAK(expr) \
    err = expr; \
    if (err != OK) { \
        break; \
    }

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
    ERR_IO = 14,
    ERR_MAX_STR = 15,
    ERR_INST_LEN = 16,
    ERR_SYM_NOT_DEF = 17,
    ERR_NEG = 18,
    ERR_IMM_OUT_RANGE = 19,
    ERR_INST_UNK = 20,
    ERR_BAD_ORG = 21,
} error_t;

extern const char *error_string[];

#endif // ERRORS_H
