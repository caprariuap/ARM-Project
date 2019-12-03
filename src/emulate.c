#include "emulate_lib/decode.h"
#include "emulate_lib/execute.h"

int32_t fetch(int32_t *memory, int32_t *registers);
void printEverything(int32_t *memory, int32_t *registers);

int main(int32_t argc, char **argv) {
    // Check if the programs gets the file argument
    if (argc != 2) {
        fprintf(stderr, "Program has the incorrect number of arguments");
        return EXIT_SUCCESS;
    }

    //Declare the variables
    FILE *binfile;
    int32_t *memory, *registers, *currentLocation, buffer[1], toDecode = 0;
    struct Instruction toExecute;

    //Initialize
    memory = malloc(two16);
    memset(memory, 0, two16);
    registers = malloc(noRegisters * 4);
    memset(registers, 0, noRegisters * 4);
    currentLocation = memory;

    //Reading the file
    binfile = fopen(*(argv + 1), "rb");
    while (!feof(binfile)) {
        fread(buffer, 4, 1, binfile);
        *currentLocation = buffer[0];
        currentLocation++;
        memset(buffer, 0, 4);
    }
    fclose(binfile);

    //Initialize pc and run the code
    registers[PC] = 8;
    while (1) {
        toDecode = fetch(memory, registers);
        if (toDecode == 0) {
            break;
        }
        toExecute = decode(toDecode);
        execute(toExecute, memory, registers);
        registers[PC] += 4;
    }

    printEverything(memory, registers);
    free(memory);
    free(registers);
    return EXIT_SUCCESS;
}

//Fetches next instruction in memory
int32_t fetch(int32_t *memory, int32_t *registers) {
    return *(memory + ((registers[PC] - 8) / 4));
}

void printEverything(int32_t *memory, int32_t *registers) {
    int32_t i;
    printf("Registers:\n");
    for (i = 0; i < 13; i++) {
        if (registers[i]) {
            printf("$%-3u: %10d (%#010x)\n", i, registers[i], registers[i]);
        } else {
            printf("$%-3u: %10d (0x%#08x)\n", i, registers[i], registers[i]);
        }
    }
    if (registers[PC]) {
        printf("PC  :%11d (%#010x)\n", registers[PC], registers[PC]);
    } else {
        printf("PC  : %10d (0x00000000)\n", 0);
    }
    if (registers[CPSR]) {
        printf("CPSR: %10d (%#010x)\n", registers[CPSR], registers[CPSR]);
    } else {
        printf("CPSR: %10d (0x00000000)\n", 0);
    }
    printf("Non-zero memory:\n");
    if (memory[0] != 0) {
        printf("0x%#08x: %#010x\n", 0, reversedBytes(memory[0]));
    }
    int32_t memsize = two16 / 4;
    for (i = 1; i < memsize; i++) {
        if (memory[i] != 0) {
            printf("%#010x: %#010x\n", i * 4, reversedBytes(memory[i]));
        }
    }
}


