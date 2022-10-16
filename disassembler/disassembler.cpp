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

#define DEF_CMD(name, num, code, args)                                          \
    {                                                                            \
    case CMD_##name:                                                              \
        printCommand(output, commands[ip], args, commands, &ip, &commandCount);    \ 
        break;                                                                      \
    }                                                                                \

int translateCommand(FILE *input, FILE *output, int *labels) {
    int errorCode = NO_ERROR;
    int header[3] = {};

    fread(header, sizeof(int), 3, input);

    if (!header) {
        return CANNOT_OPEN_FILE;
    }

    fprintf(output, "%d %d %d\n", header[0], header[1], header[2]);
    char *commands = (char *) calloc(sizeof(int), header[2]);
    if (!commands) {
        return NULL_PTR;
    }
    fread(commands, sizeof(int), header[2], input);

    if (!labels) labels = (int *) calloc(header[2], sizeof(int));
    if (!labels) return NULL_PTR;

    poisonLabels(labels, header[2]);
    errorCode = readLabels(commands, header[2], labels);

    // fprintf(stderr, "%d ", labels[58]);

    int commandCount = 0, ip = 0;

    while (commandCount < header[2]) {
        switch (commands[ip] & 0x1F) {
            
            #include "../cmd.h"

            default:
                errorCode = UNKNOWN_COMMAND;
                break;
        }
    }

    free(commands);

    return errorCode;
}

#undef DEF_CMD

int poisonLabels(int *labels, int labelSize) {
    if (!labels) return NULL_PTR;

    for (int i = 0; i < labelSize; i++) {
        labels[i] = -1;
    }

    return NO_ERROR;
}

#define DEF_CMD(name, num, code, args)                                          \
    {                                                                            \
    case CMD_##name:                                                              \
        if (args == 2) {\
            addLabel(labels, *((int *)(commands + comIp + sizeof(char))));\
            comIp += sizeof(int);\
            comNumber++;\
        }   \
        else if (args == 1) {\
            int commandArgs = commands[comIp] & 0xE0; \
            if ((commandArgs & iMask) && (commandArgs & rMask)) {\
                comIp += 2 * sizeof(int);\
                comNumber += 2;\
            } else {\
                comIp += sizeof(int);\
                comNumber++;\
            }\
        }    \
        break;                                                                      \
    }                                                                                \


int readLabels(char *commands, int comSize, int *labels) {
    if (!commands || !labels) return NULL_PTR;

    int errorCode = NO_ERROR;

    int comIp = 0, comNumber = 0;
    while (comNumber < comSize) {



        switch (commands[comIp] & 0x1F) {
            
            #include "../cmd.h"

            default:
                errorCode = UNKNOWN_COMMAND;
                break;
        }  

        //fprintf(stderr, "%d ", comNumber);
        comNumber++;
        comIp++;
    } 

    return errorCode;
}

#undef DEF_CMD

int addLabel(int *labels, int label) {
    if (!labels) return NULL_PTR;

    if (labels[label] == -1) {
        labels[label] = label;
    }

    return NO_ERROR;
}

int createRegStr(char *str, int value) {
    if (!str) return NULL_PTR;

    *str++ = 'r';
    *str++ = 'a' + value;
    *str++ = 'x';

    str -= 2;

    return NO_ERROR;
}

int printCommand(FILE *output, char maskArgs, char popArgs, char *commands, int *ip, int *commandCount) {
    int errorCode = NO_ERROR;
    char com = commands[*ip] & 0x1F, args = commands[*ip] & 0xE0;

    if (args & iMask) {
        if (args & rMask) {
            if (args & mMask) {
                char regStr[4] = {};
                errorCode = createRegStr(regStr, *((int *)(commands + sizeof(char) + sizeof(int))));
                fprintf(output, "%s [%s+%d]\n", language[com], regStr, *((int *)(commands + sizeof(char))));

                (*commandCount) += 3;
                (*ip) += sizeof(char) + 2 * sizeof(int);
            }
            else {
                char regStr[4] = {};
                errorCode = createRegStr(regStr, *((int *)(commands + sizeof(char) + sizeof(int))));
                fprintf(output, "%s %s+%d\n", language[com], regStr, *((int *)(commands + sizeof(char))));

                (*commandCount) += 3;
                (*ip) += sizeof(char) + 2 * sizeof(int);
            }
        } else if (args & mMask) {
            fprintf(output, "%s [%d]\n", language[com], *((int *)(commands + sizeof(char))));

            (*commandCount) += 2;
            (*ip) += sizeof(char) + sizeof(int);
        } else {
            fprintf(output, "%s %d\n", language[com], *((int *)(commands + sizeof(char))));

            (*commandCount) += 2;
            (*ip) += sizeof(char) + sizeof(int);
        }
    } else if (args & rMask) {
        if (args & mMask) {
            char regStr[4] = {};
            errorCode = createRegStr(regStr, *((int *)(commands + sizeof(char))));
            fprintf(output, "%s [%s]\n", language[com], regStr);

            (*commandCount) += 2;
            (*ip) += sizeof(char) + sizeof(int);
        } else {
            char regStr[4] = {};
            errorCode = createRegStr(regStr, *((int *)(commands + sizeof(char))));
            fprintf(output, "%s %s\n", language[com], regStr);

            (*commandCount) += 2;
            (*ip) += sizeof(char) + sizeof(int);
        }
    } else {
        fprintf(output, "%s\n", language[com]);
        (*commandCount) += 1;
        (*ip)++;
    }

    return NO_ERROR;
}