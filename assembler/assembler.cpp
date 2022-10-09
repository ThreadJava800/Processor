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

    char command[MAX_COMMAND_LENGTH] = {};

    char dj[MAX_COMMAND_LENGTH];
    int a, b;
    int tmp = sscanf("PUSH [3]", "%s %d", dj, &b, dj);
    //printf("%d %s", tmp, dj);

    // int val = sscanf(buf, "%s %3s %d", command, command, arg2);
    // printf("%s %d %d", command, arg2, val);
    // if (val == 2) {
    //     if (sizeof(command) == 5 * 2) {         // utf-8
    //         if (command[0] == 'r' && command[2] == 'x') {
    //             *commandId = rMask + iMask + (*commandId);
    //             *count = 2;
    //             *arg1 = command[1] - 'a';

    //             return OK;
    //         }

    //         return INCORRECT_FORMAT;
    //     } else {
    //         return INCORRECT_FORMAT;
    //     }
    // }

    // val = sscanf(buf, "%s [%s %d]", command, command, arg2);
    // printf("%d", val);
    // if (val == 2) {
    //     printf("IN");
    //     if (sizeof(command) == 5 * 2) {         // utf-8
    //         if (command[0] == 'r' && command[2] == 'x') {
    //             *commandId = mMask + rMask + iMask + (*commandId);
    //             *count = 2;
    //             *arg1 = command[1] - 'a';

    //             return OK;
    //         }

    //         return INCORRECT_FORMAT;
    //     } else {
    //         return INCORRECT_FORMAT;
    //     }
    // }

    int val = sscanf(buf, "%s %d", command, arg1);
    if (val == 2) {
        if (hasBrackets(buf) == 1) {
            *commandId = mMask + iMask + (*commandId);
        } else {
            *commandId = iMask + (*commandId);
        }

        printf("%d ", *commandId);

        *count = 1;
        return OK;
    }

    val = sscanf(buf, "%s %s", command, command);
    if (val == 2) {
        if (sizeof(command) == 5 * 2) {         // utf-8
            if (command[0] == 'r' && command[2] == 'x') {
                if (hasBrackets(buf) == 1) {
                    *commandId = mMask + rMask + (*commandId);
                } else {
                    *commandId = rMask + (*commandId);
                }

                *count = 1;
                *arg1 = command[1] - 'a';
            }
        } else {
            return INCORRECT_FORMAT;
        }
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