#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vector.h"
#include "string.h"

typedef struct {
    char *name;
    int value;
} register_alias_t;

// extern const int _registers_len;
// extern const register_alias_t _registers[];
extern const vector_t registers;
cmp_t string_register_alias_cmp(string_t *key, register_alias_t *reg_alias);

typedef enum {
    REG,
    IMM,
} operand_type_t;

typedef enum {
    ABS,
    SIG,
    IMM_0,
    IMM_1,
    IMM_5_11_00,
    IMM_5_11_20,
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
    char *name;
    inst_fmt_t fmt;
    int8_t ops_len;
    operand_t *ops;
    uint8_t opcode;
    uint8_t funct3;
    uint8_t funct7;
} instruction_t;

extern const int instructions_len;
extern const instruction_t instructions[];

// typedef struct {
//     uint8_t rd;
//     uint8_t rs1;
//     uint8_t rs2;
//     uint32_t imm;
// } operand_t;

#endif // INSTRUCTIONS_H
