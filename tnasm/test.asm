; just a test assembly file to see how things are going

.const thirty_one 31

;-------------------
; Don't change any of the lines in this boxed section. They are crafted to
; demonstrate multipass label resolution is working
.variable amos 0x74A3 ;1
.var leroy
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
    str [r4-3], r3      ;1
    str [-3  + r4], r3      ;1

.raw 123 -15 thirty_one 0x12F_F 0b_110_00000_1011_00_10 ;5

    mpy r2, !more           ;2
    SHF rD, r2 - !middle    ;2
    lod r3, [rD]
    lod rA, [SP - 19]
    lod rA, [-19 + SP]
    lod PC, [!top_of_loop + rC]
    lod PC, [rC + !top_of_loop]
!more
    BTF r1, r3 + thirty_one ;2
    BtS r3, 5
    bTc rB, 4
    ;str SP - aero, amos
    rOt rB, !early ;2

;; Here are some PSH/POP expansion validations
    pop rD, [rA]
    psh [rC], rA
    psh [rD], rB - 17
    psh [rE], 4

    JMP PC - 4
    JMP -4 + PC
    STR [aero], r6
    DLy r3 + 5
    DLy r5
    DLy 31
    lod r4, ['a']
    lod r4, ['\n']

    psh r5 + thirty_one
    psh thirty_one + r5


    ; here's an example do..loop using LUP
    SET rC, 17
!top_of_loop
    ; do some stuff
    LUP rC, !top_of_loop

    Psh r4
    CAL PC ; oooh... what could this be used for? >:-)

    PSH PC - !more
    CAL rB + 0xF7

    lod r5, [r1 - amos]
    lod PC, [aero]

    RoL rB, 3
    ror rA, 2
    ROL rz, -1
    roR pc, -2

    ShL rB, 3
    ShR rA, 2
    ShL rz, -1
    ShR pc, -2

    set rB, 80
    dly rb, 1000  ; delay ~80ms at 1 MHz

    dly rA, rC
    dly rD, rE - 567 

    INT rA
    RTI

    INV rA                  ;2

    JNE rZ + !more
; and that's all

!my_string
.raw "Leroy"  ; a null terminated string

; packed strings have two characters per 16 bit word.  Here are 
; two examples one byte apart in length.
!a_packed_string  
.raw 'Aero jumps at midnight'
!another_packed_string
.raw 'Leroy jumps at midnight'
