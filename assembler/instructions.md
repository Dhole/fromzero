ADCS Rd, Rd, Rm
ADDS Rd, Rn, imm
ADDS Rd, imm
ADDS Rd, Rn, Rm
ADD Rd, SP Rd
ADDS Rd Rm
ADD Rd, Rd, Rm
ADD Rd, PC, imm
ADR Rd, label
ADD Rd, SP imm
ADD SP, SP, imm
ANDS Rd, Rd, Rm
ASRS Rd, Rm, imm
ASRS Rd, Rd, Rs
B<cond> label
B label
BICS Rd, Rd, Rm
BKPT imm
BL label
BLX Rm
BX Rm
CMN Rn, Rm
CMP Rn, imm
CMP Rn, Rm
CPS<effect> iflags
CPY Rd, Rn == MOV Rd, Rn
DMB option
DSB option
MOV Rd, Rm
EOR Rd, Rd, Rm
ISB option
LDMIA Rn, registers
LDMIA Rn!, registers
LDR Rd, [Rn, imm]
LDR Rd, [SP, imm]
LDR Rd, [Rn, Rm]
LDR Rd, [PC, imm]
LDR Rd, label
LDRB Rd, [Rn, imm]
LDRB Rd, [Rn, Rm]
LDRH Rd, [Rn, imm]
LDRH Rd, [Rn, Rm]
LDRSB Rd, [Rn, Rm]
LDRSH Rd, [Rn, Rm]
MOVS Rd, Rm
LSLS Rd, Rm, imm
LSRS Rd, Rm, imm
LSRS Rd, Rd, Rs
MOVS Rd, imm
MRS Rd, spec_reg
MSR spec_reg, Rn
ADDS Rd, Rm #0
MOV Rd, Rm
MULS Rd, Rm, Rd
MVNS Rd, Rm
NEG Rd, Rm == RSBS Rd, Rm, #0
NOP
RSBS Rd, Rm, #0
ORRS Rd, Rd, Rm
POP registers
POP registers, PC
PUSH registers
PUSH registers, LR
REV Rd, Rn
REV16 Rd, Rn
REVSH Rd, Rn
RORS Rd, Rd, Rs
SBC Rd, Rd, Rm
SEV
STMIA Rn!, registers
STR Rd, [Rn, imm]
STR Rd, [SP, imm]
STR Rd, [Rn, Rm]
STRB Rd, [Rn, imm]
STRB Rd, [Rn, Rm]
STRH Rd, [Rn, imm]
STRH Rd, [Rn, Rm]
SUBS Rd, RN, imm
SUBS Rd, imm
SUBS Rd, Rn, Rm
SUB Sp, Sp, imm
SVC imm
SXTB Rd, Rm
SXTH Rd, Rm
TST Rd, Rm
UDF imm
UXTB Rd, Rm
UXTH Rd, Rm
WFE
WFI
YELD
