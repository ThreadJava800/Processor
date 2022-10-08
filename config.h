enum LanguageDict {
    HLT  = 0,
    PUSH = 1,
    ADD  = 2,
    DIV  = 3,
    OUT  = 4,
    IN   = 5,
    MUL  = 6,
    SUB  = 7,
    DUMP = 8,
    POP  = 9,
};

static const char *language[] = {
    "HLT",
    "PUSH",
    "ADD",
    "DIV",
    "OUT",
    "IN",
    "MUL",
    "SUB",
    "DUMP",
    "POP",
};

const char iMask = 0x10;
const char rMask = 0x20;
const char mMask = 0x40;