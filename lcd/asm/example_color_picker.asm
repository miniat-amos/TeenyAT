;====================================
; Color Picker example
;
; The TeenyAT LCD has a unique color system similar
; to the outer edge of a color wheel, but includes
; both black and white.  Many colors are left out
; (including greys or brown), so this app lets you
; see all the LCD colors and use your mouse to find 
; out its integer value from the output on the terminal.

.const LIVESCREEN 0x8000
.const UPDATESCREEN 0x9000
.const X1 0xD000
.const Y1 0xD001
.const X2 0xD002
.const Y2 0xD003
.const STROKE 0xD010
.const FILL 0xD011
.const DRAWFILL 0xD012
.const DRAWSTROKE 0xD013
.const RAND 0xD700
.const UPDATE 0xE000
.const RECT 0xE010
.const LINE 0xE011
.const POINT 0xE012
.const MOUSEX 0xFFFC
.const MOUSEY 0xFFFD
.const MOUSEB 0xFFFB
.const TERM 0xFFFF
.const KEY 0xFFFE

;;;
;;; Draw all 4K colors on the screen
;;;

    SET rC, 4096           ; 64x64 = 4096
    SET rA, UPDATESCREEN   ; address of next pixel
    SET rB, rZ             ; which color index to render
!draw_next_color
    STR [rA], rB
    INC rA
    INC rB
    LUP rC, !draw_next_color

    STR [UPDATE], rZ

;;;
;;; Show the color code under the mouse to the terminal
;;;

!show_color
    LOD rA, [MOUSEX]
    LOD rB, [MOUSEY]
    MPY rB, 64
    ADD rB, rA + UPDATESCREEN
    LOD rC, [rB]
    STR [TERM], rC 

    JMP !show_color
