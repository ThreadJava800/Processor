#include <stdio.h>
#include <stdlib.h>

int main() {
    system("cd assembler && cmake ../assembler/ && cmake --build . && ./Assembler fibbonachi.txt");
    system("cd cpu && cmake ../cpu/ && cmake --build . && ./Cpu ../assembler/output.txt");
}