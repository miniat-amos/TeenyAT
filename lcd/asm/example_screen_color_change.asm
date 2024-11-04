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

!main 
    STR [X1], rZ
    STR [Y1], rZ
    SET rA, 63
    STR [X2], rA
    STR [Y2], rA
    STR [DRAWSTROKE], rZ  ;no stroke
!top
    STR [FILL], rC
    STR [RECT], rD
    STR [UPDATE], SP
    INC rC  
    JMP !top
