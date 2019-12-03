#ifndef DECODE_H
#define  DECODE_H

#include "../lib/bitutils.h"
#include "../lib/data_types.h"

struct Instruction decode(int32_t instruction);

struct Instruction decodeDataProcessing(int32_t instruction);

struct Instruction decodeMultiply(int32_t instruction);

struct Instruction decodeSingleDataTransfer(int32_t instruction);

struct Instruction decodeBranch(int32_t instruction);

#endif
