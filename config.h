#define DEF_CMD(name, num, code, args) CMD_##name = num, \
      
enum LanguageDict {
    #include "cmd.h"
}; 

#undef DEF_CMD


#define DEF_CMD(name, num, code, args) #name, \

static const char *language[] = {
    #include "cmd.h"
};

#undef DEF_CMD

const char iMask = 0x20;
const char rMask = 0x40;
const char mMask = 0x80;