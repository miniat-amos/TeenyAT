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

; EDISON BOARD BUZZER PERIPHERAL
;
; Bit15 defines wave to be played 0 -> sine wave
;                                 1 -> triangle wave
;
; Bit14 -> Bit0 are the frequency to be played
; 
; Stores to the buzzer turn them on and make them play the 
; sound/frequency until a zero is stored into that buzzer 
; thats pretty much it....

!init
    str [PORT_A_DIR], rZ    ; set port A to output
    str [PORT_B_DIR], rZ    ; set port B to output 
    ;  Print out info to lcd screen
    set rB, !use_faders_str  
    cal !print_string_rB
    set rB, 0x0001
    str [LCD_CURSOR_XY], rB
    set rB, !turn_sound_up_str
    cal !print_string_rB
!main
    str [BUZZER_LEFT], rA
    str [BUZZER_RIGHT], rB

    lod rA, [FADER_LEFT]
    lod rB, [FADER_RIGHT]

    mpy rA, 100
    mpy rB, 100
    bts rA, 15
    btc rB, 15

    str [PORT_A], rA
    str [PORT_B], rB

    jmp !main

;------------- UTILITY SECTION ------------------
!print_string_rB
    psh rB
    psh rA

    !print_string_rB_loop
        lod rA, [rB]
        cmp rA, rZ
        je !print_string_rB_terminated

        str [LCD_CURSOR], rA
        inc rB
        jmp !print_string_rB_loop

    !print_string_rB_terminated
        pop rA
        pop rB

     ret
;------------- DATA SECTION ---------------------
!use_faders_str
'U' 's' 'e'  ' ' 'f' 'a' 'd' 'e' 'r' 's' 0

!turn_sound_up_str
'T' 'u' 'r' 'n' ' ' 's' 'o' 'u' 'n' 'd' ' ' 'u' 'p' '!' 0