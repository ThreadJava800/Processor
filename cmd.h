DEF_CMD(HLT, 0, {
    exit(0);
})

DEF_CMD(PUSH, 1, {
    //printf("%d\n", cpu->ip);
    errorCode = push(cpu, args);
})

DEF_CMD(ADD, 2, {
    errorCode = add(&cpu->stack);
})

DEF_CMD(DIV, 3, {
    div(&cpu->stack);
})

DEF_CMD(OUT, 4, {
    errorCode = out(&cpu->stack);
})

DEF_CMD(IN, 5, {
    errorCode = in(&cpu->stack);
})

DEF_CMD(MUL, 6, {
    errorCode = mul(&cpu->stack);
})

DEF_CMD(SUB, 7, {
    errorCode = sub(&cpu->stack);
})

DEF_CMD(DUMP, 8, {
    DUMP_CPU(cpu, errorCode);
})

DEF_CMD(POP, 9, {
    errorCode = pop(cpu, args);
})

DEF_CMD(JMP, 10, {
    errorCode = jmp(cpu);
})

DEF_CMD(CALL, 11, {
    stackPush(&cpu->callStack, cpu->ip + sizeof(char) + sizeof(int), &errorCode);
    errorCode = jmp(cpu);
    //printf("%d ", cpu->ip);
})

DEF_CMD(RET, 12, {\
    cpu->ip = stackPop(&cpu->callStack, &errorCode) - 1;
})
