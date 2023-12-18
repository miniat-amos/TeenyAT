; just a test assembly file to see how things are going

.const thirty_one 31

;-------------------
; Don't change any of the lines in this boxed section. They are crafted to
; demonstrate multipass label resolution is working
.variable amos 0x74A3 ;1
.variable aero 0xBeeF ;1

!main
    add r3, r4 + 7  ;1
    mod r3, r1      ;1
    OR  SP, rA - 5  ;1
!early
    div rE, 15      ;2
    xor rA, -17     ;2
!middle
;--------------------

    sub PC, PC - !main  ;1 this is an unconditional jmp :-)

    neg r1              ;1
    inc r2              ;1
    dec r2              ;1
    str [r4 + 16], r3     ;2
    jmp r3              ;1
    ret                 ;1
    psh 5              ;1
    psh -20              ;2
    psh r3 - 4          ;1
    psh r3              ;1
    jGE !main
    psh r3 + 14         ;2
    str [r4 + 3], r3      ;1

    123 -15 thirty_one 0x12F_F 0b_110_00000_1011_00_10 'a' ;6

    mpy r2, !more           ;2
    SHF rD, r2 - !middle    ;2
    lod r3, [rD]
!more
    BTF r1, r3 + thirty_one ;2
    ;str SP - aero, amos
    rOt rB, !early ;2

    JMP PC - 4
    STR [aero], r6
    DLy r3 + 5
    DLy r5
    DLy 31
    lod r4, 'a'
    lod r4, '\n'

    Psh r4
    CAL PC ; oooh... what could this be used for? >:-)

    PSH PC - !more
    CAL rB + 0xF7

    lod r5, [r1 - Amos]
    lod PC, aero

    JNE rZ + !more
; and that's all
