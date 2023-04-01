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

add si, 2 ; Each element in the list is 2 bytes

push si

; Get current mode information. 
mov ax, 0x4F01
mov di, VbeModeInfo
int 0x10

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
    VbeSignature db 'VESA' ; VBE Signature
    VbeVersion dw 0200h ; VBE Version
    OemStringPtr dd ? ; Pointer to OEM String
    Capabilities db 4 dup (?) ; Capabilities of graphics controller
    VideoModePtr dd ? ; Pointer to VideoModeList
    TotalMemory dw ? ; Number of 64kb memory blocks
    ; Added for VBE 2.0
    OemSoftwareRev dw ? ; VBE implementation Software revision
    OemVendorNamePtr dd ? ; Pointer to Vendor Name String
    OemProductNamePtr dd ? ; Pointer to Product Name String
    OemProductRevPtr dd ? ; Pointer to Product Revision String
    Reserved db 222 dup (?) ; Reserved for VBE implementation scratch
    ; area
    OemData db 256 dup (?) ; Data Area for OEM Strings
VbeCurrentMode: dw ?
VbeModeInfo:
    ; Mandatory information for all VBE revisions
    ModeAttributes dw ? ; mode attributes
    WinAAttributes db ? ; window A attributes
    WinBAttributes db ? ; window B attributes
    WinGranularity dw ? ; window granularity
    WinSize dw ? ; window size
    WinASegment dw ? ; window A start segment
    WinBSegment dw ? ; window B start segment
    WinFuncPtr dd ? ; pointer to window function
    BytesPerScanLine dw ? ; bytes per scan line
    ; Mandatory information for VBE 1.2 and above
    XResolution dw ? ; horizontal resolution in pixels or characters3
    YResolution dw ? ; vertical resolution in pixels or characters
    XCharSize db ? ; character cell width in pixels
    YCharSize db ? ; character cell height in pixels
    NumberOfPlanes db ? ; number of memory planes
    3Pixels in graphics modes, characters in text modes.
    VBE Functions
    03h Return Current VBE Mode
    VBE CORE FUNCTIONS VERSION 2.0 Page 17
    DOCUMENT REVISION 1.1
    BitsPerPixel db ? ; bits per pixel
    NumberOfBanks db ? ; number of banks
    MemoryModel db ? ; memory model type
    BankSize db ? ; bank size in KB
    NumberOfImagePages db ? ; number of images
    Reserved db 1 ; reserved for page function
    ; Direct Color fields (required for direct/6 and YUV/7 memory models)
    RedMaskSize db ? ; size of direct color red mask in bits
    RedFieldPosition db ? ; bit position of lsb of red mask
    GreenMaskSize db ? ; size of direct color green mask in bits
    GreenFieldPosition db ? ; bit position of lsb of green mask
    BlueMaskSize db ? ; size of direct color blue mask in bits
    BlueFieldPosition db ? ; bit position of lsb of blue mask
    RsvdMaskSize db ? ; size of direct color reserved mask in bits
    RsvdFieldPosition db ? ; bit position of lsb of reserved mask
    DirectColorModeInfo db ? ; direct color mode attributes
    ; Mandatory information for VBE 2.0 and above
    PhysBasePtr dd ? ; physical address for flat memory frame buffer
    OffScreenMemOffset dd ? ; pointer to start of off screen memory
    OffScreenMemSize dw ? ; amount of off screen memory in 1k units
    Reserved db 206 dup (?) ; remainder of ModeInfoBlock