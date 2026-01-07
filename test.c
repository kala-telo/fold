void _start() {
    register long rax asm("rax") = 1;
    register long rdi asm("rdi") = 0;
    register long rsi asm("rsi") = (long)"hello\n";
    register long rdx asm("rdx") = (long)6;

    asm volatile("syscall"
                 : "=a"(rax)
                 : "a"(rax), "D"(rdi), "S"(rsi), "d"(rdx)
                 : "rcx", "r11", "memory");
}
