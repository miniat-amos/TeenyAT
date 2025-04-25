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


.const MOUSE_LEFT 1
.const MOUSE_RIGHT 2

.const MUTATEAMT 50
.const COLORAMT 50
    
!main
; load into registers the state of the mouse
    lod rA, [MOUSEX]        
    lod rB, [MOUSEY]
    lod rC, [MOUSEB]

    cmp rC, MOUSE_LEFT          ; left mouse button is down
    je !changecolor_and_paint

    cmp rC, MOUSE_RIGHT         ; middle mouse button is down
    je !mutate

    jmp !main

;---------------------------------
; change color of and draw it at the mouse   
!changecolor_and_paint
    add rD, COLORAMT
    str [X1], rA
    str [Y1], rB
    str [STROKE], rD
    str [POINT], rZ
    str [UPDATE], rZ
    jmp !main

;---------------------------------
; increment the color of all the pixles by MUTATEAMT 
!mutate
    set rA, 0
!loop
    lod rC, [MOUSEB]
    cmp rC, MOUSE_RIGHT 
    jne !main    
    lod rE, [rA + UPDATESCREEN]
    add rE, MUTATEAMT
    str [rA + UPDATESCREEN], rE
    inc rA
    cmp rA, 0x1000
    jg !end
    jmp !loop
!end
    str [UPDATE],rZ
    jmp !main
