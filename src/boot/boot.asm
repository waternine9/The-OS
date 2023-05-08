[BITS 16]
section .boot


Boot:
    

    ; NOTE: At boot the boot drive number is stored in DL,
    ;       Preserve it for later 
    mov   [DriveNumber], dl

    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov ax, 0x7C00
    mov sp, ax

    xor ax, ax
    mov es, ax

    ; NOTE: Load the OS
    mov si, DAPACK		; address of "disk address packet"
    mov ah, 0x42		; AL is unused
    mov dl, 0x80		; drive number 0 (OR the drive # with 0x80)
    or dl, [DriveNumber]
    int 0x13

    ; NOTE: Activate A20
    mov   ax, 0x2403
    int   0x15


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

    mov eax, 0x1000000
    mov esp, eax

    ; Use scancode set 1 (PS/2 keyboard)
    mov dx, 0x60
    mov al, 0xF0
    out dx, al
    mov al, 1
    out dx, al

    jmp unkernel_end

align 4
DAPACK:
	db	0x10
	db	0
blkcnt:	dw	0x7F		; int 13 resets this to # of blocks actually read/written
db_add:	dw	0x7E00		; memory buffer destination address (0:7c00)
	dw	0		; in memory page zero
d_lba:	dd	1		; put the lba to read in this spot
	dd	0		; more storage bytes only for big lba's ( > 4 bytes )

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
    .GDTSize dw GDTEnd - GDTStart - 1 ; GDT size
    .GDTAddr dd GDTStart          ; GDT address

times 0x1BE-($-$$) db 0

db 0x80, 0, 0, 0, 0, 0, 0, 0
dd 0
dd 0xFFFFFFFF

times 510-($-$$) db 0
dw 0xAA55

unkernel_end:

extern kmain
cli
call kmain
cli
hlt

%include "src/boot/irq_handlers.asm"

section .text

section .end
OsEnd: