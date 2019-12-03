#include "data_types.h"

static const char *mnemonics[] = {"add", "sub", "rsb", "and", "eor", "orr", "mov", "tst", "teq", "cmp", "mul", "mla",
                                  "ldr",
                                  "str", "beq", "bne", "bge", "blt", "bgt", "ble", "b", "lsl", "andeq"};

int32_t searchLabelValue(char *p, struct key_value *labelArray) {
    int i = 0;
    while (labelArray[i].key) {
        if (!strcmp(labelArray[i].key, p)) {
            return labelArray[i].value;
        }
        i++;
    }
    return -1; //Or any negative number
}

enum mnemonicsEnum findMnemonic(char *p) {
    int32_t i;
    for (i = 0; i <= 22; i++) {
        if (!strcmp(p, mnemonics[i])) {
            return i;
        }
    }
    perror("instruction not found");
    return -1;
}

enum shiftTypes getShiftType(char *p) {
    assert(p && strlen(p) >= 3);
    if (p[0] == 'r') {
        return ROR;
    }
    if (p[0] == 'a') {
        return ASR;
    }
    if (p[2] == 'l') {
        return LSL;
    }
    return LSR;
}

bool isImmediateValue(char *p) {
    assert(p != NULL && strlen(p) > 0);
    return p[0] == '#';
}

long int getImmediateValue(char *p) {
    char *value_ptr = p + 1;
    int32_t l = strlen(value_ptr);
    if (value_ptr[l - 1] == ']') {
        value_ptr[l - 1] = '\0';
    }
    if (p[1] == '0' && p[2] == 'x') {
        return strtol(value_ptr, 0, 16);
    }
    return strtol(value_ptr, 0, 10);
}

int32_t getRegister(char *p) {
    assert(p && strlen(p) > 1 && p[0] == 'r');
    if (p[2] >= '0' && p[2] <= '9') {
        return (p[1] - '0') * 10 + p[2] - '0';
    }
    return p[1] - '0';
}