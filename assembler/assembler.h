#ifndef SECURED_STACK_H
#define SECURED_STACK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../mlibs/onegin/onegin.h"
#include "../mlibs/stack/stack.h"

const int VERSION = 1;
const int SIGNATURE = 67 * 256 + 74;  // CJ

enum AsmErrorCodes {
    OK                      =  0,
    INCORRECT_FILE_NAME     = -1,
    CANNOT_OPEN_FILE        = -2,
    COMMANDS_NULL           = -3,
    NULL_PTR                = -4,
};

enum LanguageDict {
    PUSH = 0,
    ADD  = 1,
    DIV  = 2,
    OUT  = 3,
    IN   = 4,
    HLT  = 5,   
};

const int MAX_COMMAND_LENGTH = 10;

static const char *language[] = {
    "PUSH",
    "ADD",
    "DIV",
    "OUT",
    "IN",
    "HLT",
};

static const char *baseOutputName = "output.txt";

int checkCommands(Strings *commands);

int parseAsm(const char *fileName, const char *outputName = baseOutputName);

int translateCommand(Strings *commands, const char *outputName);

#endif