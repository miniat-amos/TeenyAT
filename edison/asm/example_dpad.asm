; D-PAD USAGE DEMO
;
; A simple demo to show off the experiment board's d-pad controls.
; Depending on machine speed... THE 'INITIAL_SPEED' constant 
; may have to be altered per machine.
; 
; - John Pertell 11.09.24

.const PORTA_DIR 0x8000
.const PORTA 0x8002

.const LCD_CURSOR 0xA000 
.const LCD_CURSOR_X 0xA00A
.const LCD_CURSOR_Y 0xA00B

.const LEROY_1 0xD1
.const LEROY_2 0xD3
.const INITIAL_SPEED 1700 ; may need to be adjusted per machine

.var flipped_flag 0   

; Designated Registers:
; rA => x-coordinate
; rB => y-coordinate
; rC => char_sprite
; rD => delay_amt
; rE => temp

!init_demo
    ; Set port A to input mode
    set rE, 0xFFFF 
    str [PORTA_DIR], rE

    ; Initialize the delay amount 
    set rD, INITIAL_SPEED

    ; Load initial character sprite
    set rC, LEROY_1         

    ; Zero out the initial coordinates
    set rA, 0                ; x-coordinate
    set rB, 0                ; y-coordinate

    str [flipped_flag], rZ   
    cal !draw_leroy

; Main loop
!demo_loop
    cal !check_for_input
    cal !draw_leroy
    cal !delay
    jmp !demo_loop

; Check for d-pad input on port A and go to corresponding label
!check_for_input
    set rE, 0xFFFF 
    str [PORTA_DIR], rE

    lod rE, [PORTA]         
    and rE, 0xF    ; Bitmask to grab only d-pad buttons

    ; Down button
    cmp rE, 0x8        
    je !down_key_pressed
        
    ; Right button
    cmp rE, 0x4             
    je !right_key_pressed

    ; Up button
    cmp rE, 0x2             
    je !up_key_pressed

    ; Left button
    cmp rE, 0x1             
    je !left_key_pressed

    !key_pressed
    ret                      

; Draws Leroy according to the active x and y coords
; and then also alternates sprites.
!draw_leroy
    ; Set cursor position
    str [LCD_CURSOR_X], rA     ; x-coordinate
    str [LCD_CURSOR_Y], rB     ; y-coordinate

    ; Toggle sprite between LEROY_1 and LEROY_2
    cmp rC, LEROY_1      
    je !use_sprite_two
    !use_sprite_one
        set rC, LEROY_1 
        jmp !char_sprite_selected
    !use_sprite_two
        set rC, LEROY_2  
    !char_sprite_selected

    ; Shall we flip the sprite?
    lod rE, [flipped_flag]
    cmp rE, 0             
    je !draw_normal
    !draw_flipped
        bts rC, 14        ; Set bit 14 to flip the sprite
        str [LCD_CURSOR], rC
        jmp !leroy_drawn
    !draw_normal
        str [LCD_CURSOR], rC

    !leroy_drawn
    cal !delay
    ret

; Delete old sprite at old location.
; Called after key press.
!delete_old_pos
    str [LCD_CURSOR_X], rA
    str [LCD_CURSOR_Y], rB
    str [LCD_CURSOR], rZ
    ret

; Move Leroy down
!down_key_pressed
    cal !delete_old_pos
    inc rB     
    jmp !key_pressed

; Move Leroy to the right and set sprite orientation to the right
!right_key_pressed
    cal !delete_old_pos
    inc rA     
    str [flipped_flag], rZ  
    jmp !key_pressed

; Move Leroy up
!up_key_pressed
    cal !delete_old_pos
    dec rB     
    jmp !key_pressed

; Move Leroy left and set sprite orientation to the left
!left_key_pressed
    cal !delete_old_pos
    dec rA     
    set rE, 1
    str [flipped_flag], rE  
    jmp !key_pressed

; Delay used to slow down the frames
!delay
    set rE, 0                       
    !inner_delay_loop
        inc rE
        dly rE
        cmp rE, rD          
        jl !inner_delay_loop
    ret
