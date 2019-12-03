#include "lib/bitutils.h"
#include "lib/data_types.h"
#include "assemble_lib/assemble_instructions.h"

//Tokenize declaration
void
tokenize(char *buffer, int32_t *instruction, struct key_value *labelArray, int32_t currentAddress,
         int32_t *addedMemory, int32_t *noAddedMemory, int32_t noInstructions);

//Sets the mnemonic and splits the instruction into its operands
void
tokenize(char *buffer, int32_t *instruction, struct key_value *labelArray, int32_t currentAddress,
         int32_t *addedMemory, int32_t *noAddedMemory, int32_t noInstructions) {
  int32_t l = strlen(buffer);
  if (l < 1 || buffer[l - 1] == ':') {
    return;
  }
  char *sep = " ,", *p = strtok(buffer, sep), operands[6][30];
  memset(operands, 0, sizeof(operands));
  int32_t mnemonicType = findMnemonic(p), noOperands = 0;
  while (p != NULL) {
    p = strtok(NULL, sep);
    if (p) {
      strcpy(operands[noOperands], p);
      noOperands++;
    }
  }
  if (mnemonicType < MUL) {
    *instruction = assembleDataProcessing(mnemonicType, operands);
    return;
  }
  if (mnemonicType < LDR) {
    *instruction = assembleMultiply(mnemonicType, operands);
    return;
  }
  if (mnemonicType < BEQ) {
    *instruction =
        assembleSingleDataTransfer(mnemonicType, operands, noOperands, currentAddress, addedMemory,
                                   noAddedMemory, noInstructions);
    return;
  }
  if (mnemonicType < LSLOP) {
    *instruction = assembleBranch(mnemonicType, operands, labelArray, currentAddress);
    return;
  }
  *instruction = assembleSpecial(mnemonicType, operands);
}

int main(int argc, char **argv) {
  FILE *srcFile = fopen(*(argv + 1), "r");
  FILE *dstFile = fopen(*(argv + 2), "ab");

  struct key_value *labelArray = calloc(MAX_LABELS, sizeof(struct key_value));
  int32_t *addedMemory = calloc(MAX_LABELS, sizeof(int32_t)), noAddedMemory = 0, noInstructions = 0;
  assert(labelArray && addedMemory);
  int noLabels = 0, currentAddress = 0;

  //First pass
  while (!feof(srcFile)) {
    char buffer[MAX_LINE_SIZE];
    memset(buffer, 0, sizeof(buffer) * sizeof(char));
    fgets(buffer, 50, srcFile);
    int32_t l = strlen(buffer);
    if (l < 2) { //'\n' or nothing (feof marker)
      continue;
    }
    if (buffer[l - 1] == '\n') {
      buffer[l - 1] = '\0';
      l--;
    }
    if (buffer[l - 1] == ':') {
      buffer[l - 1] = '\0';
      labelArray[noLabels].key = calloc(MAX_LINE_SIZE, sizeof(char));
      strcpy(labelArray[noLabels].key, buffer);
      labelArray[noLabels].value = currentAddress;
      noLabels++;
      continue;
    }
    noInstructions++;
    currentAddress += 4;
  }

  fclose(srcFile);
  srcFile = fopen(*(argv + 1), "r");
  currentAddress = 0;

  //Second pass
  while (!feof(srcFile)) {
    char buffer[MAX_LINE_SIZE];
    memset(buffer, 0, sizeof(buffer) * sizeof(char));
    fgets(buffer, 50, srcFile);
    int32_t l = strlen(buffer);
    if (l < 2) {
      continue;
    }
    if (buffer[l - 1] == '\n') {
      buffer[l - 1] = '\0';
      l--;
    }
    if (buffer[l - 1] == ':') {
      continue;
    }
    int32_t instruction = 0;
    tokenize(buffer,
             &instruction,
             labelArray,
             currentAddress,
             addedMemory + noAddedMemory,
             &noAddedMemory,
             noInstructions);
    writeBinaryFile(dstFile, instruction);
    currentAddress += 4;
  }
  int i;
  for (i = 0; i < noAddedMemory; i++) {
    writeBinaryFile(dstFile, addedMemory[i]);
  }
  fclose(srcFile);
  fclose(dstFile);
}