#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

static const int32_t two16 = 65536, noRegisters = 17, mask24to31bit = -33554432;
static const int32_t PC = 15, CPSR = 16, GPIO[] = {0x20200000, 0x20200004, 0x20200008, 0x20200028, 0x2020001c},
        MAX_LINE_SIZE = 50, MAX_LABELS = 50, two8 = 256;

enum shiftTypes {
    LSL, LSR, ASR, ROR
};

enum mnemonicsEnum {
    ADD, SUB, RSB, AND, EOR, ORR, MOV, TST, TEQ, CMP, MUL, MLA, LDR, STR, BEQ, BNE, BGE, BLT, BGT, BLE,
    B, LSLOP, ANDEQ
};

enum InstructionType {
    DATAPROCESSING, MULTIPLY, SINGLEDATATRANSFER, BRANCH
};
struct DataProcessing {
    int32_t iFlag, opCode, sFlag, rn, rd, shConstant, shRegister, shType,
            immValue, rm, bit4;
};
struct Multiply {
    int32_t aFlag, sFlag, rn, rd, rs, rm;
};

struct SingleDataTransfer {
    int32_t iFlag, pFlag, uFlag, lFlag, rn, rd, rm, shRegister, shConstant, shType, offset;
};

struct Branch {
    int32_t offset;
};

struct Instruction {
    int32_t cond;
    enum InstructionType iType;
    struct DataProcessing dataProcessing;
    struct Multiply multiply;
    struct SingleDataTransfer singleDataTransfer;
    struct Branch branch;
};

struct key_value {
    char *key;
    int32_t value;
};

int32_t searchLabelValue(char *p, struct key_value *labelArray);

enum mnemonicsEnum findMnemonic(char *p);

enum shiftTypes getShiftType(char *p);

bool isImmediateValue(char *p);

long int getImmediateValue(char *p);

int32_t getRegister(char *p);

#endif
