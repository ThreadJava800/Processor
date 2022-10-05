#include <stdio.h>
#include <malloc.h>

#include "../mlibs/onegin/onegin.h"
#include "../mlibs/stack/stack.h"
#include "../config.h"

const int VERSION = 1;
const int SIGNATURE = 74 * 256 + 67;  // CJ

enum CpuErrorCodes {
    NO_ERROR                =  0,
    INCORRECT_FILE_NAME     = -1,
    CANNOT_OPEN_FILE        = -2,
    INVALID_VERSION         = -3, 
    INVALID_SIGNATURE       = -4,
    DIVISION_BY_ZERO        = -5,
    NO_MEMORY               = -6,
    UNKNOWN_COMMAND         = -7,
};

int verifyFile(FILE *file);

int init(char *fileName);

int parseCommands(FILE *file);

int push(Stack_t *stack, int value);

int add(Stack_t *stack);

int div(Stack_t *stack);

int out(Stack_t *stack);

int in(Stack_t *stack);

int mul(Stack_t *stack);

int sub(Stack_t *stack);

void hlt();