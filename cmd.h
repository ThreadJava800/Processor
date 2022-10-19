#define POP stackPop(&cpu->stack, &errorCode)


#define JUMP_HELPER(sign)   \
    if (POP sign POP) {      \
        jmp(cpu);             \
    } else {                   \
        cpu->ip += sizeof(int); \  
    }                        


DEF_CMD(HLT, 0, {
    exit(0);
}, 0)

DEF_CMD(PUSH, 1, {
    errorCode = push(cpu, args);
}, 1)

DEF_CMD(ADD, 2, {
    errorCode = add(&cpu->stack);
}, 0)

DEF_CMD(DIV, 3, {
    div(&cpu->stack);
}, 0)

DEF_CMD(OUT, 4, {
    errorCode = out(&cpu->stack);
}, 0)

DEF_CMD(IN, 5, {
    errorCode = in(&cpu->stack);
}, 0)

DEF_CMD(MUL, 6, {
    errorCode = mul(&cpu->stack);
}, 0)

DEF_CMD(SUB, 7, {
    errorCode = sub(&cpu->stack);
}, 0)

DEF_CMD(DUMP, 8, {
    DUMP_CPU(cpu, errorCode);
}, 0)

DEF_CMD(POP, 9, {
    errorCode = pop(cpu, args);
}, 1)

DEF_CMD(JMP, 10, {
    errorCode = jmp(cpu);
}, 2)

DEF_CMD(CALL, 11, {
    stackPush(&cpu->callStack, cpu->ip + sizeof(char) + sizeof(int), &errorCode);
    errorCode = jmp(cpu);
}, 2)

DEF_CMD(RET, 12, {
    cpu->ip = stackPop(&cpu->callStack, &errorCode) - 1;
}, 0)

DEF_CMD(JB, 13, {
    JUMP_HELPER(<)
}, 2)

DEF_CMD(JA, 14, {
    JUMP_HELPER(>)
}, 2)

DEF_CMD(JAE, 15, {
    JUMP_HELPER(>=)
}, 2)

DEF_CMD(JBE, 16, {
    JUMP_HELPER(<=)
}, 2)

DEF_CMD(JE, 17, {
    JUMP_HELPER(==)
}, 2)

DEF_CMD(JNE, 18, {
    JUMP_HELPER(!=)
}, 2)

DEF_CMD(SQRT, 19, {
    double val = sqrt(POP);
    stackPush(&cpu->stack, floor(val) / 100, &errorCode);
}, 0)

DEF_CMD(PRINF, 20, {
    fprintf(stderr, "INFINITE ROOTS\n");
}, 0)

DEF_CMD(PRNO, 21, {
    fprintf(stderr, "NO ROOTS\n");
}, 0)

DEF_CMD(JMPMND, 22, {
    tm *calendar; // календарьььь с удобным временем и датами
    time_t timestamp = time(NULL);

    calendar = localtime(&timestamp);
    if (calendar->tm_wday == 1) {
        jmp(cpu);
    }
}, 2)

DEF_CMD(DRAW_PIXEL, 23, {
    SDL_Window* window = NULL;
    window = SDL_CreateWindow
    (
        "Basic circle", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        600,
        600,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );

    SDL_RenderClear( renderer );

    SDL_Rect r;
    r.x = 600;
    r.y = 600;
    r.w = 6;
    r.h = 6;

    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 0 );
    SDL_RenderFillRect( renderer, &r );

    for (int i = 0; i < RAMSIZE; i++) {
        if (cpu->ram[i] != 0) {

            r.x = (i % 600);
            r.y = i / 600;

            SDL_SetRenderDrawColor( renderer, 0, 255, 0, 0 );
            SDL_RenderFillRect( renderer, &r );
        }
    }

    SDL_RenderPresent(renderer);

    SDL_Delay( 5000 );

    SDL_DestroyWindow(window);
    SDL_Quit();
}, 0)
