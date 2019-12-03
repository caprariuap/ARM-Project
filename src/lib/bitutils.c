#include "bitutils.h"

//Checks bit at given position
int32_t checkBit(int32_t seq, int32_t pos) {
    return (seq >> pos) % 2 != 0;
}

//Sets bit at given position to a given value
void setBitAtPos(int32_t pos, int32_t value, int32_t *number) {
    if (value) {
        *number |= (1 << pos);
        return;
    }
    if (*number & (1 << pos)) {
        *number ^= (1 << pos);
    }
}

//Sets bits between pos1 and pos2 in number to the first pos2 - pos1 + 1 bits of value
void setSeq(int32_t pos1, int32_t pos2, int32_t value, int32_t *number) {
    int i;
    for (i = pos1; i <= pos2; i++) {
        setBitAtPos(i, checkBit(value, i - pos1), number);
    }
}

//Returns bits between pos1 and pos2 in the reverse order
int32_t getSeqRev(int32_t pos1, int32_t pos2, int32_t instruction) {
    int32_t result = 0, i;
    for (i = 0; i <= pos2 - pos1; i++) {
        setBitAtPos(i,
                    checkBit(instruction, pos1 + i), &result);
    }
    return result;
}

//Reverses the order of the bytes in number
int32_t reversedBytes(int32_t number) {
    u_int32_t byte, mask = 255;
    byte = (u_int32_t) number;
    return (byte >> 24) + ((byte >> 8) & (mask << 8)) +
           ((byte << 8) & (mask << 16)) + ((byte << 24) & (mask << 24));;
}

//Functions for shifts and rotates; all return the carry
int32_t ror(int32_t *n, int32_t noTimes) {
    if (!noTimes) {
        return 0;
    }
    *n = (int32_t)(((u_int32_t) * n) << (32 - noTimes)) + (((u_int32_t) * n) >> noTimes);
    return (*n >> 31) % 2;
}

int32_t lsl(int32_t *n, int32_t noTimes) {
    if (!noTimes) {
        return 0;
    }
    int32_t carry = (*n >> (32 - noTimes)) % 2;
    *n <<= noTimes;
    return carry;
}

int32_t lsr(int32_t *n, int32_t noTimes) {
    if (!noTimes) {
        return 0;
    }
    int32_t carry = (*n >> (noTimes - 1)) % 2;
    *n = (int32_t)(((u_int32_t) * n) >> noTimes);
    return carry;
}

int32_t asr(int32_t *n, int32_t noTimes) {
    if (!noTimes) {
        return 0;
    }
    int32_t carry = (*n >> (noTimes - 1)) % 2;
    *n >>= noTimes;
    return carry;
}

//Gets memory from pos and the next 3 bytes
int32_t getMemory(int32_t pos, int32_t *memory) {
    u_int32_t startByte = reversedBytes(memory[pos / 4]),
            endByte = reversedBytes(memory[pos / 4 + 1]);
    u_int32_t result = startByte << (8 * (pos % 4));
    result += pos % 4 == 0 ? 0 : (endByte >> (8 * (4 - pos % 4)));
    return reversedBytes((int32_t) result);
}

//Changes memory at pos and the next 3 bytes to value in little endian
void changeMemory(int32_t pos, int32_t value, int32_t *memory) {
    u_int32_t startByte = reversedBytes(memory[pos / 4]),
            endByte = reversedBytes(memory[pos / 4 + 1]), noCarryBytes;
    noCarryBytes = pos % 4;
    if (!noCarryBytes) {
        memory[pos / 4] = value;
        return;
    }
    value = reversedBytes(value);
    startByte = ((startByte >> (32 - 8 * noCarryBytes) << (32 - 8 * noCarryBytes)))
                + (value >> (8 * noCarryBytes));
    endByte = ((endByte << (8 * noCarryBytes)) >> (8 * noCarryBytes))
              + (value << (32 - 8 * noCarryBytes));
    memory[pos / 4] = reversedBytes(startByte);
    memory[pos / 4 + 1] = reversedBytes(endByte);
}

void writeBinaryFile(FILE *file, u_int32_t binOut) {
    //Writes a given number in binary to a given file, then resets the numbers bits.
    fwrite(&binOut, sizeof(u_int32_t), 1, file);
    memset(&binOut, 0, 4);
}
