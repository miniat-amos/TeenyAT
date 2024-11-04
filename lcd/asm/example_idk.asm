.const RAND 0x8010

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
.const MOUSEB 0xFFFB
.const TERM 0xFFFF
.const KEY 0xFFFE

.const COLORAMT 50
.const  MOVEAMT 1

SET rE, 0
!main
   CAL !init
   CAL !across
   CAL !down
   JMP !main

!init
    SET rA, 0
    SET rB, 0
    SET rC, 63
    SET rD, 63
    STR [X1], rA
    STR [Y1], rB
    STR [X2], rC
    STR [Y2], rD
    STR [STROKE], rE
    STR [LINE],rZ
    STR [UPDATE], rZ
    RET
    
!down 
    ADD rB, MOVEAMT
    cmp rB, 64
    JGE !return

    STR [X1], rA
    STR [Y1], rB

    NEG rB
    
    STR [X2], rC
    STR [Y2], rB
    STR [LINE], rZ
    STR [UPDATE], rZ

    NEG rB

    JMP !down


!across
    ADD rA, MOVEAMT
    cmp rA, 64
    JGE !return

    STR [X1], rA
    STR [Y1], rZ

    NEG rA
    
    STR [X2], rA
    STR [Y2], rD
    STR [LINE], rZ
    STR [UPDATE], rZ
    
    NEG rA

    JMP !across

!return
    ADD rE, COLORAMT
    STR [STROKE], rE   
    RET 
