JMP !main

!Screen #9000

!main
    SET $a $z
    SET $b $z
    SET $c $z
!loop
    STR $c !Screen $a
    INC $c
    CMP $c #1000
    JE !end
    INC $a
    MOD $a 360
    JMP !loop
!end
STR $z #E000 $z