[BITS 16]
section .boot

Boot:
    

    ; NOTE: At boot the boot drive number is stored in DL,
    ;       Preserve it for later 
    mov   [DriveNumber], dl
    

    ; NOTE: Load GDT and activate protected mode
    cli
    lgdt  [GDTDesc]
    mov   eax, cr0
    or    eax, 1
    mov   cr0, eax
    jmp   8:After

global DriveNumber
DriveNumber: db 0
[BITS 32]

After:
    ; NOTE: Setup segments.
    mov   ax, 16
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    
    mov edi, unkernel_end
    mov ecx, 4
.ReadSector:
    mov dl, [DriveNumber]
    push edi
    push ecx
    call kernel_read_ata
    pop ecx
    pop edi
    add edi, 512
    inc ecx
    cmp edi, OsEnd - 512
    jl .ReadSector

    mov dl, [DriveNumber]
    mov edi, 0x8000
    mov ecx, 2
    call kernel_read_ata
    cli
    hlt

    jmp 8:unkernel_end

GDTStart:
    dq 0 
GDTCode:
    dw 0xFFFF     ; Limit
    dw 0x0000     ; Base
    db 0x00       ; Base
    db 0b10011010 ; Access
    db 0b11001111 ; Flags + Limit
    db 0x00       ; Base
GDTData:
    dw 0xFFFF     ; Limit
    dw 0x0000     ; Base
    db 0x00       ; Base
    db 0b10010010 ; Access
    db 0b11001111 ; Flags + Limit
    db 0x00       ; Base
GDTEnd:

GDTDesc:
    .GDTSize dw GDTEnd - GDTStart ; GDT size 
    .GDTAddr dd GDTStart          ; GDT address

%include "src/drivers/ide/ata.asm"



times 510-($-$$) db 0
dw 0xAA55

times 512 db 0
extern CoreStart
[BITS 16]
StartupCore: ; Startup code for each core
    cli
    lgdt  [.GDTDesc]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp   8:.After
.GDTStart:
    dq 0 
.GDTCode:
    dw 0xFFFF     ; Limit
    dw 0x0000     ; Base
    db 0x00       ; Base
    db 0b10011010 ; Access
    db 0b11001111 ; Flags + Limit
    db 0x00       ; Base
.GDTData:
    dw 0xFFFF     ; Limit
    dw 0x0000     ; Base
    db 0x00       ; Base
    db 0b10010010 ; Access
    db 0b11001111 ; Flags + Limit
    db 0x00       ; Base
.GDTEnd:

.GDTDesc:
    .GDTSize dw .GDTEnd - .GDTStart ; GDT size 
    .GDTAddr dd .GDTStart          ; GDT address

[BITS 32]
.After:
    mov   ax, 16
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    
    ; Set up stack
    mov     eax, 1
    cpuid
    shr    ebx, 24
    mov    edi, ebx

    shl ebx, 15
    add ebx, 0x2000000
    mov esp, ebx
    jmp CoreStart
global rsdp

rsdp:
.Signature times 8 db 0
.Checksum db 0
.OEMID times 6 db 0
.Revision db 0
.Rsdt dd 0

times 2048 - ($ - $$) db 0

unkernel_end:

extern kmain
cli
call kmain
cli
hlt



section .text

section .end
OsEnd: