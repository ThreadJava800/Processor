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

const int MAX_COMMAND_LENGTH = 10;

static const char *baseOutputName = "output.txt";

int checkCommands(Strings *commands);

int parseAsm(const char *fileName, const char *outputName = baseOutputName);

int translateCommand(Strings *commands, const char *outputName, int *labels = nullptr);

int parseLine(char *buf, int *count, int *commandId, int *arg1, int *arg2);

#endif