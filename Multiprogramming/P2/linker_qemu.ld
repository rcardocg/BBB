ENTRY(_start)

MEMORY
{
    P2_RAM (rwx) : ORIGIN = 0x00030000, LENGTH = 64K
}

SECTIONS
{
    . = ORIGIN(P2_RAM);

    .text ALIGN(4) : {
        *(.text.start)
        *(.text*)
    } > P2_RAM

    .rodata ALIGN(4) : {
        *(.rodata*)
    } > P2_RAM

    .data ALIGN(4) : {
        *(.data*)
    } > P2_RAM

    .bss (NOLOAD) : ALIGN(8) {
        *(.bss*)
        *(COMMON)
    } > P2_RAM

    _stack_top = ORIGIN(P2_RAM) + LENGTH(P2_RAM);

    /DISCARD/ : {
        *(.comment*)
        *(.note*)
        *(.eh_frame*)
    }
}