#include "assemble_instructions.h"

//Each assemble function sets its integer instruction to be passed 
int32_t assembleDataProcessing(enum mnemonicsEnum mnemonic, char operands[6][30]) {
  int32_t result = 0, opCode, cond = 14; //1110
  setSeq(28, 31, cond, &result);
  if (mnemonic >= ADD && mnemonic <= ORR) {
    switch (mnemonic) {
      case ADD:
        opCode = 4;  //0100
        break;
      case SUB:
        opCode = 2;  //0010
        break;
      case RSB:
        opCode = 3;  //0011
        break;
      case AND:
        opCode = 0;  //0000
        break;
      case EOR:
        opCode = 1;  //0001
        break;
      case ORR:
        opCode = 12; //1100
        break;
      default:
        perror("this should be impossible: invalid mnemonic");
    }
    int32_t rd = getRegister(operands[0]), rn = getRegister(operands[1]);
    setSeq(21, 24, opCode, &result);
    setSeq(16, 19, rn, &result);
    setSeq(12, 15, rd, &result);
    return setOperand2(2, operands, result, mnemonic);
  }
  if (mnemonic == MOV) {
    int32_t rd = getRegister(operands[0]);
    opCode = 13; //1101
    setSeq(21, 24, opCode, &result);
    setSeq(12, 15, rd, &result);
    return setOperand2(1, operands, result, mnemonic);
  }
  switch (mnemonic) {
    case TST:
      opCode = 8;  //1000
      break;
    case TEQ:
      opCode = 9;  //10001
      break;
    case CMP:
      opCode = 10; //1010
      break;
    default:
      perror("this should be impossible: invalid mnemonic");
  }
  int32_t rn = getRegister(operands[0]);
  setBitAtPos(20, 1, &result);
  setSeq(21, 24, opCode, &result);
  setSeq(16, 19, rn, &result);
  return setOperand2(1, operands, result, mnemonic);
}

int32_t assembleMultiply(enum mnemonicsEnum mnemonic, char operands[6][30]) {
  int32_t result = 0, cond = 14; //1110
  int32_t mask = 9; // 1001
  int32_t rd = getRegister(operands[0]), rm = getRegister(operands[1]),
      rs = getRegister(operands[2]);
  setSeq(28, 31, cond, &result);
  setSeq(16, 19, rd, &result);
  setSeq(0, 3, rm, &result);
  setSeq(8, 11, rs, &result);
  setSeq(4, 7, mask, &result);
  if (mnemonic == MLA) {
    int32_t rn = getRegister(operands[3]);
    setSeq(12, 15, rn, &result);
    setBitAtPos(21, 1, &result);
  }
  return result;
}

int32_t
assembleSingleDataTransfer(enum mnemonicsEnum mnemonic, char operands[6][30], int32_t noOperands,
                           int32_t currentAddress, int32_t *addedMemory, int32_t *noAddedMemory,
                           int32_t noInstructions) {
  int32_t result = 0, cond = 14, rd = getRegister(operands[0]);//, offset;
  setSeq(12, 15, rd, &result);
  setSeq(28, 31, cond, &result);
  setBitAtPos(26, 1, &result);
  setBitAtPos(23, 1, &result);
  if (mnemonic == LDR) {
    setBitAtPos(20, 1, &result);
    if (operands[1][0] == '=') {
      int32_t expression = getImmediateValue(operands[1]);
      if (expression <= 0xFF) {
        operands[1][0] = '#';
        return assembleDataProcessing(MOV, operands);
      }
      setBitAtPos(24, 1, &result);
      setSeq(16, 19, PC, &result);
      addedMemory[0] = expression;
      *noAddedMemory = *noAddedMemory + 1;
      setSeq(0, 11, (*noAddedMemory + noInstructions) * 4 - currentAddress - 12, &result);
      return result;
    }
  }
  int32_t rn = getRegister(operands[1] + 1);
  setSeq(16, 19, rn, &result);
  if (noOperands == 2) {
    setBitAtPos(24, 1, &result);
    return result;
  }
  if (strchr(operands[1], ']')) {
    return setOperand2(2, operands, result, mnemonic);
  }
  setBitAtPos(24, 1, &result);
  return setOperand2(2, operands, result, mnemonic);
}

