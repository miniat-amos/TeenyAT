;                            dP                                                   dP
;                            88                                                   88
; .d8888b. dP    dP .d8888b. 88 .d8888b.    .d8888b. .d8888b. dP    dP 88d888b. d8888P
; 88'  `"" 88    88 88'  `"" 88 88ooood8    88'  `"" 88'  `88 88    88 88'  `88   88
; 88.  ... 88.  .88 88.  ... 88 88.  ...    88.  ... 88.  .88 88.  .88 88    88   88
; `88888P' `8888P88 `88888P' dP `88888P'    `88888P' `88888P' `88888P' dP    dP   dP
;               .88
;           d8888P
; 
; Description:
; Demonstrates the TeenyAT "Cycle Count" internal peripheral.
; The Edison Port B 7-segment display shows the cycles elapsed
; since the program started or cycle count was reset.  Instead
; of the raw cycle count, though, it uses an expponentially
; scaled divisor you can control with the Port A DIPs on the
; Edison board.  Say you set the DIPs to 0011 (or 3)... this
; will define an exponential divisor, 2^3 == 8.  The 7-segment
; display will show the cycle count divided by 8.
;
; Controls:
; - Port A DIPs to control the exponentially scaled divisor
; - Press 'R' (Port A, Pin 12) to reset the count to zero.

; TeenyAT Constants
.const PORT_A_DIR       0x8000
.const PORT_B_DIR       0x8001
.const PORT_A           0x8002
.const PORT_B           0x8003
.const RAND             0x8010
.const RAND_BITS        0x8011
.const CYCLE_CNT        0x8090
.const CYCLE_CNT_RESET  0x8091
.const WALL_TIME        0x8094
.const WALL_TIME_RESET  0x8095

.const CONTROL_STATUS_REGISTER  0x8EFF

; Edison Constants
.const LCD_CURSOR           0xA000
.const LCD_CLEAR_SCREEN     0xA001
.const LCD_MOVE_LEFT        0xA002
.const LCD_MOVE_RIGHT       0xA003
.const LCD_MOVE_UP          0xA004
.const LCD_MOVE_DOWN        0xA005
.const LCD_MOVE_LEFT_WRAP   0xA006
.const LCD_MOVE_RIGHT_WRAP  0xA007
.const LCD_MOVE_UP_WRAP     0xA008
.const LCD_MOVE_DOWN_WRAP   0xA009
.const LCD_CURSOR_X         0xA00A
.const LCD_CURSOR_Y         0xA00B
.const LCD_CURSOR_XY        0xA00C
.const BUZZER_LEFT          0xA010
.const BUZZER_RIGHT         0xA011
.const FADER_LEFT           0xA020
.const FADER_RIGHT          0xA021


.const R_BUTTON_MASK  0x1000

.const DIPS_MASK      0x00F0
.const DIPS_LOW_BIT   4

.const DIVISOR_EXPONENT_MASK     0b_0011_1100
.const DIVISOR_EXPONENT_LOW_BIT  2


    ; Skip over data to !main
    jmp !main

!info1
.raw "7-Seg is cycle count"

!info2
.raw 0x20 0x95 0x98 "Try hex mode"

!info3
.raw 0x20 0x95 0x98 "DIPs set divisor"

!info4
.raw 0x20 0x95 0x98 "'R' resets count"

!main

; Port A, pin 12 (the 'R' key) is button input
; Port A, pins 4..7 (the DIPs) are inputs
    set rA, R_BUTTON_MASK
    or  rA, DIPS_MASK
    str [PORT_A_DIR], rA

; Port B bits are all output
    str [PORT_B_DIR], rZ

; First info string at (0,0)

    str [LCD_CURSOR_XY], rZ
    psh !info1
    cal !print_string

; Second info string at (1,0)
    str [LCD_CURSOR_X], rZ
    set rA, 1
    str [LCD_CURSOR_Y], rA
    psh !info2
    cal !print_string

; Third info string at (2,0)
    str [LCD_CURSOR_X], rZ
    set rA, 2
    str [LCD_CURSOR_Y], rA
    psh !info3
    cal !print_string

; Fourth info string at (3,0)
    str [LCD_CURSOR_X], rZ
    set rA, 3
    str [LCD_CURSOR_Y], rA
    psh !info4
    cal !print_string

!forever

; Get the state of R button and DIPs
    lod rA, [PORT_A]

; Check whether to reset the wall time base
    set rB, rA
    and rB, R_BUTTON_MASK
    cmp rB, rZ
    je  !set_divisor_scale

    str [CYCLE_CNT_RESET], rZ

!set_divisor_scale

; Isolate the 4 DIP bits and move to the divisor bits' position in the CSR
    and rA, DIPS_MASK
    shr rA, DIPS_LOW_BIT
    shl rA, DIVISOR_EXPONENT_LOW_BIT

; Clear the previous divisor bits w/ ~DIVISOR_EXPONENT_MASK
    lod rB, [CONTROL_STATUS_REGISTER]
    set rC, DIVISOR_EXPONENT_MASK
    inv rC
    and rB, rC

; Set the new divisor bits
    or  rB, rA
    str [CONTROL_STATUS_REGISTER], rB

; Show cycle count
    lod rA, [CYCLE_CNT]
    str [PORT_B], rA


    jmp !forever



;--------------------------------------------
; print_string:
; 
; stack argument is the address of the string
;
; stack --> SP RET STR_ADDR

!print_string

; backup used registers
    psh rA
    psh rB

; stack --> SP rB rA RET STR_ADDR

    lod rA, [SP + 4]

!print_next_char
    lod rB, [rA]  ; get the next character in the string
    cmp rB, rZ    ; stop printing when we find the NULL terminator
    je  !done_printing

    str [LCD_CURSOR], rB  ; draw the character
    inc rA                ; move on to the next character

    jmp !print_next_char

!done_printing

; replace STR_ADDR on stack with return address
    lod rB, [SP + 3]
    str [SP + 4], rB

; stack --> SP rB rA RET RET

; restore used registers
    pop rB
    pop rA

; stack --> SP RET RET

; adjust stack for return, it should be just below what will be popped
    add SP, 1
    ret
