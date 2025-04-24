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

.const COLORAMT 50

; rA = X
; rB = Y
; rC = KEY_HIT

!main
    set rA, 20
    set rB, 20
    set rC, 0

!main_loop
    lod rD, [KEY]            ; load in keyboard input
  
; player movement code
    cmp rD, 'W'
    je !UP

    cmp rD, 'S'
    je !DOWN

    cmp rD, 'A'
    je !LEFT

    cmp rD, 'D'
    je !RIGHT

!end_movement
    add rE, COLORAMT

    str [X1], rA
    str [Y1], rB
    str [STROKE], rE
    str [POINT], rZ
    str [UPDATE], rZ

    jmp !main_loop

;-----------------------------
!UP
    cmp rB, 0
    jle !end_movement
    dec rB
    jmp !end_movement

;-----------------------------
!DOWN
    cmp rB, 63              ; the highest possible y value
    jge !end_movement
    inc rB
    jmp !end_movement

;-----------------------------
!LEFT
    cmp rA, 0
    jle !end_movement
    dec rA
    jmp !end_movement

;-----------------------------
!RIGHT
    cmp rA, 63              ; the highest possible x value
    jge !end_movement
    inc rA
    jmp !end_movement