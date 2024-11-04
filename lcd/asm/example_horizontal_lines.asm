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
.const TERM 0xFFFF
.const KEY 0xFFFE

.const AMT 65
.const SLOWDOWN 4000

!main
    SET rA, 63
    SET rB, rZ
    SET rC, 0
    SET rD, 0
    SET rE, 0  ; color of our lines

!down
    STR [X1], rA
    STR [Y1], rB
    STR [X2], rC
    STR [Y2], rB
    STR [STROKE], rE
    STR [LINE], rA

    ADD rE, AMT
    ADD rB, 2
    DLY SLOWDOWN
    CMP rB, 64
    JNE !down

    DEC rB

!up
    STR [X1], rA
    STR [Y1], rB
    STR [X2], rC
    STR [Y2], rB
    STR [STROKE], rE
    STR [LINE], rA

    ADD rE, AMT
    SUB rB, 2
    DLY SLOWDOWN
    CMP rB, 0
    JGE !up

!again
    STR [UPDATE], rZ

    SET rC, 0
    SET rB, 0
    JMP !down

