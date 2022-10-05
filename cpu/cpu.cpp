#include "cpu.h"

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

int init(char *fileName) {
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

    parseCommands(instructions);

    return errorCode;
}

int parseCommands(FILE *file) {
    int count = 0, errorCode = NO_ERROR;

    fread(&count, sizeof(int), 1, file);

    int *commands = (int *) calloc(count + 1, sizeof(int));
    if (!commands) {
        errorCode = NO_MEMORY;
    }
    fread(commands, sizeof(int), count, file);

    Stack_t stack = {};
    stackCtor(&stack, 1);

    for (int ip = 0; ip < count; ip++) {
        switch(commands[ip]) {
            case PUSH:
                push(&stack, commands[++ip]);
                break;
            case ADD:
                add(&stack);
                break;
            case DIV:
                div(&stack);
                break;
            case OUT:
                out(&stack);
                break;
            case IN:
                in(&stack);
                break;
            case SUB:
                sub(&stack);
                break;
            case MUL:
                mul(&stack);
                break;
            case HLT:
                hlt();
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

void hlt() {
    exit(0);
}