SECTIONS
{
    . = 0x80000;
    __start = .;
    
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)
    }
    . = ALIGN(4096);
    
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096);

    .data :
    {
        *(.data)
    }
    . = ALIGN(4096);

    __bss_start = .;
    .bss :
    {
        *(.bss)
    }
    . = ALIGN(4096);
    __bss_end = .;
}