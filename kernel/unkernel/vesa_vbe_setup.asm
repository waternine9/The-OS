;; Set up VESA VBE with 640x480 Resolution.
VesaVbeSetup:

; Get controller information
mov ax, 0x4F00
mov di, VbeControllerInfo
int 0x10

; Loop through the returned video modes to find one with 640x480 Resolution.

mov si, VbeControllerInfo.VideoModePtr
jmp .loop

.checkClosest:

; Accumulator
xor bx, bx

; Check diff X
mov ax, [VbeModeInfo.XResolution]
; Adding to accumulator
add bx, ax

; Check diff Y
mov ax, [VbeModeInfo.YResolution]
; Adding to accumulator
add bx, ax

cmp bx, [.closestAccum]
jl .loop

mov [.closestAccum], bx
mov ax, [VbeCurrentMode]
mov [.closestModeNumber], ax
mov ax, [VbeModeInfo.XResolution]
mov [VESA_RES_X], ax
mov ax, [VbeModeInfo.YResolution]
mov [VESA_RES_Y], ax

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
int 0x10

pop si

; Checking if resolution and bits per pixel matches the requirements, if not restart

; Check if 24 bit
mov al, [VbeModeInfo.BitsPerPixel]
cmp al, 24

jne .loop

mov ax, [VbeModeInfo.XResolution]
cmp ax, 1280
jg .loop

mov ax, [VbeModeInfo.YResolution]
cmp ax, 720
jg .loop

; Check if supports linear frame buffer
mov ax, [VbeModeInfo.ModeAttributes]
and ax, 0x90
cmp ax, 0x90

jne .loop

; Check if RGB
mov ax, [VbeModeInfo.MemoryModel]
cmp ax, 0x06

jne .loop

jmp .checkClosest

.loop_done:


; Set video mode IF FOUND
mov bx, [.closestModeNumber]
cmp bx, -1
je .done

mov ax, 0x4F01
mov cx, [.closestModeNumber]
mov di, VbeModeInfo
int 0x10

mov ax, 0x4F02
or bx, 0x4000          
xor di, di
int 0x10

.done:
ret


.closestAccum: dw 0x0
.closestModeNumber: dw -1
global VESA_RES_X
global VESA_RES_Y
VESA_RES_X: dw 0
VESA_RES_Y: dw 0