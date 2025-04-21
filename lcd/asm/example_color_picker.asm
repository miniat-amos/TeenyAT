; ====================================
; Color Picker example
;
; The TeenyAT LCD has a unique color system similar
; to the outer edge of a color wheel, but includes
; both black and white.  Many colors are left out
; (including greys or brown), so this app lets you
; see all the LCD colors and use your mouse to find 
; out its integer value from the output on the terminal.

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
.const TERM 0xFFFF
.const KEY 0xFFFE


; draw all 4K colors on the screen
!main
    set rC, 4096           ; 64x64 = 4096
    set rA, UPDATESCREEN   ; address of next pixel
    set rB, rZ             ; which color index to render
!draw_next_color
    str [rA], rB
    inc rA
    inc rB
    lup rC, !draw_next_color

    str [UPDATE], rZ

; show the color code under the mouse to the terminal
!show_color
    lod rA, [MOUSEX]
    lod rB, [MOUSEY]
    mpy rB, 64
    add rB, rA + UPDATESCREEN
    lod rC, [rB]
    str [TERM], rC 

    jmp !show_color
