#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../mlibs/onegin/onegin.h"
#include "../config.h"

const int VERSION = 1;
const int SIGNATURE = 74 * 256 + 67;  // CJ

const int MAX_LABEL_LEN = 128;
const int MAX_COMPILE_ATTEMPTS = 3;

enum AsmErrorCodes {
    OK                      =  0,
    INCORRECT_FILE_NAME     = -1,
    CANNOT_OPEN_FILE        = -2,
    COMMANDS_NULL           = -3,
    NULL_PTR                = -4,
    INCORRECT_FORMAT        = -5,
    TOO_MANY_COMPILES       = -6,
};

struct Assembler_t {
    int commandBytes = 0;
    int commandCount = 0;
    char *machineCommands = nullptr;
    Strings humanCommands = {};
};

struct Label_t {
    char labelTxt[MAX_LABEL_LEN] = {};
    int size = 0;
    int gotoIp = 0;
};

const int MAX_COMMAND_LENGTH = 10;

static const char *baseOutputName = "output.txt";

int checkCommands(Strings *commands);

int execute(const char *fileName, const char *outputName = baseOutputName);

int compile(Assembler_t *assembler, Label_t *labels, int *labelCount, int compileCount = 0);

int generateMachineFile(Assembler_t *assembler, const char *fileName);

int hasBrackets(char *command);

int parsePushPop(Assembler_t *assembler, int ip, char commandId);

int parseJumpCall(Assembler_t *assembler, int ip, char commandType, int *needSecondCompile, Label_t *labels, int *labelCount);

int freeMemory(Assembler_t *assembler, Label_t *labels);

#endif