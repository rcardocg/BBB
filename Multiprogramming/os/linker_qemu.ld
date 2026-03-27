ENTRY(_start)

MEMORY
{
    OS_RAM (rwx) : ORIGIN = 0x00010000, LENGTH = 64K
}

SECTIONS
{
    . = ORIGIN(OS_RAM);

    .vectors ALIGN(32) : {
        KEEP(*(.vectors))
    } > OS_RAM

    .text ALIGN(4) : {
        *(.text.boot)
        *(.text*)
    } > OS_RAM

    .rodata ALIGN(4) : {
        *(.rodata*)
    } > OS_RAM

    .data ALIGN(4) : {
        *(.data*)
    } > OS_RAM

    .bss (NOLOAD) : ALIGN(8) {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } > OS_RAM

    /DISCARD/ : {
        *(.comment*)
        *(.note*)
        *(.eh_frame*)
    }
}

__irq_stack_top = 0x0001F000;
__svc_stack_top = 0x00020000;