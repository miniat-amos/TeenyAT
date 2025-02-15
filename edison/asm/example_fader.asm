.const PORT_A_DIR 0x8000
.const PORT_B_DIR  0x8001
.const PORT_A 0x8002
.const PORT_B 0x8003
.const FADER_LEFT 0xA020
.const FADER_RIGHT 0xA021


!main
    LOD rA, [FADER_LEFT]
    CMP rA, 50
    JGE !light_led
    set rB, 0xF0FF
    STR [PORT_B], rB

    !check_right
    LOD rA, [FADER_RIGHT]
    CMP rA, 50
    JGE !reversed

    set rB, 0x0F00
    STR [PORT_A], rB
    JMP !main

!light_led
    set rB, 0xFFFF
    STR [PORT_B], rB
    JMP !main

!reversed
    set rB, 0xF0F0
    STR [PORT_A], rB
    JMP !main
