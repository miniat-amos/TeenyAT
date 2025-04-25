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

.const FIRST_CHARACTER 33
.const LAST_CHARACTER 247

; EDISON BOARD LCD PERIPHERAL
;
; The Edison board lcd can display all standard ASCII characters 
;                                 domain: 0 -->  127
;
; The Edison lcd also supports characters custom to Edison for display
;                                 domain: 128 --> 247
;
; The 3 msb of the value stored to the lcd affect its display
;       
;        Bit15  --> rotate 180 degrees
;        Bit14  --> flip horizontal
;        Bit13  --> flip vertical
;
; Either none or any combination of them can be toggled at a time       

!reset
    set rA, FIRST_CHARACTER
!main
    cmp rA, LAST_CHARACTER
    jg !reset
    str [LCD_CURSOR], rA
    inc rA
    jmp !main
