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

    int *machineCommands = (int *) calloc(2 * commands->size, sizeof(int));
    int commandCount = 0;

    for (int i = 0; i < commands->size; i++) {
        int languageLen = sizeof(language) / sizeof(language[0]);

        for (int j = 0; j < languageLen; j++) {
            char command[MAX_COMMAND_LENGTH] = {};
            int commandLength = 0;
            sscanf(commands->array[i], "%s %n", command, &commandLength);

            if (!strcasecmp(command, language[j])) {
                if (j == PUSH) {
                    int value = atoi(commands->array[i] + commandLength);
                    *machineCommands = j;
                    machineCommands++;

                    *machineCommands = value;
                    machineCommands++;

                    commandCount += 2;
                } else {
                    *machineCommands = j;
                    machineCommands++;

                    commandCount++;
                }
            }
        }
    }

    machineCommands -= commandCount;

    fwrite(&SIGNATURE, sizeof(SIGNATURE), 1, output);
    fwrite(&VERSION, sizeof(VERSION), 1, output);
    fwrite(&commandCount, sizeof(commandCount), 1, output);
    fwrite(machineCommands, sizeof(machineCommands[0]), commandCount, output);

    fclose(output);
    free(machineCommands);

    return OK;
}
