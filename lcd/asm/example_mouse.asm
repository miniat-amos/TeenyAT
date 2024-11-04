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

.const MOUSE_LEFT 1
.const MOUSE_RIGHT 2
 
.const MUTATEAMT 50
.const COLORAMT 50

SET rD, 0
!main
    LOD rA, [MOUSEX]
    LOD rB, [MOUSEY]
    LOD rC, [MOUSEB]

    CMP rC, MOUSE_LEFT ; left down
    JE !changecolor_and_paint

    CMP rC, MOUSE_RIGHT ; middle down
    JE !mutate

    JMP !main
    
!changecolor_and_paint
    ADD rD, COLORAMT
    STR [X1], rA
    STR [Y1], rB
    STR [STROKE], rD
    STR [POINT], rZ
    STR [UPDATE], rZ
    JMP !main

!mutate
    SET rA, 0
    !loop
        LOD rC, [MOUSEB]
        CMP rC, MOUSE_RIGHT 
        JNE !main
        
        LOD rE, [rA + UPDATESCREEN]
        ADD rE, MUTATEAMT
        STR [rA + UPDATESCREEN], rE
        INC rA
        CMP rA, 0x1000
        JG !end
        JMP !loop
    !end
        STR [UPDATE],rZ
        JMP !main
