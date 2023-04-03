
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
mov bx, 0

; Check diff X
mov ax, [VbeModeInfo.XResolution]
sub ax, [.closestResX]
; Taking absolute value then adding to accumulator
mov cx, ax
shr cx, 15
xor ax, cx
sub ax, cx
add bx, ax

; Check diff Y
mov ax, [VbeModeInfo.YResolution]
sub ax, [.closestResY]
; Taking absolute value then adding to accumulator
mov cx, ax
shr cx, 15
xor ax, cx
sub ax, cx
add bx, ax

cmp bx, [.closestAccum]
jg .loop

mov [.closestAccum], bx
mov ax, [VbeCurrentMode]
mov [.closestModeNumber], ax
mov ax, [VbeModeInfo.XResolution]
mov [.closestResX], ax
mov ax, [VbeModeInfo.YResolution]
mov [.closestResY], ax

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
cmp ax, 640
jne .checkClosest

mov ax, [VbeModeInfo.YResolution]
cmp ax, 480
jne .checkClosest

; Check if supports linear frame buffer
mov ax, [VbeModeInfo.ModeAttributes]
and ax, 0x90
cmp ax, 0x90

jne .checkClosest

; Check if RGB
mov ax, [VbeModeInfo.MemoryModel]
cmp ax, 0x06

jne .checkClosest

mov ax, [VbeCurrentMode]
mov [.closestModeNumber], ax

.loop_done:

; TESTING PURPOSES
mov word [.closestModeNumber], 0x112

; Set video mode IF FOUND
mov bx, [.closestModeNumber]
cmp bx, 0xFFFF
je .done

mov ax, 0x4F02
or bx, 0x4000          
mov di, 0
int 0x10

.done:
ret

.closestResX: dw 0
.closestResY: dw 0
.closestAccum: dw 0xFFFF
.closestModeNumber: dw -1