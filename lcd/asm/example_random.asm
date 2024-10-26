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

.const AMT 100

!main
    SET rD, rZ
    STR [UPDATE], rZ
!loop
    ; get random x and y values
    LOD rA, [RAND]
    MOD rA, 64
    LOD rB, [RAND]
    MOD rB, 64
    ; get random color valye
    LOD rC, [RAND]
    STR [STROKE], rC

    STR [X1], rA
    STR [Y1], rB
    STR [POINT], rZ

    INC rD
    CMP rD, AMT
    JGE !main
    JMP !loop
    