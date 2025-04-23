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

.const COLORAMT 50
.const  MOVEAMT 1


!main
   cal !init
   cal !across
   cal !down
   jmp !main

!init
    set rA, 0
    set rB, 0
    set rC, 63
    set rD, 63
    str [X1], rA
    str [Y1], rB
    str [X2], rC
    str [Y2], rD
    str [STROKE], rE
    str [LINE],rZ
    str [UPDATE], rZ
    ret
    
!down 
    add rB, MOVEAMT
    cmp rB, 64
    jge !return

    str [X1], rA
    str [Y1], rB

    neg rB
    
    str [X2], rC
    str [Y2], rB
    str [LINE], rZ
    str [UPDATE], rZ

    neg rB

    jmp !down


!across
    add rA, MOVEAMT
    cmp rA, 64
    jge !return

    str [X1], rA
    str [Y1], rZ

    neg rA
    
    str [X2], rA
    str [Y2], rD
    str [LINE], rZ
    str [UPDATE], rZ
    
    neg rA

    jmp !across

!return
    add rE, COLORAMT
    str [STROKE], rE   
    ret 
