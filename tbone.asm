; Name:     tbone.asm
; Author:   John Pertell
;
; Desc:     Example tnasm assembly to showcase a  
;           simplified GPIO led system. Compile using
;           the tnasm assembler.

.var tbone 0
.const PORT_A 0x8002

set rA, tbone

!main
    str [PORT_A], rA
    inv rA ; Inverts the bits on rA
    jmp !main