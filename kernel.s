%include "boot.s"
%include "vesa_vbe_setup.s"

call VesaVbeSetup

; TESTING
mov dword [VbeModeInfo.PhysBasePtr], 0xFFFFFFFF
cli
hlt