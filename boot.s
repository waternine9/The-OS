[BITS 16]
section .boot

Boot:
    ; NOTE: At boot the boot drive number is stored in DL,
    ;       Preserve it for later 
    mov   [DriveNumber], dl
    
    ; NOTE: Activate A20
    mov   ax, 0x2403
    int   0x15
    
    ; NOTE: Load the next sector into memory
    mov   ah, 0x02     ; Function 02h of Int 13h is used for reading sectors
    mov   al, 32       ; Read 32 sectors
    mov   ch, 0x00     ; Cylinder 0
    mov   cl, 0x02     ; Sector 2
    mov   dh, 0x00     ; Head 0
    mov   bx, 0x7E00   ; Destination address is 0000:7E00
    int   0x13         ; Call the disk interrupt to read the sector
    
    ; NOTE: SETUP VBE
    jmp SetupVbe
    
    %include "vesa_vbe_setup.s"

SetupVbe:
    call VesaVbeSetup
    
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

DriveNumber db 0

times 510-($-$$) db 0
dw 0xAA55

extern OS_Start

Start:
    sti
    call OS_Start
    cli
    hlt
  
align 16
%include "vesa_vbe_setup_vars.s"
%include "irq_handlers.s"
%include "ata.s"

; PS2 keyboard functions

global kernel_WaitForKey
kernel_WaitForKey:
.loop:
    in al, 0x60
    cmp al, 0x0
    jl .loop
    ret

global kernel_GetKeyPressed
kernel_GetKeyPressed:
    in al, 0x60
    cmp al, 0x0
    jl .error
    jmp .return
.error:
    mov al, -1
.return:
    ret

LastKey: db -1

global kernel_WaitForKeyNoRepeat
kernel_WaitForKeyNoRepeat:
    jmp .loop
.loop_reset:
    mov byte [LastKey], -1
.loop:
    in al, 0x60
    cmp al, 0x0
    jl .loop_reset
    cmp al, [LastKey]
    je .loop
    mov [LastKey], al
    ret

global kernel_GetKeyPressedNoRepeat
kernel_GetKeyPressedNoRepeat:
    in al, 0x60
    cmp al, 0x0
    jl .error_lt_0
    cmp al, [LastKey]
    je .error
    mov [LastKey], al
    jmp .return
.error_lt_0:
    mov byte [LastKey], -1
.error:
    mov al, -1
.return:
    ret




section .text

;
; ----------------------------------------------------------------------------
;
; Includes here
;
;




