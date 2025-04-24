;    ___  ___                       _____ _          _     _                       
;   |  \/  |                       /  __ \ |        (_)   | |                      
;   | .  . | ___ _ __ _ __ _   _  | /  \/ |__  _ __ _ ___| |_ _ __ ___   __ _ ___ 
;   | |\/| |/ _ \ '__| '__| | | | | |   | '_ \| '__| / __| __| '_ ` _ \ / _` / __|
;   | |  | |  __/ |  | |  | |_| | | \__/\ | | | |  | \__ \ |_| | | | | | (_| \__ \
;   \_|  |_/\___|_|  |_|  \__, |  \____/_| |_|_|  |_|___/\__|_| |_| |_|\__,_|___/
;                          __/ |                                                 
;                         |___/              
;
; Author: Noah Breedy
; Description: Falling sand simulation to demo with use of our teenyAT lcd system

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


; some standard lcd colors
.const BROWN 3900                   ; this is not actually brown lol
.const WHITE 2561
.const RED 3585
.const GREEN 1537
.const LIGHTGREEN 1796
.const YELLOW 2116
.const LAVENDER 2689

; the biggest possible pixel index we can have
.const BOTTOMSCREEN 0xB000          ; UPDATESCREEN + 0x1000


!main
    cal !draw_trunk
!main_loop
    cal !draw_lights                ; draw the lights of the tree
    cal !gen_snow                   ; randomly spawn new white pixels
    cal !update_snow                ; move white pixels down the screen
    str [UPDATE], rZ
    jmp !main_loop

;-------------------------------------
; draw a random white pixel along the top edge of the screen
!gen_snow
    lod rA, [RAND]
    mod rA, 63
    inc rA                          ; prevents zero since our loop dosent handle it       
    set rB, WHITE
    str [STROKE], rB
    str [X1], rA
    str [Y1], rZ
    str [POINT], rZ
    ret

;-------------------------------------
!update_snow
    set rA, 0x1000                  ; have rA start at the bottom of the screen
!snow_loop
    lod rB, [rA + UPDATESCREEN]     ; load in the color of the pixel 

    cmp rB, WHITE                   ; check if its a snow pixel 
    jne !snow_loop_bottom
        
    set rC, rA + UPDATESCREEN       ; this is the current pixel index were at

    add rC, 64                      ; adding 64 means this is the the pixel index directly below
    
    cmp rC, BOTTOMSCREEN            ; stop falling if index is at the bottom of the screen
    jge !snow_loop_bottom

    lod rD, [rC + 0]                ; check if the lower pixel is clear (LAVENDER)
    cmp rD, LAVENDER 

; randomly fall if space is clear else attempt to "slide" down slopes
    je !snow_shimmy_random                     
    jmp !snow_shimmy_check           
           
!snow_shimmy_return
    set rE, WHITE
    str [STROKE], rE
    str [rC + 0], rE                ; draw new white pixel 

    set rE, LAVENDER
    str [STROKE], rE
    str [rA + UPDATESCREEN], rE     ; clear old pixel

!snow_shimmy_fail
!snow_loop_bottom
    lup rA, !snow_loop              ; loop until rA reaches zero
    ret
        
; randomly attempt to move the snow to the pixel 1 to the left or right
!snow_shimmy_random
    lod rE, [RAND]
    mod rE, 2

    cmp rE, 0
    je !snow_shimmy_right

    jmp !snow_shimmy_left

!snow_shimmy_right
    inc rC                          ; increment rC means we move one pixel to the right
    lod rD, [rC + 0]
    cmp rD, LAVENDER                ; check if its valid location
    je !snow_shimmy_return
    dec rC                          ; revert our changes (just fall down)
    jmp !snow_shimmy_return

!snow_shimmy_left
    dec rC                          ; decrement rC means we move one pixel to the left
    lod rD, [rC + 0]
    cmp rD, LAVENDER                ; check if its valid location
    je !snow_shimmy_return
    inc rC                          ; revert our changes (just fall down)
    jmp !snow_shimmy_return

; attempt to move either 1 to the left or to the right       
!snow_shimmy_check
    inc rC
    lod rD, [rC + 0]
    cmp rD, LAVENDER 
    je !snow_shimmy_return
    dec rC
    
    dec rC
    lod rD, [rC + 0]
    cmp rD, LAVENDER 
    je !snow_shimmy_return
    inc rC
    jmp !snow_shimmy_fail

;-------------------------------------      
!set_light_yellow
    set rD, YELLOW
    str [STROKE], rD
    jmp !slide_end

!set_light_green
    set rD, LIGHTGREEN
    str [STROKE], rD
    jmp !slide_end

!set_light_red
    set rD, RED
    str [STROKE], rD
    jmp !slide_end

!draw_trunk
    set rA, 28
    set rB, 44
    str [X1], rA
    str [Y1], rB
    set rA, 35
    set rB, 63
    str [X2], rA
    str [Y2], rB

    set rA, BROWN
    str [DRAWSTROKE], rZ
    str [FILL], rA
    str [RECT], rZ
    ret

; the draw lights function works by first drawing
; dark green lines or smaller and smaller length then moving up 
; it then after goes over those same spots and draws a random light color 
!draw_lights
    set rC, 0
!lights_loop
    set rD, 0
    set rE, 0
    set rA, 18
    add rA, rC
    set rB, 44
    sub rB, rC
    str [Y1], rB
    set rB, 45
    sub rB, rC
!slide_loop
            
    set rE, rA
    mod rE, 2
    cmp rE, 0
    set rD, GREEN
    str [STROKE], rD
    je !slide_end

    lod rE, [RAND]
    mod rE, 4

    cmp rE, 0
    je !set_light_yellow

    cmp rE, 1
    je !set_light_green

    cmp rE, 2
    je !set_light_red

!slide_end
    str [X1], rA
    str [POINT], rZ
    dly 500
    inc rA
    cmp rA, rB
    jle !slide_loop
    
    inc rC
    cmp rC, 15              
    jl !lights_loop
    ret