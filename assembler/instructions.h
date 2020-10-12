#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Parser

typedef enum {
    REG,
    IMM,
} operand_type_t;

typedef enum {
    ABS,
    SIG,
} imm_opt_t;

typedef enum {
    RD,
    RS1,
    RS2,
} reg_opt_t;

typedef struct {
    operand_type_t type;
    imm_opt_t imm_opt;
    reg_opt_t reg_opt;
} operand_t;

typedef enum {
    R_TYPE,
    I_TYPE,
    S_TYPE,
    B_TYPE,
    U_TYPE,
    J_TYPE,
} inst_fmt_t;

typedef struct {
    char *inst;
    inst_fmt_t fmt;
    uint8_t ops_len;
    operand_t *ops;
} instruction_t;

extern const int instructions_len;
extern const instruction_t instructions[];

// Parsed

typedef struct {
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint32_t imm;
} operands_t;

#endif // INSTRUCTIONS_H
