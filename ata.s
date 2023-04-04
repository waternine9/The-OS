global ReadATASector
ReadATASector:
; Save the base pointer (EBP) on the stack
    push ebp
    ; Set the new base pointer to the current stack pointer (ESP)
    mov ebp, esp
    mov edi, [ebp + 8]
    mov ebx, [ebp + 12]
    mov     dx,1f6h         ;Drive and head port
    mov     al,0b11100000         ;Drive 0, head 0
    out     dx,al

    mov     dx,1f2h         ;Sector count port
    mov     al,1            ;Read one sector
    out     dx,al

    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al

    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al


    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al

    mov     dx,1f7h         ;Command port
    mov     al,20h          ;Read with retry.
    out     dx,al
.still_going:
    in      al,dx
    test    al,8            ;This means the sector buffer requires
            ;servicing.
    jz      .still_going     ;Don't continue until the sector buffer
            ;is ready.

    mov ecx, 256
    mov     dx,1f0h         ;Data port - data comes in and out of here.
    rep     insw
    pop ebp
    ret

WriteATASector:
; Save the base pointer (EBP) on the stack
    push ebp
    ; Set the new base pointer to the current stack pointer (ESP)
    mov ebp, esp
    mov edi, [ebp + 8]
    mov ebx, [ebp + 12]
    mov     dx,1f6h         ;Drive and head port
    mov     al,0b11100000         ;Drive 0, head 0
    out     dx,al

    mov     dx,1f2h         ;Sector count port
    mov     al,1            ;Read one sector
    out     dx,al

    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al

    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al


    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al

    mov     dx,1f7h         ;Command port
    mov     al,30h          ;Write with retry.
    out     dx,al
.still_going:
    in      al,dx
    test    al,8            ;This means the sector buffer requires
            ;servicing.
    jz      .still_going     ;Don't continue until the sector buffer
            ;is ready.

    mov ecx, 256
    mov     dx,1f0h         ;Data port - data comes in and out of here.
    rep     outsw
    pop ebp
    ret
