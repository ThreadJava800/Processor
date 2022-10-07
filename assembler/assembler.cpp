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

    error = translateCommand(&commands, outputName);

    freeStrings(&commands);

    return error;
}

int translateCommand(Strings *commands, const char *outputName){ 
    if (!commands) {
        return COMMANDS_NULL;
    }
    if (!commands->array) {
        return COMMANDS_NULL;
    }

    FILE *output = fopen(outputName, "wb");
    if (!output) {
        return INCORRECT_FILE_NAME;
    }

    char *machineCommands = (char *) calloc(2 * commands->size, sizeof(int));
    int commandCount = 0, machineCommandsSize = 0;

    for (int i = 0; i < commands->size; i++) {
        int languageLen = sizeof(language) / sizeof(language[0]);

        for (int j = 0; j < languageLen; j++) {
            char command[MAX_COMMAND_LENGTH] = {};
            int commandLength = 0;

            sscanf(commands->array[i], "%s %n", command, &commandLength);

            if (!strcasecmp(command, language[j])) {
                if (j == PUSH) {
                    int count = 0, commandId = j, arg1 = 0, arg2 = 0;
                    parseLine(commands->array[i], &count, &commandId, &arg1, &arg2);

                    *machineCommands = commandId;
                    machineCommands++;
                    machineCommandsSize++;

                    *machineCommands = arg1;
                    machineCommands += sizeof(int);
                    machineCommandsSize += sizeof(int);

                    commandCount += 2;
                } else {
                    *machineCommands = j;
                    machineCommands++;
                    machineCommandsSize++;

                    commandCount++;
                }
            }
        }
    }

    machineCommands -= machineCommandsSize;

    fwrite(&SIGNATURE, sizeof(SIGNATURE), 1, output);
    fwrite(&VERSION, sizeof(VERSION), 1, output);
    fwrite(&commandCount, sizeof(commandCount), 1, output);
    fwrite(machineCommands, sizeof(machineCommands[0]), machineCommandsSize, output);

    fclose(output);
    free(machineCommands);

    return OK;
}

int parseLine(char *buf, int *count, int *commandId, int *arg1, int *arg2) {
    if (!buf || !count || !commandId || !arg1 || !arg2) return NULL_PTR;

    char command[MAX_COMMAND_LENGTH] = {};

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
        *commandId = iMask + (*commandId);
        *count = 1;
        return OK;
    }

    val = sscanf(buf, "%s %s", command, command);
    if (val == 2) {
        if (sizeof(command) == 5 * 2) {         // utf-8
            if (command[0] == 'r' && command[2] == 'x') {
                *commandId = rMask + (*commandId);
                *count = 1;
                *arg1 = command[1] - 'a';
            }
        } else {
            return INCORRECT_FORMAT;
        }
    }

    val = sscanf(buf, "%s [%d]", command, arg1);
    if (val == 2) {
        *commandId = mMask + iMask + (*commandId);
        *count = 1;
        return OK;
    }

    val = sscanf(buf, "%s [%s]", command, command);
    if (val == 2) {
        if (sizeof(command) == 5 * 2) {         // utf-8
            if (command[0] == 'r' && command[2] == 'x') {
                *commandId = mMask + rMask + (*commandId);
                *count = 1;
                *arg1 = command[1] - 'a';

                return OK;
            }

            return INCORRECT_FORMAT;
        } else {
            return INCORRECT_FORMAT;
        }
    }

    return INCORRECT_FORMAT;
}
