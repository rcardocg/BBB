ENTRY(_start)

MEMORY
{
    P1_RAM (rwx) : ORIGIN = 0x82100000, LENGTH = 64K
}

SECTIONS
{
    . = ORIGIN(P1_RAM);

    .text ALIGN(4) : {
        *(.text.start)
        *(.text*)
    } > P1_RAM

    .rodata ALIGN(4) : {
        *(.rodata*)
    } > P1_RAM

    .data ALIGN(4) : {
        *(.data*)
    } > P1_RAM

    .bss (NOLOAD) : ALIGN(8) {
        *(.bss*)
        *(COMMON)
    } > P1_RAM

    /DISCARD/ : {
        *(.comment*)
        *(.note*)
        *(.eh_frame*)
    }
}
