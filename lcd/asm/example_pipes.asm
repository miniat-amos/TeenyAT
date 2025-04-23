; TeenyAT Constants
.const PORT_A_DIR   0x8000
.const PORT_B_DIR   0x8001
.const PORT_A       0x8002
.const PORT_B       0x8003
.const RAND         0x8010
.const RAND_BITS    0x8011

; LCD Peripherals
.const LIVESCREEN 0x9000
.const UPDATESCREEN 0xA000
.const X1 0xD000
.const Y1 0xD001
.const X2 0xD002
.const Y2 0xD003
.const STROKE 0xD010
.const FILL 0xD011
.const DRAWFILL 0xD012
.const DRAWSTROKE 0xD013
.const UPDATE 0xE000
.const RECT 0xE010
.const LINE 0xE011
.const POINT 0xE012
.const MOUSEX 0xFFFC
.const MOUSEY 0xFFFD
.const MOUSEB 0xFFFB
.const TERM 0xFFFF
.const KEY 0xFFFE

.const AMT 17

!main
; set the random end of a nonexistent previous line
    cal !rand_rA_-10...+10
    str [X2], rA
    cal !rand_rA_-10...+10
    str [Y2], rA

    xor rC, rC   ; start with color 0

!next_line
; make the previous end of line the start of the next line
    lod rA, [X2]
    str [X1], rA
    lod rA, [Y2]
    str [Y1], rA

; new random end point
    cal !rand_rA_-10...+10
    lod rB, [X2]
    add rA, rB

    cmp rA, 0
    jge !x>=0
    set rA, 0

!x>=0
    cmp rA, 63
    jle !x<=63
    set rA, 63
    
!x<=63
    str [X2], rA

    cal !rand_rA_-10...+10
    lod rB, [Y2]
    add rA, rB

    cmp rA, 0
    jge !y>=0
    set rA, 0

!y>=0
    cmp rA, 63
    jle !y<=63
    set rA, 63

!y<=63
    str [Y2], rA

; set color and draw the line
    str [STROKE], rC
    str [LINE], rZ
    str [UPDATE], rZ

    add rC, AMT   ; go to next color
    jmp !next_line

;--------------------
; generates a random number between -10 and 10
; stores the value in rA
!rand_rA_-10...+10
    lod rA, [RAND]
    mod rA, 21
    sub rA, 10
    ret
