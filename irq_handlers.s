global HandlerIRQ0
global HandlerIRQ1
global HandlerIRQ2
global HandlerIRQ3
global HandlerIRQ4
global HandlerIRQ5
global HandlerIRQ6 
global HandlerIRQ7
global HandlerIRQ8
global HandlerIRQ9
global HandlerIRQ10 
global HandlerIRQ11
global HandlerIRQ12 
global HandlerIRQ13 
global HandlerIRQ14 
global HandlerIRQ15
global LoadIDT

extern CHandlerIRQ0
extern CHandlerIRQ1
extern CHandlerIRQ2
extern CHandlerIRQ3
extern CHandlerIRQ4
extern CHandlerIRQ5
extern CHandlerIRQ6
extern CHandlerIRQ7
extern CHandlerIRQ8
extern CHandlerIRQ9
extern CHandlerIRQ10
extern CHandlerIRQ11
extern CHandlerIRQ12
extern CHandlerIRQ13
extern CHandlerIRQ14
extern CHandlerIRQ15

HandlerIRQ0:
    pusha
    call CHandlerIRQ0
    popa
    iret

HandlerIRQ1:
    pusha
    call CHandlerIRQ1
    popa
    iret

HandlerIRQ2:
    pusha
    call CHandlerIRQ2
    popa
    iret

HandlerIRQ3:
    pusha
    call CHandlerIRQ3
    popa
    iret

HandlerIRQ4:
    pusha
    call CHandlerIRQ4
    popa
    iret

HandlerIRQ5:
    pusha
    call CHandlerIRQ5
    popa
    iret

HandlerIRQ6:
    pusha
    call CHandlerIRQ6
    popa
    iret

HandlerIRQ7:
    pusha
    call CHandlerIRQ7
    popa
    iret

HandlerIRQ8:
    pusha
    call CHandlerIRQ8
    popa
    iret

HandlerIRQ9:
    pusha
    call CHandlerIRQ9
    popa
    iret

HandlerIRQ10:
    pusha
    call CHandlerIRQ10
    popa
    iret

HandlerIRQ11:
    pusha
    call CHandlerIRQ11
    popa
    iret

HandlerIRQ12:
    pusha
    call CHandlerIRQ12
    popa
    iret

HandlerIRQ13:
    pusha
    call CHandlerIRQ13
    popa
    iret

HandlerIRQ14:
    pusha
    call CHandlerIRQ14
    popa
    iret

HandlerIRQ15:
    pusha
    call CHandlerIRQ15
    popa
    iret  

LoadIDT:
    mov   edx, [esp + 4]
    lidt  [edx]
    sti
    ret