#include "cpu.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        execute(argv[1]);
    }

    return 0;
}