; Hit key A to draw new mosaic
; Hit key S to mutate mosaic

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

.const MUTATEAMT 50
.const COLORAMT 50
.const AMT 100

jmp !loop
!main
    SET rD, rZ
    STR [UPDATE], rZ
    !key_loop
    LOD rA, [KEY]
    CMP rA, 65 ; A KEY
    JE !loop
    CMP rA, 83 ; S Key 
    JE !mutate
    JMP !main
!loop
    ; get random x1 and y1 values
    LOD rA, [RAND]
    MOD rA, 64
    LOD rB, [RAND]
    MOD rB, 64
    ; get random color valye
    LOD rC, [RAND]
    STR [STROKE], rC


    STR [X1], rA
    STR [Y1], rB

     ; get random x1 and y1 values
    LOD rA, [RAND]
    MOD rA, 64
    LOD rB, [RAND]
    MOD rB, 64
    ; get random color valye
    LOD rC, [RAND]
    STR [FILL], rC


    STR [X2], rA
    STR [Y2], rB
    STR [RECT], rZ
    INC rD
    CMP rD, AMT
    JGE !main  
    JMP !loop


!mutate
    SET rA, 0
    !mutate_loop
        LOD rE, [rA + UPDATESCREEN]
        ADD rE, MUTATEAMT
        STR [rA + UPDATESCREEN], rE
        INC rA
        CMP rA, 0x1000
        JG !end_mutate
        JMP !mutate_loop
    !end_mutate
        STR [UPDATE],rZ
        JMP !main