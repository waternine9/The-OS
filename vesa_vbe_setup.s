%define VESA_MODE_NUMBER 0x112

;; Set up VESA VBE with 640x480 Resolution.
VesaVbeSetup:

; Get controller information
mov ax, 0x4F00
mov di, VbeControllerInfo
int 0x10

; Loop through the returned video modes to find one with 640x480 Resolution.

mov si, VbeControllerInfo.VideoModePtr
.loop:

mov cx, [si] ; Our current mode number
mov [VbeCurrentMode], cx

cmp cx, 0xFFFF ; Check if at end of the list
je .loop_done

add si, 2  ; Each element in the list is 2 bytes

push si

; Get current mode information. 
mov ax, 0x4F01
mov di, VbeModeInfo
mov cx, VESA_MODE_NUMBER
int 0x10

pop si

; Checking if resolution and bits per pixel matches the requirements, if not restart
mov ax, [VbeModeInfo.XResolution]
cmp ax, 640
jl .loop

mov ax, [VbeModeInfo.YResolution]
cmp ax, 480
jl .loop

; Check if supports linear frame buffer
mov ax, [VbeModeInfo.ModeAttributes]
and ax, 0x90
cmp ax, 0x90

jne .loop

; Check if RGB
mov ax, [VbeModeInfo.MemoryModel]
cmp ax, 0x06

jne .loop

.loop_done:

; Set video mode IF FOUND
mov bx, [VbeCurrentMode]
cmp bx, 0xFFFF
je .done

mov ax, 0x4F02
mov bx, VESA_MODE_NUMBER
or bx, 0x4000          
mov di, 0
int 0x10

.done:
ret
