#include "cpu.h"

int checkCpu(Cpu_t *cpu) {
    if (!cpu) return CPU_NULL;
    if (!cpu->commands) COMMANDS_NULL;
    if (!cpu->ram) RAM_NULL;
    if (!cpu->reg) REG_NULL;

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
    int count = 0, errorCode = NO_ERROR;

    fread(&count, sizeof(int), 1, file);

    int *commands = (int *) calloc(count + 1, sizeof(int));
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


    for (int ip = 0; ip < count; ip++) {
        switch(commands[ip]) {
            case PUSH:
                errorCode = push(&cpu->stack, commands[++ip]);
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

int push(Stack_t *stack, int value) {
    int errorCode = NO_ERROR;

    stackPush(stack, value, &errorCode);

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

void dump(Cpu_t *cpu, int errorCode, const char *file, const char *function, int line) {
    if (cpu) {
        dumpStack(&cpu->stack, stderr, errorCode, printElemT, function, file, line);
    } else {
        mprintf(stderr, "%s", "CPU[0x00000000] - NULLPTR");
    }
}

void hlt() {
    exit(0);
}