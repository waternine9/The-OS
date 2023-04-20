[BITS 16]
section .boot

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
    mov edi, 0x7E00
    mov ecx, 1

    .keyboard_check:
    xor ax,ax
    in al,0x64
    bt ax, 1 ; Test if buffer is still full
    jc .keyboard_check

    ; Disable keyboard because of a weird bug
    mov dx, 0x60
    mov al, 0xF5
    out dx, al

LoadSectors:
    push edi
    push ecx
    call LoadATASectorASM
    pop ecx
    pop edi
    
    add edi, 512
    inc ecx
    cmp edi, Os_End

    jl LoadSectors

    mov eax, [PhysBasePtr]
    mov [VbeModeInfo.PhysBasePtr], eax
    
    .keyboard_check:
    xor ax,ax
    in al,0x64
    bt ax, 1 ;test if buffer is still full
    jc .keyboard_check

    ; Enable keyboard because of a weird bug
    mov dx, 0x60
    mov al, 0xF4
    out dx, al

    ; Use scancode set 1
    mov dx, 0x60
    mov al, 0xF0
    out dx, al
    
    mov al, 1
    out dx, al
    

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


; ecx = LBA
; edi = Destination
LoadATASectorASM:
    ; Get IO Port (primary or secondary) and whether drive is primary or secondary
    ; IO port start is stored in ebx, Prim or secon stored in edx (bit 5)!
    mov al, [DriveNumber]
    cmp al, 0x80
    jne .second_test
    mov ebx, 0x1F0
    xor edx, edx
    jmp .end_probe
.second_test:
    cmp al, 0x81
    jne .third_test
    mov ebx, 0x1F0
    mov edx, 0b10000
    jmp .end_probe
.third_test:
    cmp al, 0x82
    jne .fourth_test
    mov ebx, 0x170
    xor edx, edx
    jmp .end_probe
.fourth_test:
    mov ebx, 0x170
    mov edx, 0b10000
.end_probe:



    and ecx, 0x0FFFFFFF
    mov eax, ecx
    shr eax, 24

    or al, 0b11100000
    or al, dl
    mov edx, ebx
    add edx, 6
    out dx, al

    mov edx, ebx
    add edx, 2
    mov al, 1
    out dx, al
    mov eax, ecx
    mov edx, ebx
    add edx, 3
    out dx, al
    mov eax, ecx
    shr eax, 8
    mov edx, ebx
    add edx, 4
    out dx, al
    mov eax, ecx
    shr eax, 16
    mov edx, ebx
    add edx, 5
    out dx, al
    mov edx, ebx
    add edx, 7
    mov al, 0x20 ; Read with retry
    out dx, al
.wait_drq_set:
    in al, dx
    test al, 8
    jz .wait_drq_set
    mov ecx, 256
    mov edx, ebx
    rep insw
    ret
PhysBasePtr: dd 0


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
Os_End:
