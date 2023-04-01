%include "boot.s"
mov eax, 0
mov al, VbeModeInfo.BitsPerPixel
cli
hlt