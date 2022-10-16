#include "disassembler.h"

int main(int argc, char *argv[]) {
    int errorCode = 0;


    if (argc == 2) {
        errorCode = parseMachineCode((const char *) argv[1]);
    } 
    else if (argc == 3) {
        errorCode = parseMachineCode((const char *) argv[1], (const char *) argv[2]);
    }
    else {
        printf("Disassembler takes 2(1) arguments");
    }

    return 0;
}