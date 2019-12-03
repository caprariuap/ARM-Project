#include "execute.h"

//Checks if the address is GPIO-related and prints the respective information
int32_t checkGPIO(int32_t address) {
  if (address == GPIO[0]) {
    printf("One GPIO pin from 0 to 9 has been accessed\n");
    return 1;
  }
  if (address == GPIO[1]) {
    printf("One GPIO pin from 10 to 19 has been accessed\n");
    return 1;
  }
  if (address == GPIO[2]) {
    printf("One GPIO pin from 20 to 29 has been accessed\n");
    return 1;
  }
  if (address == GPIO[3]) {
    printf("PIN OFF\n");
    return 1;
  }
  if (address == GPIO[4]) {
    printf("PIN ON\n");
    return 1;
  }
  return 0;
}

//Checks if cpsr satisfies given condition
int32_t checkCond(int32_t cond, int32_t cpsr) {
  switch (cond) {
    case 0:  //0000
      return checkBit(cpsr, 30);
    case 1:  //0001
      return !checkBit(cpsr, 30);
    case 10: //1010
      return checkBit(cpsr, 31) ==
          checkBit(cpsr, 28);
    case 11: //1011
      return checkBit(cpsr, 31) !=
          checkBit(cpsr, 28);
    case 12: //1100
      return !checkBit(cpsr, 30) &&
          (checkBit(cpsr, 31) ==
              checkBit(cpsr, 28));
    case 13: //1101
      return checkBit(cpsr, 30) ||
          (checkBit(cpsr, 31) !=
              checkBit(cpsr, 28));
    case 14: //1110
      return 1;
    default:
      printf("Invalid condition %d\n", cond);
      return 0;
  }
}

//Calls functions to execute an instruction depending on its type
void execute(struct Instruction instruction, int32_t *memory, int32_t *registers) {
  if (!checkCond(instruction.cond, registers[CPSR])) {
    return;
  }
  switch (instruction.iType) {
    case 0:
      executeDataProcessing(instruction.dataProcessing, registers);
      return;
    case 1:
      executeMultiply(instruction.multiply, registers);
      return;
    case 2:
      executeDataTransfer(instruction.singleDataTransfer, memory, registers);
      return;
    case 3:
      executeBranch(instruction.branch, registers);
      return;
  }
}

void executeDataProcessing(struct DataProcessing instruction, int32_t *registers) {
  int32_t operand2, c = 0, z = 0, n = 0;
  if (instruction.iFlag) {
    operand2 = instruction.immValue;
    c = ror(&operand2, 2 * instruction.shConstant);
  } else {
    operand2 = registers[instruction.rm];
    int32_t shiftWith = instruction.bit4 ?
                        registers[instruction.shRegister] :
                        instruction.shConstant;
    switch (instruction.shType) {
      case 0:
        c = lsl(&operand2, shiftWith);
        break;
      case 1:
        c = lsr(&operand2, shiftWith);
        break;
      case 2:
        c = asr(&operand2, shiftWith);
        break;
      case 3:
        c = ror(&operand2, shiftWith);
        break;
      default:
        printf("Incorrect shift type");
    }
  }
  switch (instruction.opCode) {
    case 0:  //0000 and
      z = registers[instruction.rd] = registers[instruction.rn] & operand2;
      break;
    case 1:  //0001 eor
      z = registers[instruction.rd] = registers[instruction.rn] ^ operand2;
      break;
    case 2:  //0010 sub
      z = registers[instruction.rd] = registers[instruction.rn] - operand2;
      c = z >= 0;
      break;
    case 3:  //0011 rsb
      z = registers[instruction.rd] = operand2 - registers[instruction.rn];
      c = z >= 0;
      break;
    case 4:  //0100 add
      z = registers[instruction.rd] = registers[instruction.rn] + operand2;
      c = z < operand2;
      break;
    case 8:  //1000 tst
      z = registers[instruction.rn] & operand2;
      break;
    case 9:  //1001 teq
      z = registers[instruction.rn] ^ operand2;
      break;
    case 10: //1010 cmp
      z = registers[instruction.rn] - operand2;
      c = z >= 0;
      break;
    case 12: //1100 orr
      z = registers[instruction.rd] = registers[instruction.rn] | operand2;
      break;
    case 13: //1101 mov
      z = registers[instruction.rd] = operand2;
      break;
    default:
      printf("Incorrect operation code");
  }
  n = checkBit(z, 31);
  z = z ? 0 : 1;
  if (instruction.sFlag) {
    setBitAtPos(31, n, &registers[CPSR]);
    setBitAtPos(30, z, &registers[CPSR]);
    setBitAtPos(29, c, &registers[CPSR]);
  }
}

