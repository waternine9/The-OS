; ecx = LBA
; edi = Destination
; dl = Drive Number
kernel_read_ata:
    ; Get IO Port (primary or secondary) and whether drive is primary or secondary
    ; IO port start is stored in ebx, Prim or secon stored in edx (bit 5)!
    mov al, dl
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