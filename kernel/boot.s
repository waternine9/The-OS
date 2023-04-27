
%include "kernel/unkernel/unkernel.asm"

; THE BOOTLOADER LOADS THIS CODE AT LBA 4 FOR 1 SECTOR, OUR TASK IS TO LOAD THE OS NOW

; Use scancode set 1
mov dx, 0x60
mov al, 0xF0
out dx, al    
mov al, 1
out dx, al

; Find RSDP
mov ecx, 0x20000
mov esi, 0xE0000
RSDPfind:
mov eax, [esi]
cmp eax, "RSD "
jne .NotFound
mov eax, [esi + 4]
cmp eax, "PTR "
je .Found
.NotFound:
inc esi
loop RSDPfind
mov eax, 0xEEEEEE
cli
hlt
.Found:
mov edi, rsdp
mov ecx, 24
.Copy:
mov eax, [esi]
mov [edi], eax
inc esi

inc edi
loop .Copy
mov edi, IsFirstTime
mov ecx, 5

LoadOS:
mov dl, [DriveNumber]
push edi
push ecx
call unkernel_read_ata
pop ecx
pop edi
inc ecx
add edi, 512
cmp edi, OS_End - 512
jl LoadOS
 

; Now boot the OS
jmp Start

times (512 * 5) - ($-$$) db 0
global IsFirstTime
IsFirstTime:
db 1
times 511 db 0

; CODE BELOW STARTS AT LBA 5
FileAllocTable:
; File Allocation Table Sector(s) (32 sectors)
times 512 * 32 db 0
FileSysVars:
; Reserved for extra variables in file system (1 sector)
times 512 db 0
section .text


extern OS_Start

Start:
    call OS_Start
    cli
    hlt
  
align 16
%include "kernel/irq_handlers.s"

xapicBase: dd 0




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
OS_End:
