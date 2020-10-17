#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "instructions.h"
#include "vector.h"
#include "string.h"

const register_alias_t _registers[] = {
    {name: "a0", value: 10},
    {name: "a1", value: 11},
    {name: "a2", value: 12},
    {name: "a3", value: 13},
    {name: "a4", value: 14},
    {name: "a5", value: 15},
    {name: "a6", value: 16},
    {name: "a7", value: 17},
    {name: "fp", value: 8},
    {name: "gp", value: 3},
    {name: "ra", value: 1},
    {name: "s0", value: 8},
    {name: "s1", value: 9},
    {name: "s10", value: 26},
    {name: "s11", value: 27},
    {name: "s2", value: 18},
    {name: "s3", value: 19},
    {name: "s4", value: 20},
    {name: "s5", value: 21},
    {name: "s6", value: 22},
    {name: "s7", value: 23},
    {name: "s8", value: 24},
    {name: "s9", value: 25},
    {name: "sp", value: 2},
    {name: "t0", value: 0},
    {name: "t1", value: 1},
    {name: "t2", value: 2},
    {name: "t3", value: 28},
    {name: "t4", value: 29},
    {name: "t5", value: 30},
    {name: "t6", value: 31},
    {name: "tp", value: 4},
    {name: "x0", value: 0},
    {name: "x1", value: 1},
    {name: "x10", value: 10},
    {name: "x11", value: 11},
    {name: "x12", value: 12},
    {name: "x13", value: 13},
    {name: "x14", value: 14},
    {name: "x15", value: 15},
    {name: "x16", value: 16},
    {name: "x17", value: 17},
    {name: "x18", value: 18},
    {name: "x19", value: 19},
    {name: "x2", value: 2},
    {name: "x20", value: 20},
    {name: "x21", value: 21},
    {name: "x22", value: 22},
    {name: "x23", value: 23},
    {name: "x24", value: 24},
    {name: "x25", value: 25},
    {name: "x26", value: 26},
    {name: "x27", value: 27},
    {name: "x28", value: 28},
    {name: "x29", value: 29},
    {name: "x3", value: 3},
    {name: "x30", value: 30},
    {name: "x31", value: 31},
    {name: "x4", value: 4},
    {name: "x5", value: 5},
    {name: "x6", value: 6},
    {name: "x7", value: 7},
    {name: "x8", value: 8},
    {name: "x9", value: 9},
    {name: "zero", value: 0},
};

const vector_t registers = {
	data: (void *) _registers,
	length: 65,
	capacity: 0,
	elem_size: sizeof(register_alias_t),
	elem_free_fn: NULL,
};

cmp_t
string_register_alias_cmp(string_t *key, register_alias_t *reg_alias)
{
    return string_cmp_c(key, reg_alias->name);
}