void
executeDataTransfer(struct SingleDataTransfer instruction, int32_t *memory, int32_t *registers) {
  int32_t offset = instruction.offset;
  if (instruction.iFlag) {
    if (checkBit(instruction.offset, 4)) {
      instruction.shConstant = registers[instruction.shRegister];
    }
    offset = registers[instruction.rm];
    switch (instruction.shType) {
      case 0:
        lsl(&offset, instruction.shConstant);
        break;
      case 1:
        lsr(&offset, instruction.shConstant);
        break;
      case 2:
        asr(&offset, instruction.shConstant);
        break;
      case 3:
        ror(&offset, instruction.shConstant);
        break;
    }
  }
  if (!instruction.uFlag) {
    offset = -offset;
  }
  if (instruction.lFlag) {
    if (instruction.pFlag) {
      if (checkGPIO(registers[instruction.rn] + offset)) {
        registers[instruction.rd] = registers[instruction.rn] + offset;
        return;
      }
      if (registers[instruction.rn] + offset >= two16
          || registers[instruction.rn] + offset < 0) {
        printf("Error: Out of bounds memory access at address %#010x\n",
               registers[instruction.rn] + offset);
        return;
      }
      registers[instruction.rd] = getMemory(registers[instruction.rn] + offset, memory);
      return;
    }
    if (checkGPIO(registers[instruction.rn])) {
      registers[instruction.rd] = registers[instruction.rn];
      return;
    }
    if (registers[instruction.rn] >= two16
        || registers[instruction.rn] < 0) {
      printf("Error: Out of bounds memory access at address %#010x\n",
             registers[instruction.rn]);
      return;
    }
    registers[instruction.rd] = getMemory(registers[instruction.rn], memory);
    registers[instruction.rn] += offset;
    return;
  }
  if (instruction.pFlag) {
    if (checkGPIO(registers[instruction.rn] + offset)) {
      return;
    }
    if (registers[instruction.rn] + offset >= two16
        || registers[instruction.rn] + offset < 0) {
      printf("Error: Out of bounds memory access at address %#010x\n",
             registers[instruction.rn] + offset);
      return;
    }
    changeMemory(registers[instruction.rn] + offset, registers[instruction.rd], memory);
    return;
  }
  if (checkGPIO(registers[instruction.rn])) {
    return;
  }
  if (registers[instruction.rn] >= two16
      || registers[instruction.rn] < 0) {
    printf("Error: Out of bounds memory access at address %#010x\n",
           registers[instruction.rn]);
    return;
  }
  changeMemory(registers[instruction.rn], registers[instruction.rd], memory);
  registers[instruction.rn] = registers[instruction.rn] + offset;
}

void executeMultiply(struct Multiply instruction, int32_t *registers) {
  if (!instruction.aFlag) {
    registers[instruction.rd] = registers[instruction.rm] *
        registers[instruction.rs];
  } else {
    registers[instruction.rd] = registers[instruction.rm] *
        registers[instruction.rs] +
        registers[instruction.rn];
  }
  if (instruction.sFlag) {
    setBitAtPos(31, checkBit(registers[instruction.rd], 31), &registers[CPSR]);
    if (registers[instruction.rd] == 0) {
      setBitAtPos(30, 1, &registers[CPSR]);
    }
  }

}

void executeBranch(struct Branch instruction, int32_t *registers) {
  instruction.offset <<= 2;
  if (checkBit(instruction.offset, 25)) {
    instruction.offset |= mask24to31bit;
  }
  registers[PC] += instruction.offset + 4;
}

