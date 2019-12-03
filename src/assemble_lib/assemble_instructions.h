#ifndef ASSEMBLE_INSTRUCTIONS_H
#define ASSEMBLE_INSTRUCTIONS_H

#include "../lib/bitutils.h"
#include "../lib/data_types.h"

int32_t searchLabelValue(char *p, struct key_value *labelArray);

int32_t assembleDataProcessing(enum mnemonicsEnum mnemonic, char operands[6][30]);

int32_t assembleMultiply(enum mnemonicsEnum mnemonic, char operands[6][30]);

int32_t
assembleSingleDataTransfer(enum mnemonicsEnum mnemonic, char operands[6][30], int32_t noOperands,
                           int32_t currentAddress, int32_t *addedMemory, int32_t *noAddedMemory,
                           int32_t noInstructions);

int32_t
assembleBranch(enum mnemonicsEnum mnemonic,
               char operands[6][30],
               struct key_value *labelArray,
               int32_t currentAddress);

int32_t assembleSpecial(enum mnemonicsEnum mnemonic, char operands[6][30]);

int32_t setOperand2(int32_t operandsIndex,
                    char operands[6][30],
                    int32_t result,
                    enum mnemonicsEnum mnemonic);

#endif
