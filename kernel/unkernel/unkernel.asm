[BITS 16]
section .boot

Boot:
    

    ; NOTE: At boot the boot drive number is stored in DL,
    ;       Preserve it for later 
    mov   [DriveNumber], dl
    
    ; NOTE: Activate A20
    mov   ax, 0x2403
    int   0x15

    mov cx, 4
.FindRsdpOuter:    
    ; Find rsdp
    mov ax, 0x800
    mul cx
    add ax, 0xE000
    mov es, ax
    push cx
    mov cx, 0x7FFF    
    mov di, 0
.FindRsdp:
    mov eax, [es:di]
    cmp eax, 'RSD '
    jne .NotFound
    mov eax, [es:di + 4]
    cmp eax, 'PTR '
    je .Found
.NotFound:
    inc di
    pop ax
    loop .FindRsdp
    pop cx
    loop .FindRsdpOuter
    cli
    hlt
.Found:
    pop cx
    mov si, rsdp
    mov cx, 100
.CopyLoop:
    mov ax, [di]

    mov [si], al
    inc di

    inc si
    loop .CopyLoop

    
    ; NOTE: SETUP VBE
    jmp SetupVbe
    %include "kernel/unkernel/vesa_vbe_setup.asm"
SetupVbe:
    call VesaVbeSetup

    ; NOTE: Load GDT and activate protected mode
    cli
    lgdt  [GDTDesc]
    mov   eax, cr0
    or    eax, 1
    mov   cr0, eax
    jmp   8:After


RsdpSignature: db "RSD PTR "

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
    
    mov dl, [DriveNumber]
    mov edi, unkernel_end
    mov ecx, 4
    call unkernel_read_ata

    mov dl, [DriveNumber]
    mov edi, 0x8000
    mov ecx, 2
    call unkernel_read_ata


    jmp unkernel_end

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

%include "kernel/unkernel/ata/ata.asm"



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
    add ebx, 0xA0000000
    mov esp, ebx
    jmp CoreStart
%include "kernel/unkernel/vesa_vbe_setup_vars.asm"
global rsdp
rsdp:
.Signature: times 8 db 0
.Checksum: db 0
.OemID: times 6 db 0
.Revision: db 0
.Rsdt: dd 0

times 2048-($-$$) db 0
unkernel_end: