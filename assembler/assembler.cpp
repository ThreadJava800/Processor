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

int execute(const char *fileName, const char *outputName) {
    int error = OK;

    if (!fileName) {
        return INCORRECT_FILE_NAME;
    }

    Strings commands = fromFile(fileName);

    error = checkCommands(&commands);
    if (error) return error;

    char *machineCommands = (char *) calloc(3 * commands.size, sizeof(int));
    Label_t *labels = (Label_t *) calloc(commands.size, sizeof(Label_t));
    if (!machineCommands || !labels) return NULL_PTR;

    Assembler_t assembler = {
        .commandBytes = 0,
        .commandCount = 0,
        .machineCommands = machineCommands,
        .humanCommands = commands,
    };

    int labelCount = 0;
    error = compile(&assembler, labels, &labelCount);

    generateMachineFile(&assembler, outputName);

    freeMemory(&assembler, labels);

    return error;
}

#define DEF_CMD(name, num, code, args)                                           \
    if (!strcasecmp(command, #name)) {                                            \
        if (args == 1) {                                                           \
                                                                                    \
            parsePushPop(assembler, i, num);                                         \
                                                                                      \
        } else if (args == 2) {                                                        \
                                                                                        \
            parseJumpCall(assembler, i, num, &needSecondCompile, labels, labelCount);    \
                                                                                          \
        } else {                                                                           \
            *assembler->machineCommands = num;                                              \
             assembler->machineCommands++;                                                   \
             assembler->commandBytes++;                                                       \
                                                                                               \
             assembler->commandCount++;                                                         \
        }                                                                                        \
                                                                                                  \
    } else                                                                                                                                                                                                                                                                                                                                                                                                                              

int compile(Assembler_t *assembler, Label_t *labels, int *labelCount, int compileCount){ 
    if (!assembler || !labelCount || !labels) return NULL_PTR;
    if (!assembler->machineCommands || !assembler->humanCommands.array) return NULL_PTR;

    int needSecondCompile = 0;

    for (int i = 0; i < assembler->humanCommands.size; i++) {
        char command[MAX_COMMAND_LENGTH] = {};
        sscanf(assembler->humanCommands.array[i], "%s", command);

        #include "../cmd.h"

        /* else */ { 
            char label[MAX_LABEL_LEN] = {};
            int labelLen = 0;                                                               
            int successfuleInputs = sscanf(assembler->humanCommands.array[i], "%s%n", label, &labelLen);  

            if (successfuleInputs == 1) {
                int metLabel = 0;
                for (int i = 0; i < *labelCount; i++) {
                    if (!strcasecmp(labels[i].labelTxt, label)) {
                        metLabel = 1;
                    }
                }

                if (!metLabel) {
                    label[labelLen - 1] = '\0';         //removing :
                    //printf("%s\n", label);
                    strcpy(labels[*labelCount].labelTxt, label);
                    labels[*labelCount].gotoIp = assembler->commandBytes + 1;
                    (*labelCount)++;
                }                                          
            }  
        }  
    }

    assembler->machineCommands -= assembler->commandBytes;
    compileCount++;

    if (compileCount > MAX_COMPILE_ATTEMPTS) {
        return TOO_MANY_COMPILES;
    }

    if (needSecondCompile) {
        assembler->commandBytes = 0;
        assembler->commandCount = 0;
        compile(assembler, labels, labelCount, compileCount);
    }

    return OK;
}

#undef DEF_CMD

int generateMachineFile(Assembler_t *assembler, const char *fileName) {
    if (!assembler) return NULL_PTR;

    FILE *output = fopen(fileName, "wb");
    if (!output) {
        return INCORRECT_FILE_NAME;
    }

    fwrite(&SIGNATURE, sizeof(SIGNATURE), 1, output);
    fwrite(&VERSION,   sizeof(VERSION),   1, output);
    fwrite(&assembler->commandCount,      sizeof(assembler->commandCount),       1,                       output);
    fwrite( assembler->machineCommands,   sizeof(assembler->machineCommands[0]), assembler->commandBytes, output);

    fclose(output);

    return OK;
}

int parsePushPop(Assembler_t *assembler, int ip, char commandId) {
    if (!assembler) return NULL_PTR;

    char *buf = assembler->humanCommands.array[ip];
    int ram = -1, reg = -1, count = 0; 
    int arg1 = 0, arg2 = 0;

    while (*buf != '\0') {
        if (*buf == 'r' && *(buf + 2) == 'x') {
            commandId |= rMask;
            reg = *(buf + 1) - 'a';
            count++;

            buf += 3;
        } 
        else if (*buf == '[') {
            commandId |= mMask;

            buf++;
        }
        else if (isdigit(*buf) || *buf == '-') {
            int value = 0, numberCount = 0;
            sscanf(buf, "%d%n", &value, &numberCount);
            
            commandId |= iMask;
            arg1 = ram = value;
            count++;

            buf += numberCount;
        }
        else {
            buf++;
        }
    }

    if ((commandId & 0x1F) == CMD_POP)  {
        if ((commandId & iMask) && !(commandId & mMask)) {
            return INCORRECT_FORMAT;
        }
    }

    *assembler->machineCommands = commandId;
     assembler->machineCommands++;
     assembler->commandBytes++;

    if (count == 2) {
        arg2 = reg;
    }
    if (count == 1 && reg != -1) {
        arg1 = reg;
    }

    memcpy(assembler->machineCommands, &arg1, sizeof(int));
    assembler->machineCommands += sizeof(int);
    assembler->commandBytes    += sizeof(int);

    assembler->commandCount += 2;

    if (count == 2) {
        memcpy(assembler->machineCommands, &arg2, sizeof(int));
        assembler->machineCommands += sizeof(int);
        assembler->commandBytes += sizeof(int);
        assembler->commandCount++;
    }

    return INCORRECT_FORMAT;
}

int parseJumpCall(Assembler_t *assembler, int ip, char commandType, int *needSecondCompile, Label_t *labels, int *labelCount) {
    if (!assembler || !needSecondCompile || !labels || !labelCount) return NULL_PTR;

    char buf[MAX_COMMAND_LENGTH] = {};
    char test[MAX_COMMAND_LENGTH] = {};
    int valueAmount = sscanf(assembler->humanCommands.array[ip], "%s%s", test, buf);

    if (valueAmount == 2) {
        *assembler->machineCommands = commandType;
        assembler->machineCommands++;
        assembler->commandBytes++;

        int metLabel = 0;
        for (int i = 0; i < *labelCount; i++) {
            if (!strcmp(labels[i].labelTxt, buf)) {
                memcpy(assembler->machineCommands, &(labels[i].gotoIp), sizeof(int));
                assembler->machineCommands += sizeof(int);
                assembler->commandBytes += sizeof(int);

                metLabel = 1;
                break;
            }
        }

         if (!metLabel) {
                int poison = -1;
                memcpy(assembler->machineCommands, &poison, sizeof(int));
                assembler->machineCommands += sizeof(int);
                assembler->commandBytes += sizeof(int);

                *needSecondCompile = 1;
            }

        assembler->commandCount += 2;
    }

    return OK;
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

int freeMemory(Assembler_t *assembler, Label_t *labels) {
    if (!assembler || !labels) return NULL_PTR;

    free(labels);
    free(assembler->machineCommands);
    freeStrings(&assembler->humanCommands);

    return OK;
}