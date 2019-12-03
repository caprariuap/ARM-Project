#include "decode.h"

//Calls an instruction depending on the pattern of bits
struct Instruction decode(int32_t instruction) {
  if (checkBit(instruction, 27)) {
    return decodeBranch(instruction);
  }
  if (checkBit(instruction, 26)) {
    return decodeSingleDataTransfer(instruction);
  }
  if (checkBit(instruction, 25)) {
    return decodeDataProcessing(instruction);
  }
  if (checkBit(instruction, 4) && checkBit(instruction, 7)) {
    return decodeMultiply(instruction);
  }
  return decodeDataProcessing(instruction);
}

struct Instruction decodeDataProcessing(int32_t instruction) {
  struct Instruction result;
  memset(&result, 0, sizeof(struct Instruction));
  result.iType = DATAPROCESSING;
  result.cond = getSeqRev(28, 31, instruction);
  result.dataProcessing.opCode = getSeqRev(21, 24, instruction);
  result.dataProcessing.rn = getSeqRev(16, 19, instruction);
  result.dataProcessing.rd = getSeqRev(12, 15, instruction);
  result.dataProcessing.iFlag = checkBit(instruction, 25);
  result.dataProcessing.sFlag = checkBit(instruction, 20);
  if (result.dataProcessing.iFlag) {
    result.dataProcessing.immValue = getSeqRev(0, 7, instruction);
    result.dataProcessing.shConstant = getSeqRev(8, 11, instruction);
    return result;
  }
  result.dataProcessing.rm = getSeqRev(0, 3, instruction);
  result.dataProcessing.shType = getSeqRev(5, 6, instruction);
  if (checkBit(instruction, 4)) {
    result.dataProcessing.bit4 = 1;
    result.dataProcessing.shRegister = getSeqRev(8, 11, instruction);
    return result;
  }
  result.dataProcessing.shConstant = getSeqRev(7, 11, instruction);
  return result;
}

struct Instruction decodeMultiply(int32_t instruction) {
  struct Instruction result;
  memset(&result, 0, sizeof(struct Instruction));
  result.iType = MULTIPLY;
  result.cond = getSeqRev(28, 31, instruction);
  result.multiply.aFlag = checkBit(instruction, 21);
  result.multiply.sFlag = checkBit(instruction, 20);
  result.multiply.rd = getSeqRev(16, 19, instruction);
  result.multiply.rn = getSeqRev(12, 15, instruction);
  result.multiply.rs = getSeqRev(8, 11, instruction);
  result.multiply.rm = getSeqRev(0, 3, instruction);
  return result;
}

struct Instruction decodeSingleDataTransfer(int32_t instruction) {
  struct Instruction result;
  memset(&result, 0, sizeof(struct Instruction));
  result.iType = SINGLEDATATRANSFER;
  result.cond = getSeqRev(28, 31, instruction);
  result.singleDataTransfer.iFlag = checkBit(instruction, 25);
  result.singleDataTransfer.pFlag = checkBit(instruction, 24);
  result.singleDataTransfer.uFlag = checkBit(instruction, 23);
  result.singleDataTransfer.lFlag = checkBit(instruction, 20);
  result.singleDataTransfer.rn = getSeqRev(16, 19, instruction);
  result.singleDataTransfer.rd = getSeqRev(12, 15, instruction);
  result.singleDataTransfer.offset = getSeqRev(0, 11, instruction);
  if (result.singleDataTransfer.iFlag) {
    result.singleDataTransfer.rm = getSeqRev(0, 3, instruction);
    result.singleDataTransfer.shType = getSeqRev(5, 6, instruction);
    if (checkBit(instruction, 4)) {
      result.singleDataTransfer.shRegister = getSeqRev(8, 11, instruction);
      return result;
    }
    result.singleDataTransfer.shConstant = getSeqRev(7, 11, instruction);
    return result;
  }
  return result;
}

struct Instruction decodeBranch(int32_t instruction) {
  struct Instruction result;
  memset(&result, 0, sizeof(struct Instruction));
  result.iType = BRANCH;
  result.cond = getSeqRev(28, 31, instruction);
  result.branch.offset = getSeqRev(0, 23, instruction);
  return result;
}
