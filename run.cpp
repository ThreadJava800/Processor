#include <stdio.h>
#include <stdlib.h>

int main() {
    system("cd assembler && cmake ../assembler/ && cmake --build . && ./Assembler ../examples/quadr.txt");
    system("cd cpu && cmake ../cpu/ && cmake --build . && ./Cpu ../assembler/output.txt");
    system("cd disassembler && cmake ../disassembler/ && cmake --build . && ./Disasm ../assembler/output.txt");
}