int32_t
assembleBranch(enum mnemonicsEnum mnemonic, char operands[6][30], struct key_value *labelArray,
               int32_t currentAddress) {
  int32_t result = 0;
  int32_t cond = 14;
  if (mnemonic >= BEQ && mnemonic <= B) {
    switch (mnemonic) {
      case BEQ:
        cond = 0;  //0000
        break;
      case BNE:
        cond = 1;  //0001
        break;
      case BGE:
        cond = 10; //1010
        break;
      case BLT:
        cond = 11; //1011
        break;
      case BGT:
        cond = 12; //1100
        break;
      case BLE:
        cond = 13; //1101
        break;
      case B:
        cond = 14; //1110
        break;
      default:
        perror("this should be impossible: invalid mnemonic");
    }
  }
  int32_t mask = 5; // 101
  setSeq(28, 31, cond, &result);
  setSeq(25, 27, mask, &result);
  int32_t offset = 0, value = searchLabelValue(operands[0], labelArray);
  if (value >= 0) {
    offset = value - currentAddress - 8;
  } else {
    offset = currentAddress - 8;
  }
  offset >>= 2;
  setSeq(0, 23, offset, &result);
  return result;

}

int32_t assembleSpecial(enum mnemonicsEnum mnemonic, char operands[6][30]) {
  if (mnemonic == ANDEQ) {
    return 0;
  }
  strcpy(operands[3], operands[1]);
  strcpy(operands[1], operands[0]);
  strcpy(operands[2], "lsl");
  return assembleDataProcessing(MOV, operands);
}

//Sets the second operands in dataProccessing and dataTransfer
int32_t setOperand2(int32_t operandsIndex,
                    char operands[6][30],
                    int32_t result,
                    enum mnemonicsEnum mnemonic) {
  if (isImmediateValue(operands[operandsIndex])) {
    if (mnemonic != LDR && mnemonic != STR) {
      setBitAtPos(25, 1, &result);
    }
    int32_t immValue, i;
    if (operands[operandsIndex][1] == '-') {
      immValue = getImmediateValue(operands[operandsIndex] + 1);
      setBitAtPos(23, 0, &result);
    } else {
      if (operands[operandsIndex][1] == '+') {
        immValue = getImmediateValue(operands[operandsIndex] + 1);
      } else {
        immValue = getImmediateValue(operands[operandsIndex]);
      }
    }
    if ((u_int32_t) immValue >= two8) {
      for (i = 30; i >= 0; i -= 2) {
        int32_t copyImm = immValue;
        ror(&copyImm, i);
        if ((u_int32_t) copyImm < two8) {
          setSeq(0, 7, copyImm, &result);
          setSeq(8, 11, (32 - i) / 2, &result);
          return result;
        }
      }
      perror("Immediate value can't be represented");
    }
    setSeq(0, 7, immValue, &result);
    return result;
  }
  if (mnemonic == LDR || mnemonic == STR) {
    setBitAtPos(25, 1, &result);
  }
  int32_t rm;
  if (operands[operandsIndex][1] == '-') {
    rm = getRegister(operands[operandsIndex] + 1);
    setBitAtPos(23, 0, &result);
  } else {
    if (operands[operandsIndex][1] == '+') {
      rm = getRegister(operands[operandsIndex] + 1);
    } else {
      rm = getRegister(operands[operandsIndex]);
    }
  }
  setSeq(0, 3, rm, &result);
  operandsIndex++;
  if (operands[operandsIndex][0] == '\0') {
    return result;
  }
  enum shiftTypes shiftType = getShiftType(operands[operandsIndex]);
  int32_t shiftCode = (int32_t) shiftType;
  setSeq(5, 6, shiftCode, &result);
  operandsIndex++;
  if (isImmediateValue(operands[operandsIndex])) {
    int32_t immValue = getImmediateValue(operands[operandsIndex]);
    setSeq(7, 11, immValue, &result);
    return result;
  }
  setBitAtPos(4, 1, &result);
  int32_t rs = getRegister(operands[operandsIndex]);
  setSeq(8, 11, rs, &result);
  return result;
}
