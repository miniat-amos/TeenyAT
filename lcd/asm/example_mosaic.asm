; Hit key A to draw new mosaic
; Hit key S to mutate mosaic

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

.const MUTATEAMT 50
.const COLORAMT 50
.const AMT 100

!main
    jmp !draw_loop

!main_loop
    set rD, rZ
    str [UPDATE], rZ

!key_loop
    lod rA, [KEY]
    cmp rA, 'A'                      ; 'A' key is pressed
    je !draw_loop
    cmp rA, 'S'                      ; 'S' key is pressed
    je !mutate
    jmp !main_loop

!draw_loop
; get random x1 and y1 values keeping in bounds
    lod rA, [RAND_BITS]
    mod rA, 64
    lod rB, [RAND_BITS]
    mod rB, 64
    lod rC, [RAND]                   ; get random stroke color
    str [STROKE], rC

    str [X1], rA
    str [Y1], rB

; get random x2 and y2 values in bounds
    lod rA, [RAND_BITS]
    mod rA, 64
    lod rB, [RAND_BITS]
    mod rB, 64

    lod rC, [RAND]                   ; get random fill color 
    str [FILL], rC

    str [X2], rA
    str [Y2], rB
    str [RECT], rZ                   ; blit rectangle to update buffer

    inc rD
    cmp rD, AMT
    jge !main_loop  

    jmp !draw_loop

; increment the color of all the pixles MUTATEAMT
!mutate
    set rA, 0
!mutate_loop
    lod rE, [rA + UPDATESCREEN]
    add rE, MUTATEAMT
    str [rA + UPDATESCREEN], rE
    inc rA
    cmp rA, 0x1000
    jg !end_mutate
    jmp !mutate_loop
!end_mutate
    str [UPDATE],rZ
    jmp !main_loop