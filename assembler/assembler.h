#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../mlibs/onegin/onegin.h"
#include "../config.h"

const int VERSION = 1;
const int SIGNATURE = 74 * 256 + 67;  // CJ

enum AsmErrorCodes {
    OK                      =  0,
    INCORRECT_FILE_NAME     = -1,
    CANNOT_OPEN_FILE        = -2,
    COMMANDS_NULL           = -3,
    NULL_PTR                = -4,
    INCORRECT_FORMAT        = -5,
};

struct Assembler_t {
    int commandBytes = 0;
    int commandCount = 0;
    char *machineCommands = nullptr;
    Strings humanCommands = {};
};

const int MAX_COMMAND_LENGTH = 10;

static const char *baseOutputName = "output.txt";

int checkCommands(Strings *commands);

int execute(const char *fileName, const char *outputName = baseOutputName);

int compile(Assembler_t *assembler, int *labels = nullptr);

int generateMachineFile(Assembler_t *assembler, const char *fileName);

int hasBrackets(char *command);

int parsePushPop(Assembler_t *assembler, int ip, char commandId);

int parseJump(Assembler_t *assembler, int ip, int *needSecondCompile, int *labels);

int freeAssembler(Assembler_t *assembler);

#endif