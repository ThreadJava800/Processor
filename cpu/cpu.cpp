#include "cpu.h"

int checkCpu(Cpu_t *cpu) {
    if (!cpu) return CPU_NULL;
    if (!cpu->commands) return COMMANDS_NULL;
    if (!cpu->ram) return RAM_NULL;
    if (!cpu->reg) return REG_NULL;

    return NO_ERROR;
}

int verifyFile(FILE *file) {
    int errorCode = NO_ERROR;

    if (!file) {
        return CANNOT_OPEN_FILE;
    }

    int commands[2] = {};
    fread(commands, sizeof(int), 2, file);

    if (commands[0] == SIGNATURE) {
        if (commands[1] != VERSION) {
            errorCode = INVALID_VERSION;
        }
    } else {
        errorCode = INVALID_SIGNATURE;
    }

    return errorCode;
}

int execute(char *fileName) {
    int errorCode = NO_ERROR;

    if (!fileName) {
        return INCORRECT_FILE_NAME;
    }

    FILE *instructions = fopen(fileName, "rb");

    if (!instructions) {
        return CANNOT_OPEN_FILE;
    }

    errorCode = verifyFile(instructions);
    if (errorCode) {
        return errorCode;
    }

    Cpu_t cpu = { };
    parseCommands(instructions, &cpu);

    freeCpu(&cpu);

    return errorCode;
}

#define DEF_CMD(name, num, code, args)    \
    {                                      \
    case CMD_##name:                        \
            code                             \
            break;                            \
    }                                          \

int parseCommands(FILE *file, Cpu_t *cpu) {
    size_t count = 0;
    int errorCode = NO_ERROR;

    fread(&count, sizeof(int), 1, file);

    char *commands = (char *) calloc(count + 1, sizeof(int));
    if (!commands) {
        errorCode = NO_MEMORY;
    }
    fread(commands, sizeof(int), count, file);

    Stack_t stack = {};
    Stack_t callStack = {};
    stackCtor(&stack, 1);
    stackCtor(&callStack, 1);
    int reg[REGSIZE] = {}; 
    int ram[RAMSIZE] = {};

    cpu->commands = commands;
    cpu->reg = reg;
    cpu->ram = ram;
    cpu->stack = stack;
    cpu->callStack = callStack;


    while(cpu->commands[cpu->ip] != 0) {
        char com = commands[cpu->ip] & 0x1F, args = commands[cpu->ip] >> 5;
        switch(com) {

            #include "../cmd.h"

            default:
                errorCode = UNKNOWN_COMMAND;
                break;
        }
        cpu->ip++;
    }

    return errorCode;
}

#undef DEF_CMD

int freeCpu(Cpu_t *cpu) {
    if (!cpu) return CPU_NULL;

    // cpu->commands -= cpu->ip;
    // if (cpu->commands) free(cpu->commands);
    stackDtor(&cpu->stack);

    return NO_ERROR;
}

int push(Cpu_t *cpu, char mode) {
    int pushVal = -1, errorCode = NO_ERROR;
    int value1 = *(int *)(cpu->commands + cpu->ip + sizeof(char)), value2 = *(int *)(cpu->commands + cpu->ip + sizeof(char) + sizeof(int));

    mode <<= 5;

    if (mode & iMask) {
        if (mode & rMask) {
            if (mode & mMask) {
                pushVal = cpu->ram[value1 + cpu->reg[value2]];
                printf("test %d ", pushVal);
                cpu->ip += 2 * sizeof(int);
            }
            else {
                pushVal = value1 + cpu->reg[value2];
                cpu->ip += 2 * sizeof(int);
            }
        }
        else if (mode & mMask) {
            pushVal = cpu->ram[value1];
                cpu->ip += sizeof(int);
        }
        else {
            pushVal = value1;
            cpu->ip += sizeof(int);
        }
    }
    else if (mode & rMask) {
        if (mode & mMask) {
            pushVal = cpu->ram[cpu->reg[value1]];
            cpu->ip += sizeof(int);
        }
        else {
            pushVal = cpu->reg[value1];
            cpu->ip += sizeof(int);
        }
    } 
    else {
        errorCode = UNKNOWN_COMMAND;
    }

    if (!errorCode) stackPush(&cpu->stack, pushVal, &errorCode);

    return errorCode;
}

