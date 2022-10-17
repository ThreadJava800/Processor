# Proccessor
## WHAT IS IT?
It`s a number of commands, with which you can write any program, as this is a turing-full language.

I realised a mini version of proccessor, which contains stack, ram memory and register in it!

By the way, you can check Fibbonachi counter and Quadratic equation solver, written on this language ([check it out](https://github.com/ThreadJava800/Processor/tree/main/examples)).

Here are the commands:
### HLT - closes the program
### PUSH - pushes value to stack (takes >= 1 parameters)
> PUSH number - pushes int number to stack

> PUSH [number] - pushes int value to stack from ram (number is index in ram array)

> PUSH [rax] -  pushes int value to stack from register (there are 10 values that can be kept in register: (rax, rbx...))

> PUSH [rax+number] (or vice versa) - pushes sum of register value and int value to stack

### ADD - adds two values in stack, removes them and pushes the result back

### DIV - divs two values in stack, removes them and pushes the result back

### OUT - removes value from stack and prints in on screen

### IN - takes value from console and pushes it to screen

### MUL - multiplies two values in stack, removes them and pushes the result back

### SUB - subs two values in stack, removes them and pushes the result back

### DUMP - prints all the information about proccessor (stack status, register and ram)

### POP (takes only 1 value)

> POP rax - adds value from stack to register

> POP [number] - adds value from stack to ram (with index == number)

> POP [rax] - adds value from stack to ram with (index == value from register)

### JMP - jumps to label, that must be put in code (takes only 1 value)

> JMP label1 - jumps to place of code where you placed a label1 (labels must end with : and be placed in one line!!)

### CALL - calls a function with label and adds place of return in returnStack

### RET - a keyword that indicates that function is over: return to place from returnStack

### JB - jump if last value from stack less than last by one (takes label)

### JA - jump if last value from stack more than last by one (takes label)

### JAE - jump if last value from stack more or equals than last by one (takes label)

### JBE - jump if last value from stack less or equals than last by one (takes label)

### JE - jump if last value from stack equals than last by one (takes label)

### JNE - jump if last value from stack not equals than last by one (takes label)

### SQRT - takes quare root of value popped from stack and adds this value back (with precision)

### PRNO - prints "NO ROOTS" on screen

### PRINF - prints "INFINITE ROOTS" on screen

### JMPMND [cringe alert] - same as jump, but works only on mondays

If you wanna, you can freely use this language :smiley: