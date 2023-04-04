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
    . = 0x10000+0x7C00;
    .afterdata :
    {
        *(.afterdata)
    }
    . = 0x900000+0x10000+0x7C00;
    .bss : 
    {
        *(.bss)    
    }
}

