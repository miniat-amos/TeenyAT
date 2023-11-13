; just a test assembly file to see how things are going

.const thirty_one 31
.variable amos 0x74A3
.variable aero 0xBeeF

!main
    add r3, r4 + 7
    mod r3, r1
    neg r1
    OR  SP, rA - 5
    div rE, 15
    xor rA, -17
    ;MpY rC, pc
    sub PC, PC - !main ;this is an unconditional jmp :-)

    123 -15 thirty_one 0x12F_F 0b_110_00000_1011_00_10

    mpy r2, !more

!more
    BTF r1, r3 + thirty_one
    ;str SP - aero, amos

; and that's all
