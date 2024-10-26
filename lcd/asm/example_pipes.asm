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

.const AMT 17

!main

    ;; Set the random end of a nonexistent previous line
    CAL !rand_rA_-10...+10
    STR [X2], rA
    CAL !rand_rA_-10...+10
    STR [Y2], rA

    XOR rC, rC   ; start with color 0

!next_line
    ;; make the previous end of line the start of the next line
    LOD rA, [X2]
    STR [X1], rA
    LOD rA, [Y2]
    STR [Y1], rA

    ;; new random end point
    CAL !rand_rA_-10...+10
    LOD rB, [X2]
    ADD rA, rB

    CMP rA, 0
    JGE !x>=0
    SET rA, 0
!x>=0

    CMP rA, 63
    JLE !x<=63
    SET rA, 63
!x<=63

    STR [X2], rA

    CAL !rand_rA_-10...+10
    LOD rB, [Y2]
    ADD rA, rB

    CMP rA, 0
    JGE !y>=0
    SET rA, 0
!y>=0

    CMP rA, 63
    JLE !y<=63
    SET rA, 63
!y<=63

    STR [Y2], rA

    ;; set color and draw the line
    STR [STROKE], rC
    STR [LINE], rZ
    STR [UPDATE], rZ

    ADD rC, AMT   ; go to next color
    JMP !next_line

;--------------------

!rand_rA_-10...+10
    LOD rA, [RAND]
    AND rA, 0x7FFF   ; turn off sign bit (make positive)
    MOD rA, 21
    sub rA, 10
    RET
