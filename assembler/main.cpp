#include "assembler.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        parseAsm((const char *) argv[1]);
    } 
    else if (argc == 3) {
        parseAsm((const char *) argv[1], (const char *) argv[2]);
    }
    else {
        printf("Assembler takes 2 arguments");
    }

    return 0;
}