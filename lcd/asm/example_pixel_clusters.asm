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

.const AMT 50

!main

    str [rC + UPDATESCREEN], rA     ; put color rA into update buffer

    inc rC                          ; change which pixle were looking at 

    inc rD                          ; add one to rD and modulus by 4
    mod rD, 4

    cmp rC, 0x1000                  ; check if we've reached the max update screen address
    je !end_loop                    ; if we have end update 

    cmp rD, 0                 
    jne !main

    add rA, AMT                     ; increment color value by amount 

    jmp !main 

!end_loop 
    str [UPDATE], rZ                ; update our buffer