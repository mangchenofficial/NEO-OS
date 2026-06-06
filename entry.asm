[BITS 32]
section .text
global _entry
extern kernel_main

_entry:
    call kernel_main
.halt:
    hlt
    jmp .halt
