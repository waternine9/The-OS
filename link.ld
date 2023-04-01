OUTPUT_FORMAT(binary)
ENTRY(OS_Start)
SECTIONS {
  . = 0x7C00;

  .text :
  {
    *(.boot)
    *(.text)
  }
  .data : 
  {
    *(.data)
  }
  .rodata : 
  {
    *(.rodata)
  }
  . = 0x100000;
  .bss : 
  {
    *(.bss)    
  }
}