int add(Stack_t *stack) {
    int errorCode = NO_ERROR;

    int sum = stackPop(stack, &errorCode) + stackPop(stack, &errorCode);
    stackPush(stack, sum, &errorCode);

    return errorCode;
}

int div(Stack_t *stack) {
    int errorCode = NO_ERROR;
    int val1 = stackPop(stack, &errorCode), val2 = stackPop(stack, &errorCode);

    if (val2) {
        int div = val1 / val2;
        stackPush(stack, div, &errorCode);
    } else {
        errorCode = DIVISION_BY_ZERO;
    }

    return errorCode;
}

int out(Stack_t *stack) {
    int errorCode = NO_ERROR;

    printf("\nOUTPUT: %d\n", stackPop(stack, &errorCode));

    return errorCode;
}

int in(Stack_t *stack) {
    int val = 0, errorCode = NO_ERROR;

    scanf("%d", &val);
    stackPush(stack, val, &errorCode);

    return errorCode;
}

int mul(Stack_t *stack) {
    int errorCode = NO_ERROR, mul = stackPop(stack, &errorCode) * stackPop(stack, &errorCode);;

    stackPush(stack, mul, &errorCode);

    return errorCode;
}

int sub(Stack_t *stack) {
    int errorCode = NO_ERROR, sub = stackPop(stack, &errorCode) - stackPop(stack, &errorCode);;

    stackPush(stack, sub, &errorCode);

    return errorCode;
}

int pop(Cpu_t *cpu, char mode) {
    int errorCode = NO_ERROR;
    int value1 = *(int *)(cpu->commands + cpu->ip + sizeof(char)), value2 = *(int *)(cpu->commands + cpu->ip + sizeof(char) + sizeof(int));

    mode <<= 5;
    int popVal = stackPop(&cpu->stack, &errorCode);

    if (mode & mMask) {
        if (mode & iMask) {
            if (mode & rMask) {
                cpu->ram[value1 + cpu->reg[value2]] = popVal;
                cpu->ip += 2 * sizeof(int);
            }
            else {
                cpu->ram[value1] = popVal;
                cpu->ip += sizeof(int);
            }
        }
        else {
            cpu->ram[cpu->reg[value1]] = popVal;
            cpu->ip += sizeof(int);
        }
    }
    else if (mode & rMask) {
        cpu->reg[value1] = popVal;
        cpu->ip += sizeof(int);
    }
    else {
        errorCode = UNKNOWN_COMMAND;
    }

    return errorCode;
}

int jmp(Cpu_t *cpu) {
    if (!cpu) return COMMANDS_NULL;
    if (!cpu->commands) return COMMANDS_NULL;

    int position = *(int *)(cpu->commands + cpu->ip + sizeof(char));
    cpu->ip = position - 2;

    return NO_ERROR;
}

void dump(Cpu_t *cpu, int errorCode, const char *file, const char *function, int line) {
    if (cpu) {
        dumpStack(&cpu->stack, stderr, errorCode, printElemT, function, file, line);
        mprintf(stderr, "REGISTER:\n");
        for (int i = 0; i < REGSIZE; i++) {
            mprintf(stderr, "\t[%d]: %d\n", i, cpu->reg[i]);
        }

        mprintf(stderr, "RAM:\n");
        for (int i = 0; i < 4; i++) {
            mprintf(stderr, "\t[%d]: %d\n", i, cpu->ram[i]);
        }
        mprintf(stderr, "...\n");
        for (int i = RAMSIZE - 4; i < RAMSIZE; i++) {
            mprintf(stderr, "\t[%d]: %d\n", i, cpu->ram[i]);
        }
    } else {
        mprintf(stderr, "%s", "CPU[0x00000000] - NULLPTR");
    }
}

void hlt() {
    exit(0);
}