OUTPUT_FORMAT(binary)
ENTRY(kmain)
SECTIONS {
    . = 0x7C00;
    
    .boot :
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
    . = 0x1000000 + 0x7C00;
    .bss : 
    {
        *(.bss)
    }
    .end :
    {
        *(.end)
    }
    
}

