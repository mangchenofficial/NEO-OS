ENTRY(_entry)

SECTIONS
{
    . = 0x100000;

    .text : {
        entry.o(.text)
        kernel.o(.text)
        *(.text)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }

    PROVIDE(_kernel_end = .);
}
