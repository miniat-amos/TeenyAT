JMP !main

!Screen #8000

!main
    SET $a $z
    SET $b $z
    SET $c $z
!loop
    STR $c !Screen $a
    INC $c
    JE !end
    ADD $a 7
    MOD $c #1000
    JMP !loop
!end