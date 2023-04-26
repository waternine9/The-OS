OUTPUT_FORMAT(binary)
ENTRY(OS_Start)
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
    . = 0x7C00 + 0x100000;
    .bss : 
    {
        *(.bss)
    }
    .resources :
    {
        *(.resources)
    }
}

