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

.const AMT 100          ; the amount of times we draw a random point

!main
    set rD, rZ          
    str [UPDATE], rZ    ; swap lcd texture buffers
!loop

    lod rA, [RAND_BITS] ; get random x value
    mod rA, 64          ; modulus to keep in range

    lod rB, [RAND_BITS] ; get random y value
    mod rB, 64          ; modulus to keep in range

    
    lod rC, [RAND]      ; get random color value
    str [STROKE], rC    ; store value to stroke address

    str [X1], rA        ; store into random value into X1 address
    str [Y1], rB        ; store into random value into Y1 address
    str [POINT], rZ     ; blit our point to lcd screen

    inc rD              ; increment our counter

    cmp rD, AMT         ; jump to main if rD is greater than amount
    jge !main

    jmp !loop
    