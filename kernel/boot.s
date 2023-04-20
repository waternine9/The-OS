[BITS 16]
section .boot

extern ReadATASector

Boot:
    

    ; NOTE: At boot the boot drive number is stored in DL,
    ;       Preserve it for later 
    mov   [DriveNumber], dl
    
    ; NOTE: Activate A20
    mov   ax, 0x2403
    int   0x15
    
    ; NOTE: SETUP VBE
    jmp SetupVbe
    
    %include "kernel/vesa_vbe_setup.s"

SetupVbe:
    call VesaVbeSetup
    mov eax, [VbeModeInfo.PhysBasePtr]
    mov [PhysBasePtr], eax
    ; NOTE: Load GDT and activate protected mode
    cli
    lgdt  [GDTDesc]
    mov   eax, cr0
    or    eax, 1
    mov   cr0, eax
    jmp   8:After

[BITS 32]

After:
    ; NOTE: Setup segments.
    mov   ax, 16
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    mov edi, 0x7E00
    mov ecx, 1

    

LoadSectors:
    push edi
    push ecx
    call LoadATASectorASM
    pop ecx
    pop edi
    
    add edi, 512
    inc ecx
    cmp ecx, 100000

    jl LoadSectors

    mov eax, [PhysBasePtr]
    mov [VbeModeInfo.PhysBasePtr], eax
    
    ; NOTE: `Start` is the actual starting point of the code, starts at 0x7E00
    jmp   Start

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

global DriveNumber
DriveNumber db 0

BsyBit:
    mov dx, 0x1F7
    .bruh:

    in al, dx

    test al, 0x80
    jnz .bruh
    ret 

SetupPIO:
    call BsyBit
    mov dx, 0x1F7
    mov al, 0xEF
    out dx, al
    mov dx, 0x1F2
    mov al, 0x08
    out dx, al
    call BsyBit
    ret


; ecx = LBA
; edi = Destination
LoadATASectorASM:
    and ecx, 0x0FFFFFFF
    mov eax, ecx
    shr eax, 24
    or al, 0b11100000
    mov dl, [DriveNumber]
    shl dl, 4
    or al, dl
    mov edx, 0x1F6
    out dx, al
    mov edx, 0x1F2
    
    mov al, 1
    out dx, al
    mov eax, ecx
    mov edx, 0x1F3
    out dx, al
    mov eax, ecx
    shr eax, 8
    mov edx, 0x1F4
    out dx, al
    mov eax, ecx
    shr eax, 16
    mov edx, 0x1F5
    out dx, al
    mov edx, 0x1F7
    mov al, 0x20 ; Read with retry
    out dx, al
.wait_drq_set:
    in al, dx
    test al, 8
    jz .wait_drq_set
    mov ecx, 256
    mov edx, 0x1F0
    rep insw
    ret
PhysBasePtr: dd 0

times 0x1BE-($-$$) db 0
db 0x80, 0, 1, 0, 0x0B, 0xFF, 0xFF, 0xFF
dd 1
dd 0xFFFFFFFF


times 510-($-$$) db 0
dw 0xAA55
global IsFirstTime
IsFirstTime:
db 1
times 512 * 3 - 1 db 0

FileAllocTable:
; File Allocation Table Sector(s) (32 sectors)
times 512 * 32 db 0

extern OS_Start

Start:
    call OS_Start
    cli
    hlt
  
align 16
%include "kernel/vesa_vbe_setup_vars.s"
%include "kernel/irq_handlers.s"



section .text



;
; ----------------------------------------------------------------------------
;
; Includes here
;
;

section .resources
global ResourcesAt
ResourcesAt:
incbin "resources.bin"

