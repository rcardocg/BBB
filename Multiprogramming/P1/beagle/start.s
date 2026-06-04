.syntax unified
.cpu cortex-a8
.arch armv7-a

.global _start
.extern main

.section .text.start, "ax"
.align 4
_start:
    cpsie i
    bl main
1:
    b 1b
