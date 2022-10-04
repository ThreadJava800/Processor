#include "assembler.h"

int main(int argc, char *argv[]) {
    int errorCode = 0;

    if (argc == 2) {
        errorCode = parseAsm((const char *) argv[1]);
    } 
    else if (argc == 3) {
        errorCode = parseAsm((const char *) argv[1], (const char *) argv[2]);
    }
    else {
        printf("Assembler takes 2 arguments");
    }

    return 0;
}