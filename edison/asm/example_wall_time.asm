; _ _ _ ____ _    _       ___ _ _  _ ____
; | | | |__| |    |        |  | |\/| |___
; |_|_| |  | |___ |___     |  | |  | |___
;
; Description:
; Demonstrates the TeenyAT "Wall Time" internal peripheral.
; The Edison Port B 7-segment display shows the seconds elapsed
; since the program started or wall time was reset.
;
; Controls:
; Press 'R' (Port A, Pin 12) to reset the timer to zero.
;
; As a bonus, this demo contains the sound of Leroy gargling.

; TeenyAT Constants
.const PORT_A_DIR      0x8000
.const PORT_B_DIR      0x8001
.const PORT_A          0x8002
.const PORT_B          0x8003
.const RAND            0x8010
.const RAND_BITS       0x8011
.const CYCLE_CNT       0x8090
.const CYCLE_CNT_RESET 0x8091
.const WALL_TIME       0x8094
.const WALL_TIME_RESET 0x8095

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


    ; Skip over data to !main
    jmp !main

!info1
.raw "7-Seg counts seconds"

!info2
.raw "'R' to reset time"

!main

; Port A, pin 12 (the 'R' key) is button input
; Port B bits are all output

    set rA, R_BUTTON_MASK
    str [PORT_A_DIR], rA
    str [PORT_B_DIR], rZ

; First info string at (0,0)

    str [LCD_CURSOR_XY], rZ
    psh !info1
    cal !print_string

; Second info string at (2,0)
    str [LCD_CURSOR_X], rZ
    set rA, 2
    str [LCD_CURSOR_Y], rA
    psh !info2
    cal !print_string

!show_time
    lod rA, [WALL_TIME]  ; get the time since reset in 16ths of a second
    shr rA, 4            ; get rid of the fractions of a second
    str [PORT_B], rA     ; display second count in the bottom 7-segment display

; rD (initially zero) holds the previous number of seconds displayed.  If
; the new second-count is suddenly different, this means the next second
; has arrived, so we'll play a little bleep... kind of like a clock ticking.

    cmp rA, rD
    je  !post_second_bleep
    cal !bleep
!post_second_bleep

; remember the time in seconds we're displaying
    set rD, rA

; Check whether to reset the wall time base

    lod rA, [PORT_A]
    and rA, R_BUTTON_MASK
    cmp rA, rZ
    je  !show_time

; Play another bleep while the 'R' button is pressed

    str [WALL_TIME_RESET], rZ
    cal !bleep


    jmp !show_time



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



;---------------------------
; bleep:
; make a pleasant short robo-bleep

!bleep

; backup used registers
    psh rA
    psh rB
    psh rC
    psh rD

;;;
;;; make rA have exactly 2 bits in positions 6 through 11
;;;

; random 1 in bits 9 through 11
    lod rB, [RAND]
    mod rB, 3
    set rA, rZ
    bts rA, rB + 9

; random 1 in bits 6 through 8
    lod rC, [RAND]
    mod rC, 3
    set rB, rZ
    bts rB, rC + 6

; put the two bits together in rA
    or  rA, rB

;;;
;;; make rB have exactly 2 bits in positions 6 through 11
;;;

; random 1 in bits 9 through 11
    lod rC, [RAND]
    mod rC, 3
    set rB, rZ
    bts rB, rC + 9

; random 1 in bits 6 through 8
    lod rD, [RAND]
    mod rD, 3
    set rC, rZ
    bts rC, rD + 6

; put the two bits together in rB
    or  rB, rC

;;;
;;; Trill each pitch in rA and rB down quickly
;;;
    set rC, 16
!loop
    shl rA, 3
    div rA, 9
    shl rB, 3
    div rB, 9
    str [BUZZER_LEFT], rA
    bts rB, 15
    str [BUZZER_RIGHT], rB
    btc rB, 15
    dly 8000
    lup rC, !loop

; pitches are zero (or close), ensure sound is off
    str [BUZZER_LEFT], rZ
    str [BUZZER_RIGHT], rZ


; restore used registers
    pop rD
    pop rC
    pop rB
    pop rA

    ret