#include "assembler.h"

int checkCommands(Strings *commands) {
    if (!commands) {
        return COMMANDS_NULL;
    }

    if (!commands->array) {
        return COMMANDS_NULL;
    }

    if (!commands->buffer) {
        return COMMANDS_NULL;
    }

    return OK;
}

int parseAsm(const char *fileName, const char *outputName) {
    int error = OK;

    if (!fileName) {
        return INCORRECT_FILE_NAME;
    }

    Strings commands = fromFile(fileName);

    error = checkCommands(&commands);
    if (error) return error;

    char *machineCommands = (char *) calloc(2 * commands.size, sizeof(int));
    if (!machineCommands) return NULL_PTR;

    Assembler_t assembler = {
        .commandBytes = 0,
        .commandCount = 0,
        .machineCommands = machineCommands,
        .humanCommands = commands,
    };

    error = translateCommand(&assembler);

    generateMachineFile(&assembler, outputName);

    freeAssembler(&assembler);

    return error;
}

int translateCommand(Assembler_t *assembler, int *labels){ 
    if (!assembler) return NULL_PTR;
    if (!assembler->machineCommands || !assembler->humanCommands.array) return NULL_PTR;

    if (!labels) labels = (int *) calloc(assembler->humanCommands.size, sizeof(int));
    int needSecondCompile = 0;

    for (int i = 0; i < assembler->humanCommands.size; i++) {
        int languageLen = sizeof(language) / sizeof(language[0]);

        for (char j = 0; j < languageLen; j++) {
            char command[MAX_COMMAND_LENGTH] = {};

            sscanf(assembler->humanCommands.array[i], "%s", command);

            if (!strcasecmp(command, language[j])) {
                if (j == PUSH || j == POP) {
                    int count = 0, arg1 = 0, arg2 = 0;
                    char commandId = j;
                    parseLine(assembler->humanCommands.array[i], &count, &commandId, &arg1, &arg2);

                    *assembler->machineCommands = commandId;
                    assembler->machineCommands++;
                    assembler->commandBytes++;

                    memcpy(assembler->machineCommands, &arg1, sizeof(int));
                    assembler->machineCommands += sizeof(int);
                    assembler->commandBytes += sizeof(int);

                    if (count == 2) {
                        memcpy(assembler->machineCommands, &arg2, sizeof(int));
                        assembler->machineCommands += sizeof(int);
                        assembler->commandBytes += sizeof(int);
                    }

                    assembler->commandCount += 2;
                } else if (j == JMP) {
                    int commandIp = 0;
                    int valueAmount = sscanf(assembler->humanCommands.array[i], "%s %d", command, &commandIp);

                    if (valueAmount == 2) {
                        *assembler->machineCommands = j;
                        assembler->machineCommands++;
                        assembler->commandBytes++;

                        if (labels + commandIp) {
                            if (labels[commandIp] != 0) {
                                memcpy(assembler->machineCommands, &labels[commandIp], sizeof(int));
                                assembler->machineCommands += sizeof(int);
                                assembler->commandBytes += sizeof(int);
                            } else {
                                int poison = -1;
                                memcpy(assembler->machineCommands, &poison, sizeof(int));
                                assembler->machineCommands += sizeof(int);
                                assembler->commandBytes += sizeof(int);

                                needSecondCompile = 1;
                            }
                        }

                        assembler->commandCount += 2;
                    }
                } else {
                    *assembler->machineCommands = j;
                    assembler->machineCommands++;
                    assembler->commandBytes++;

                    assembler->commandCount++;
                }
            }
            else { // if (strcasecmp(command, language[j]))
                int label = 0;
                int successfuleInputs = sscanf(assembler->humanCommands.array[i], "%d:", &label);
                if (successfuleInputs == 1) {
                    labels[label] = assembler->commandBytes + 1;
                }
            }
        }
    }

    assembler->machineCommands -= assembler->commandBytes;

    if (needSecondCompile) {
        assembler->commandBytes = 0;
        translateCommand(assembler, labels);
    }

    return OK;
}

int generateMachineFile(Assembler_t *assembler, const char *fileName) {
    if (!assembler) return NULL_PTR;

    FILE *output = fopen(fileName, "wb");
    if (!output) {
        return INCORRECT_FILE_NAME;
    }

    fwrite(&SIGNATURE, sizeof(SIGNATURE), 1, output);
    fwrite(&VERSION, sizeof(VERSION), 1, output);
    fwrite(&assembler->commandCount, sizeof(assembler->commandCount), 1, output);
    fwrite(assembler->machineCommands, sizeof(assembler->machineCommands[0]), assembler->commandBytes, output);

    fclose(output);

    return OK;
}

int parseLine(char *buf, int *count, char *commandId, int *arg1, int *arg2) {
    if (!buf || !count || !commandId || !arg1 || !arg2) return NULL_PTR;

    *count = 0;
    int ram = -1, reg = -1; 

    while (*buf != '\0') {
        if (*buf == 'r' && *(buf + 2) == 'x') {
            *commandId |= rMask;
            reg = *(buf + 1) - 'a';
            (*count)++;

            buf += 3;
        } 
        else if (*buf == '[') {
            *commandId |= mMask;

            buf++;
        }
        else if (isdigit(*buf) || *buf == '-') {
            int value = 0, numberCount = 0;
            sscanf(buf, "%d%n", &value, &numberCount);
            
            *commandId |= iMask;
            *arg1 = ram = value;
            (*count)++;

            buf += numberCount;
        }
        else {
            buf++;
        }
    }

    if (*count == 2) {
        *arg2 = reg;
    }
    if (*count == 1 && reg != -1) {
        *arg1 = reg;
    }

    return INCORRECT_FORMAT;
}

int hasBrackets(char *command) {
    if (!command) return NULL_PTR;
    int metBrackets = 0;

    while (*command != '\0') {
        if (*command == '[' || *command == ']') {
            metBrackets++;
        }

        command++;
    }

    return metBrackets == 2;
}

int freeAssembler(Assembler_t *assembler) {
    if (!assembler) return NULL_PTR;

    free(assembler->machineCommands);
    freeStrings(&assembler->humanCommands);

    return OK;
}