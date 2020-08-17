#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    REG,
    ADDR,
    IMM,
    REG_LIST,
    PC,
    SP,
} operand_type_t;

typedef enum {
    NONE,
    MINUS,
    SIGNED,
} imm_opt_t;

typedef struct {
    uint8_t bits;
    uint8_t align;
    imm_opt_t opt;
} operand_imm_t;

typedef struct {
    operand_type_t type;
    union {
	struct {
	    operand_type_t base;
	    operand_type_t offset;
	    operand_imm_t offset_imm;
	} addr;
	struct {
	    bool LR;
	    bool PC;
	} reg_list;
	operand_imm_t imm;
    };
} operand_t;

typedef struct {
    uint8_t length;
    operand_t *operands;
} operands_case_t;

typedef struct {
    char *opcode;
    uint8_t length;
    operands_case_t *cases;
} instruction_t;

operands_case_t _str_ops[] = {
    { length: 1, operands: (operand_t []){ { type: REG } } }
};

const instruction_t instructions[] = {
    { opcode: "STR", length: 1, cases: (operands_case_t[]) {
	{ length: 1, operands: (operand_t []){
		{ type: REG },
		{ type: ADDR, .addr = { base: REG, offset: REG } }
	} },
	{ length: 1, operands: (operand_t []){
		{ type: REG },
		{ type: ADDR, .addr = { base: REG, offset: IMM, offset_imm: { bits: 7, align: 4, opt: NONE } } }
	} }
    }},
    { "LDR", 0, cases: NULL },
};

const size_t instructions_length = 2;
