OUTPUT_FORMAT(binary)
ENTRY(OS_Start)
SECTIONS {
    . = 0x7C00;
    
    .text :
    {
        *(.boot)
        *(.text)
    }
    .rodata : 
    {
        *(.rodata)
    }
    .data : 
    {
        *(.data)
    }
    . = 0x1000000+0x7C00;
    .bss : 
    {
        *(.bss)    
    }
}

