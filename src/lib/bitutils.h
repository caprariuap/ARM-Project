#ifndef __BITUTILS_H
#define __BITUTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

int32_t checkBit(int32_t seq, int32_t pos);

void setBitAtPos(int32_t pos, int32_t value, int32_t *number);

void setSeq(int32_t pos1, int32_t pos2, int32_t value, int32_t *number);

int32_t getSeqRev(int32_t pos1, int32_t pos2, int32_t instruction);

int32_t reversedBytes(int32_t number);

int32_t ror(int32_t *n, int32_t noTimes);

int32_t lsl(int32_t *n, int32_t noTimes);

int32_t lsr(int32_t *n, int32_t noTimes);

int32_t asr(int32_t *n, int32_t noTimes);

int32_t getMemory(int32_t pos, int32_t *memory);

void changeMemory(int32_t pos, int32_t value, int32_t *memory);

void writeBinaryFile(FILE *file, u_int32_t binOut);

#endif