const int instructions_len = 39;
const instruction_t instructions[] = {
    {name: "add",  fmt: R_TYPE, opcode: 0b0110011, funct3: 0x0, funct7: 0x00, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {name: "sub",  fmt: R_TYPE, opcode: 0b0110011, funct3: 0x0, funct7: 0x20, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {name: "xor",  fmt: R_TYPE, opcode: 0b0110011, funct3: 0x4, funct7: 0x00, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {name: "or",   fmt: R_TYPE, opcode: 0b0110011, funct3: 0x6, funct7: 0x00, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {name: "and",  fmt: R_TYPE, opcode: 0b0110011, funct3: 0x7, funct7: 0x00, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {name: "sll",  fmt: R_TYPE, opcode: 0b0110011, funct3: 0x1, funct7: 0x00, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {name: "srl",  fmt: R_TYPE, opcode: 0b0110011, funct3: 0x5, funct7: 0x00, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {name: "sra",  fmt: R_TYPE, opcode: 0b0110011, funct3: 0x5, funct7: 0x20, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {name: "slt",  fmt: R_TYPE, opcode: 0b0110011, funct3: 0x2, funct7: 0x00, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},
    {name: "sltu", fmt: R_TYPE, opcode: 0b0110011, funct3: 0x3, funct7: 0x00, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}
    }},

    {name: "addi",  fmt: I_TYPE, opcode: 0b0010011, funct3: 0x0, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "xori",  fmt: I_TYPE, opcode: 0b0010011, funct3: 0x4, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "ori",   fmt: I_TYPE, opcode: 0b0010011, funct3: 0x6, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "andi",  fmt: I_TYPE, opcode: 0b0010011, funct3: 0x7, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "slli",  fmt: I_TYPE, opcode: 0b0010011, funct3: 0x1, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: IMM_5_11_00}
    }},
    {name: "srli",  fmt: I_TYPE, opcode: 0b0010011, funct3: 0x5, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: IMM_5_11_00}
    }},
    {name: "srai",  fmt: I_TYPE, opcode: 0b0010011, funct3: 0x5, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: IMM_5_11_20}
    }},
    {name: "slti",  fmt: I_TYPE, opcode: 0b0010011, funct3: 0x2, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "sltiu", fmt: I_TYPE, opcode: 0b0010011, funct3: 0x3, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},

    {name: "lb",    fmt: I_TYPE, opcode: 0b0000011, funct3: 0x0, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "lh",    fmt: I_TYPE, opcode: 0b0000011, funct3: 0x1, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "lw",    fmt: I_TYPE, opcode: 0b0000011, funct3: 0x2, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "lbu",   fmt: I_TYPE, opcode: 0b0000011, funct3: 0x4, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "lhu",   fmt: I_TYPE, opcode: 0b0000011, funct3: 0x5, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},

    {name: "sb",    fmt: S_TYPE, opcode: 0b0100011, funct3: 0x0, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS2}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "sh",    fmt: S_TYPE, opcode: 0b0100011, funct3: 0x1, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS2}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},
    {name: "sw",    fmt: S_TYPE, opcode: 0b0100011, funct3: 0x2, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS2}, {type: REG, reg_opt: RS1}, {type: IMM, imm_opt: SIG}
    }},

    {name: "beq",   fmt: B_TYPE, opcode: 0b1100011, funct3: 0x0, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}, {type: IMM, imm_opt: SIG}
    }},
    {name: "bne",   fmt: B_TYPE, opcode: 0b1100011, funct3: 0x1, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}, {type: IMM, imm_opt: SIG}
    }},
    {name: "blt",   fmt: B_TYPE, opcode: 0b1100011, funct3: 0x4, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}, {type: IMM, imm_opt: SIG}
    }},
    {name: "bge",   fmt: B_TYPE, opcode: 0b1100011, funct3: 0x5, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}, {type: IMM, imm_opt: SIG}
    }},
    {name: "bltu",  fmt: B_TYPE, opcode: 0b1100011, funct3: 0x6, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}, {type: IMM, imm_opt: SIG}
    }},
    {name: "bgeu",  fmt: B_TYPE, opcode: 0b1100011, funct3: 0x7, ops_len: 3, ops: (operand_t[]) {
	{type: REG, reg_opt: RS1}, {type: REG, reg_opt: RS2}, {type: IMM, imm_opt: SIG}
    }},

    {name: "jal",   fmt: J_TYPE, opcode: 0b1101111, ops_len: 2, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: IMM, imm_opt: SIG}
    }},

    {name: "jalr",  fmt: I_TYPE, opcode: 0b1100111, funct3: 0x0, ops_len: 2, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: IMM, imm_opt: SIG}
    }},

    {name: "lui",   fmt: U_TYPE, opcode: 0b0110111, ops_len: 2, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: IMM, imm_opt: SIG}
    }},
    {name: "auipc", fmt: U_TYPE, opcode: 0b0010111, ops_len: 2, ops: (operand_t[]) {
	{type: REG, reg_opt: RD}, {type: IMM, imm_opt: SIG}
    }},

    {name: "ecall", fmt: I_TYPE, opcode: 0b1110011, ops_len: -1, ops: (operand_t[]) {
	{type: IMM, imm_opt: IMM_0}
    }},
    {name: "ebreak",fmt: I_TYPE, opcode: 0b1110011, ops_len: -1, ops: (operand_t[]) {
	{type: IMM, imm_opt: IMM_1}
    }},
};
