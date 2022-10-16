#ifndef DISASM_H
#define DISASM_H

#include <stdio.h>
#include <malloc.h>

#include "../config.h"


enum DasmErrorCodes {
    NO_ERROR                =  0,
    INCORRECT_FILE_NAME     = -1,
    CANNOT_OPEN_FILE        = -2,
    COMMANDS_NULL           = -3,
    NULL_PTR                = -4,
    UNKNOWN_COMMAND         = -5,
};

int parseMachineCode(const char *input, const char *output = "code.txt");

int translateCommand(FILE *input, FILE *output, int *labels = nullptr);

int poisonLabels(int *labels, int labelSize);

int readLabels(char *commands, int comSize, int *labels);

int addLabel(int *labels, int label);

int createRegStr(char *str, int value);

int printCommand(FILE *output, char maskArgs, char popArgs, char *commands, int *ip, int *commandCount);

#endif