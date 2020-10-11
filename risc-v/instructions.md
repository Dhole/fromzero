# RV32I

## R-type
add  rd, rs1, rs2
sub  rd, rs1, rs2
xor  rd, rs1, rs2
or   rd, rs1, rs2
and  rd, rs1, rs2
sll  rd, rs1, rs2
srl  rd, rs1, rs2
sra  rd, rs1, rs2
slt  rd, rs1, rs2
sltu rd, rs1, rs2

## I-type
addi  rd, rs1, imm
xori  rd, rs1, imm
ori   rd, rs1, imm
andi  rd, rs1, imm
slli  rd, rs1, imm
srli  rd, rs1, imm
srai  rd, rs1, imm
slti  rd, rs1, imm
sltiu rd, rs1, imm

lb  rd, imm(rs1) / lb  rd, [rs1,imm]
lh  rd, imm(rs1) / lh  rd, [rs1,imm]
lw  rd, imm(rs1) / lw  rd, [rs1,imm]
lbu rd, imm(rs1) / lbu rd, [rs1,imm]
lhu rd, imm(rs1) / lhu rd, [rs1,imm]

## S-type
sb  rs2, imm(rs1) / sb rs2, [rs1,imm]
sh  rs2, imm(rs1) / sh rs2, [rs1,imm]
sw  rs2, imm(rs1) / sw rs2, [rs1,imm]

## B-type
beq  rs1, rs2, imm
bne  rs1, rs2, imm
blt  rs1, rs2, imm
bge  rs1, rs2, imm
bltu rs1, rs2, imm
bgeu rs1, rs2, imm

## J-type
jal  rd, imm

## I-type
jalr rd, imm(rs1) / jalr rd, rs1, imm

## U-type
lui   rd, imm
auipc rd, imm

## I-type
ecall

ebreak
