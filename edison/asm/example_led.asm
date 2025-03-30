.const PORT_A_DIR 0x8000
.const PORT_B_DIR 0x8001
.const PORT_A 0x8002
.const PORT_B 0x8003
.const RAND 0x8010
.const RAND_BITS 0x8011

.const LCD_CURSOR 0xA000 
.const LCD_CLEAR_SCREEN 0xA001
.const LCD_MOVE_LEFT 0xA002  
.const LCD_MOVE_RIGHT 0xA003
.const LCD_MOVE_UP 0xA004
.const LCD_MOVE_DOWN 0xA005
.const LCD_MOVE_LEFT_WRAP 0xA006  
.const LCD_MOVE_RIGHT_WRAP 0xA007
.const LCD_MOVE_UP_WRAP 0xA008
.const LCD_MOVE_DOWN_WRAP 0xA009
.const LCD_CURSOR_X 0xA00A
.const LCD_CURSOR_Y 0xA00B
.const LCD_CURSOR_XY 0xA00C
.const BUZZER_LEFT 0xA010
.const BUZZER_RIGHT 0xA011
.const FADER_LEFT 0xA020
.const FADER_RIGHT 0xA021

set rA, 0xFFFF ; set port directions to output 
str [PORT_A_DIR], rA


str [PORT_B_DIR], rZ

set rA, 0xFFFF
set rB, 0x8000

!main
    str [PORT_B], rA
    cal !blink_delay

    set rC, rA
    xor rC, rB
    str [PORT_B], rC
    rot rB, 1
    cal !blink_delay

    jmp !main

; wastes about 175000 cycles
!blink_delay
    psh rC
    set rC, 175
    
    !blink_loop
        dly 1000
        lup rC, !blink_loop
        
    pop rC
    ret