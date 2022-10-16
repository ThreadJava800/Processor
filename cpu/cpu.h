#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "../mlibs/onegin/onegin.h"
#include "../mlibs/stack/stack.h"
#include "../config.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int VERSION = 1;
const int SIGNATURE = 74 * 256 + 67;  // CJ

const int REGSIZE = 10;
const int RAMSIZE = 100;

static SDL_Window* window = NULL;
static SDL_Surface* screenSurface = NULL;

struct Cpu_t {
    char *commands = nullptr;
    Stack_t stack = {};
    Stack_t callStack = {};
    int *ram = nullptr;
    int *reg = nullptr;
    int ip = 0;
};

enum CpuErrorCodes {
    NO_ERROR                =  0,
    INCORRECT_FILE_NAME     = -1,
    CANNOT_OPEN_FILE        = -2,
    INVALID_VERSION         = -3, 
    INVALID_SIGNATURE       = -4,
    DIVISION_BY_ZERO        = -5,
    NO_MEMORY               = -6,
    UNKNOWN_COMMAND         = -7,
    CPU_NULL                = -8,
    STCK_NULL               = -9,
    RAM_NULL                = -10,
    REG_NULL                = -11,
    COMMANDS_NULL           = -12,
};

int checkCpu(Cpu_t *cpu);

int verifyFile(FILE *file);

int execute(char *fileName);

int parseCommands(FILE *file, Cpu_t *cpu);

int freeCpu(Cpu_t *cpu);

int push(Cpu_t *cpu,  char mode);

int add(Stack_t *stack);

int div(Stack_t *stack);

int out(Stack_t *stack);

int in(Stack_t *stack);

int mul(Stack_t *stack);

int sub(Stack_t *stack);

int pop(Cpu_t *cpu, char mode);

int jmp(Cpu_t *cpu);

void dump(Cpu_t *cpu, int errorCode, const char *file, const char *function, int line);

void hlt();

#define DUMP_CPU(cpu, errorCode) {                                             \
    dump(cpu, errorCode, __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__);         \
};                                                                               