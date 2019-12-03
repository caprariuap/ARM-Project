#ifndef EXECUTE_H
#define  EXEUCTE_H

#include "../lib/bitutils.h"
#include "../lib/data_types.h"

int32_t checkGPIO(int32_t address);

void execute(struct Instruction instruction, int32_t *memory, int32_t *registers);

void executeDataProcessing(struct DataProcessing instruction, int32_t *registers);

void
executeDataTransfer(struct SingleDataTransfer instruction, int32_t *memory, int32_t *registers);

void executeMultiply(struct Multiply instruction, int32_t *registers);

void executeBranch(struct Branch instruction, int32_t *registers);

#endif
