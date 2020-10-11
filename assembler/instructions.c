#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "instructions.h"

const int instructions_len = 8;
const instruction_t instructions[] = {
    {inst: "add", fmt: R_TYPE, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {inst: "sub", fmt: R_TYPE, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {inst: "xor", fmt: R_TYPE, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    // ...
    {inst: "addi", fmt: I_TYPE, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    // ...
    {inst: "lb", fmt: I_TYPE, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    // ...
    {inst: "sb", fmt: S_TYPE, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS2}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    // ...
    {inst: "beq", fmt: B_TYPE, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}, {type: IMM, imm_opt: SIG}
    }},
    // ...
    {inst: "beq", fmt: B_TYPE, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}, {type: IMM, imm_opt: SIG}
    }},
};
