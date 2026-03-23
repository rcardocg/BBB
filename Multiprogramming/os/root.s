.section .text
.global _start

_start:
b reset

reset:
ldr sp, = 0x00020000
bl main
loop:
b loop