; TeenyAT Constants
.const PORT_A_DIR   0x8000
.const PORT_B_DIR   0x8001
.const PORT_A       0x8002
.const PORT_B       0x8003
.const RAND         0x8010
.const RAND_BITS    0x8011

; LCD Peripherals
.const LIVESCREEN 0x9000
.const UPDATESCREEN 0xA000
.const X1 0xD000
.const Y1 0xD001
.const X2 0xD002
.const Y2 0xD003
.const STROKE 0xD010
.const FILL 0xD011
.const DRAWFILL 0xD012
.const DRAWSTROKE 0xD013
.const UPDATE 0xE000
.const RECT 0xE010
.const LINE 0xE011
.const POINT 0xE012
.const MOUSEX 0xFFFC
.const MOUSEY 0xFFFD
.const TERM 0xFFFF
.const KEY 0xFFFE

!main 
    str [X1], rZ          ; store zero into X1 address
    str [Y1], rZ          ; store zero into Y1 address

    set rA, 63      
    str [X2], rA          ; store rA into X2 address
    str [Y2], rA          ; store rA into Y2 address

    str [DRAWSTROKE], rZ  ; storing zero in to DRAWSTROKE means no stroke 

!loop
    str [FILL], rC        ; store rC to the FILL address
    str [RECT], rZ        ; blit rectangle to update buffer
    str [UPDATE], rZ      ; swap the display buffers
    inc rC                ; increase color value by 1
    jmp !loop
