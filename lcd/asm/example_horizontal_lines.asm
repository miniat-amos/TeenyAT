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

.const AMT 65
.const SLOWDOWN 750

!main
    set rA, 63
    set rB, rZ
    set rC, 0
    set rD, 0
    set rE, 0           ; color of our lines

!down
    str [X1], rA
    str [Y1], rB
    str [X2], rC
    str [Y2], rB
    str [STROKE], rE
    str [LINE], rA      ; blit line to update buffer

    add rE, AMT
    add rB, 2
    dly SLOWDOWN        ; delay SLOWDOWN amount of cycles
    cmp rB, 64
    jne !down

    dec rB

!up
    str [X1], rA
    str [Y1], rB
    str [X2], rC
    str [Y2], rB
    str [STROKE], rE
    str [LINE], rA

    add rE, AMT
    sub rB, 2
    dly SLOWDOWN        ; delay SLOWDOWN amount of cycles
    cmp rB, 0
    jge !up

!again
    str [UPDATE], rZ    ; swap buffers

    set rC, 0
    set rB, 0
    jmp !down

