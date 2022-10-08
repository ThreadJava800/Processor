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
    stackCtor(&stack, 1);
    int reg[REGSIZE] = {}; 
    int ram[RAMSIZE] = {};

    cpu->commands = commands;
    cpu->reg = reg;
    cpu->ram = ram;
    cpu->stack = stack;


    for (size_t ip = 0; ip < count * sizeof(int); ip++) {
        int com = commands[ip] & 0xF, args = commands[ip] >> 4;
        switch(com) {
            case PUSH:
                errorCode = push(cpu, *(commands + ip + sizeof(char)), args);
                if (args >= rMask + mMask) {
                    ip += 2 * sizeof(int);
                } else  {
                    ip += sizeof(int);
                }
                break;
            case ADD:
                errorCode = add(&cpu->stack);
                break;
            case DIV:
                errorCode = div(&cpu->stack);
                break;
            case OUT:
                errorCode = out(&cpu->stack);
                break;
            case IN:
                errorCode = in(&cpu->stack);
                break;
            case SUB:
                errorCode = sub(&cpu->stack);
                break;
            case MUL:
                errorCode = mul(&cpu->stack);
                break;
            case POP:
                if (args != 0) {
                    errorCode = pop(cpu, args, *(commands + ip + sizeof(char)));
                    ip += sizeof(int);
                } else {
                    errorCode = pop(cpu, args);
                }
                break;
            case HLT:
                hlt();
                break;
            case DUMP:
                DUMP_CPU(cpu, errorCode);
                break;
            default:
                errorCode = UNKNOWN_COMMAND;
                break;
        }
    }

    free(commands);
    stackDtor(&stack, &errorCode);

    return errorCode;
}

int freeCpu(Cpu_t *cpu) {
    if (!cpu) return CPU_NULL;
    if (cpu->commands) free(cpu->commands);
    if (cpu->reg) free(cpu->reg);
    if (cpu->ram) free(cpu->ram);
    stackDtor(&cpu->stack);

    return NO_ERROR;
}

int push(Cpu_t *cpu, int value, int mode) {
    int pushVal = value, errorCode = NO_ERROR;
    //printf("%d ", rMask + mMask);

    if (mode * 16 == rMask) {
        pushVal = cpu->reg[value];
    }

    if (mode * 16 == iMask + mMask) {
        pushVal = cpu->ram[value];
    }

    if (mode * 16 == rMask + mMask) {
        //printf("%d ", cpu->ram[cpu->reg[value]]);
        pushVal = cpu->ram[cpu->reg[value]];
    }

    stackPush(&cpu->stack, pushVal, &errorCode);
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

int pop(Cpu_t *cpu, int mode, int arg) {
    int errorCode = NO_ERROR;

    int value = stackPop(&cpu->stack, &errorCode);
    if (errorCode) return errorCode;

    if (mode * 16 == mMask + iMask) {
        cpu->ram[arg] = value;
    }

    if (mode * 16 == mMask + rMask) {
        cpu->ram[cpu->reg[arg]] = value;
    }

    if (mode * 16 == rMask) {
        cpu->reg[arg] = value;
    }

    return UNKNOWN_COMMAND;
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