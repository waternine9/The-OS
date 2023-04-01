;; Set up VESA VBE with 640x480 Resolution 256 colors.
VesaVbeSetup:

; Get controller information
mov ax, 0x4F00
mov di, VbeControllerInfo
int 0x10

; Loop through the returned video modes to find one with 640x480 Resolution 256 colors.

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
int 0x10

pop si

; Checking if resolution and bits per pixel matches the requirements, if not restart
mov ax, [VbeModeInfo.XResolution]
cmp ax, 640
jne .loop

mov ax, [VbeModeInfo.YResolution]
cmp ax, 480
jne .loop

mov ax, [VbeModeInfo.BitsPerPixel]
cmp ax, 24
jne .loop


.loop_done:

; Set video mode IF FOUND
mov bx, [VbeCurrentMode]
cmp bx, 0xFFFF
je .done

mov ax, 0x4F02
or bx, 0b0100_0000_0000_0000
int 0x10

.done:
ret


VbeControllerInfo:
    .VbeSignature db 'VESA' ; VBE Signature
    .VbeVersion dw 0200h ; VBE Version
    .OemStringPtr dd 0 ; Pointer to OEM String
    .Capabilities times 4 db 4 ; Capabilities of graphics controller
    .VideoModePtr dd 0 ; Pointer to VideoModeList
    .TotalMemory dw 0 ; Number of 64kb memory blocks
    ; Added for VBE 2.0
    .OemSoftwareRev dw 0 ; VBE implementation Software revision
    .OemVendorNamePtr dd 0 ; Pointer to Vendor Name String
    .OemProductNamePtr dd 0 ; Pointer to Product Name String
    .OemProductRevPtr dd 0 ; Pointer to Product Revision String
    .Reserved times 222 db 0 ; Reserved for VBE implementation scratch
    ; area
    .OemData times 256 db 0 ; Data Area for OEM Strings
VbeCurrentMode: dw 0
VbeModeInfo:
    ; Mandatory information for all VBE revisions
    .ModeAttributes dw 0 ; mode attributes
    .WinAAttributes db 0 ; window A attributes
    .WinBAttributes db 0 ; window B attributes
    .WinGranularity dw 0 ; window granularity
    .WinSize dw 0 ; window size
    .WinASegment dw 0 ; window A start segment
    .WinBSegment dw 0 ; window B start segment
    .WinFuncPtr dd 0 ; pointer to window function
    .BytesPerScanLine dw 0 ; bytes per scan line
    ; Mandatory information for VBE 1.2 and above
    .XResolution dw 0 ; horizontal resolution in pixels or characters3
    .YResolution dw 0 ; vertical resolution in pixels or characters
    .XCharSize db 0 ; character cell width in pixels
    .YCharSize db 0 ; character cell height in pixels
    .NumberOfPlanes db 0 ; number of memory planes
    .BitsPerPixel db 0 ; bits per pixel
    .NumberOfBanks db 0 ; number of banks
    .MemoryModel db 0 ; memory model type
    .BankSize db 0 ; bank size in KB
    .NumberOfImagePages db 0 ; number of images
    .Reserved1 db 1 ; reserved for page function
    ; Direct Color fields (required for direct/6 and YUV/7 memory models)
    .RedMaskSize db 0 ; size of direct color red mask in bits
    .RedFieldPosition db 0 ; bit position of lsb of red mask
    .GreenMaskSize db 0 ; size of direct color green mask in bits
    .GreenFieldPosition db 0 ; bit position of lsb of green mask
    .BlueMaskSize db 0 ; size of direct color blue mask in bits
    .BlueFieldPosition db 0 ; bit position of lsb of blue mask
    .RsvdMaskSize db 0 ; size of direct color reserved mask in bits
    .RsvdFieldPosition db 0 ; bit position of lsb of reserved mask
    .DirectColorModeInfo db 0 ; direct color mode attributes
    ; Mandatory information for VBE 2.0 and above
    .PhysBasePtr dd 0 ; physical address for flat memory frame buffer
    .OffScreenMemOffset dd 0 ; pointer to start of off screen memory
    .OffScreenMemSize dw 0 ; amount of off screen memory in 1k units
    .Reserved2 times 206 db 0 ; remainder of ModeInfoBlock
