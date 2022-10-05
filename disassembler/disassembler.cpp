#include "disassembler.h"

int parseMachineCode(const char *input, const char *output) {
    int errorCode = NO_ERROR;

    if (!input || !output) {
        return INCORRECT_FILE_NAME;
    }

    FILE *machineCode = fopen(input, "rb");
    FILE *humanCode = fopen(output, "wb");
    if (!machineCode || !humanCode) {
        return CANNOT_OPEN_FILE;
    }

    errorCode = translateCommand(machineCode, humanCode);

    fclose(machineCode);
    fclose(humanCode);

    return errorCode;
}

int translateCommand(FILE *input, FILE *output) {
    int errorCode = NO_ERROR;
    int header[3] = {};

    fread(header, sizeof(int), 3, input);

    if (!header) {
        return CANNOT_OPEN_FILE;
    }

    fprintf(output, "%d %d %d\n", header[0], header[1], header[2]);
    int *commands = (int *) calloc(sizeof(int), header[2]);
    if (!commands) {
        return NULL_PTR;
    }
    fread(commands, sizeof(int), header[2], input);

    for (int ip = 0; ip < header[2]; ip++) {
        switch (commands[ip]) {
            case PUSH:
                fprintf(output, "PUSH %d\n", commands[++ip]);
                break;
            case ADD:
                fprintf(output, "ADD\n");
                break;
            case DIV:
                fprintf(output, "DIV\n");
                break;
            case OUT:
                fprintf(output, "OUT\n");
                break;
            case IN:
                fprintf(output, "IN\n");
                break;
            case SUB:
                fprintf(output, "SUB\n");
                break;
            case MUL:
                fprintf(output, "MUL\n");
                break;
            case HLT:
                fprintf(output, "HLT\n");
                break;
            default:
                errorCode = UNKNOWN_COMMAND;
                break;
        }
    }

    free(commands);

    return errorCode;
}