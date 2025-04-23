; _________  ________  ________  ___________ _______________________________   _____   ____  __._____________________ 
; \_   ___ \ \_____  \ \______ \ \_   _____/ \______   \______   \_   _____/  /  _  \ |    |/ _|\_   _____/\______   \
; /    \  \/  /   |   \ |    |  \ |    __)_   |    |  _/|       _/|    __)_  /  /_\  \|      <   |    __)_  |       _/
; \     \____/    |    \|    `   \|        \  |    |   \|    |   \|        \/    |    \    |  \  |        \ |    |   \
;  \______  /\_______  /_______  /_______  /  |______  /|____|_  /_______  /\____|__  /____|__ \/_______  / |____|_  /
;         \/         \/        \/        \/          \/        \/        \/         \/        \/        \/         \/ 
; 
; Author: Noah Breedy
;  
; Description: Use your code breaking skills to guess the sequence
;              and prevent the bomb from exploding and destroying
;              the edison board forever!
;
; Controls:    Just press the arrow keys if easy mode
;              And the port A buttons as well if hard mode
;


.const PORT_A_DIR 0x8000
.const PORT_B_DIR 0x8001
.const PORT_A 0x8002
.const PORT_B 0x8003
.const RAND 0x8010
.const RAND_BITS 0x8011

.const LCD_CURSOR 0xA000 
.const LCD_CLEAR_SCREEN 0xA001
.const LCD_MOVE_LEFT 0xA002  
.const LCD_MOVE_RIGHT 0xA003
.const LCD_MOVE_UP 0xA004
.const LCD_MOVE_DOWN 0xA005
.const LCD_MOVE_LEFT_WRAP 0xA006  
.const LCD_MOVE_RIGHT_WRAP 0xA007
.const LCD_MOVE_UP_WRAP 0xA008
.const LCD_MOVE_DOWN_WRAP 0xA009
.const LCD_CURSOR_X 0xA00A
.const LCD_CURSOR_Y 0xA00B
.const LCD_CURSOR_XY 0xA00C
.const BUZZER_LEFT 0xA010
.const BUZZER_RIGHT 0xA011
.const FADER_LEFT 0xA020
.const FADER_RIGHT 0xA021

.const SOUND_F5 0x02BA
.const SOUND_G5 0x030F 
.const SOUND_A5 0x0370
.const SOUND_B5 0x03DB

.const Q_KEY 0x8000
.const W_KEY 0x4000
.const E_KEY 0x2000
.const R_KEY 0x1000
.const A_KEY 0x800
.const S_KEY 0x400
.const D_KEY 0x200
.const F_KEY 0x100
.const DOWN_KEY 0x8
.const RIGHT_KEY 0x4
.const UP_KEY 0x2
.const LEFT_KEY 0x1

.const KILL_LOOP_DELAY_COUNT 10
.const TIMER_REDUCTION 14 ; How much time to take off timer after success

.const START_SCREEN_TEXT_LOCATION_0 0x0400
.const START_SCREEN_TEXT_LOCATION_1 0x0502
.const START_SCREEN_TEXT_LOCATION_2 0x0003

.const GAME_SCREEN_TEXT_LOCATION_0 0x1300
.const GAME_SCREEN_TEXT_LOCATION_1 0x0501

.const WEIRD_OF_SQUIGGLES 451
.const CLOCK_SPRITE 460

.const ARROW_HEAD_SPRITE 409
.const ARROW_TRAIL_SPRITE 406
.const ARROW_END_SPRITE 405

.const BOMB_SPRITE  469

.const CHECK_SPRITE 494
.const WRONG_SPRITE 495

jmp !init

.var ARROW_LOCATION 1
.var CURRENT_GUESS 0
.var BUFFER_TIMER_THRESHOLD 0x800 ; How long it takes for arrow to move
.var RESORCEFUL_WAIT_TIMER 2      ; I hate this (need to implement interrupts)
.var SECRET_CODE_RANGE 5          ; easy mode -> 5   ; hard mode -> 13
.var PLAYER_SCORE 0 
;---------------- SPLASH SCREEN  ----------------

!init
    set rA, 0xFF0F
    str [PORT_A_DIR], rA
!start_screen

    cal !start_screen_reset

    set rB, !CODE_BREAKER_str
    cal !print_string_rB
    
    set rB, START_SCREEN_TEXT_LOCATION_1  
    str [LCD_CURSOR_XY], rB

    set rB, !Press_ANY_str
    cal !print_string_rB

    set rB, START_SCREEN_TEXT_LOCATION_2
    str [LCD_CURSOR_XY], rB

    set rC, WEIRD_OF_SQUIGGLES
    set rA, 20
    set rB, 1

!squiggle_loop
    str [LCD_CURSOR], rC
    lup rA, !squiggle_loop
    
    cal !wait_for_button_push

    cal !game_screen_reset

!code_breaker_game_init
    set rA, rZ   ; Index of Current Guess
    set rD, rZ   ; This is our timer variable (DONT TOUCH)
    set rE, rZ   ; Timer Buffer Index  (DONT TOUCH)
!code_breaker_game_loop

    cal !render_current_guess
    
    lod rA, [CURRENT_GUESS]
    add rA, rA
    add rA, 5   ; This is done so that the underscore is drawn at the right place
    str [LCD_CURSOR_X], rA 
    set rA, 2
    str [LCD_CURSOR_Y], rA
    set rA, '_'
    str [LCD_CURSOR], rA

;--------- BUTTON PRESS LOGIC ---------------
    cal !get_single_button_push_rB
    cmp rB, rZ
    je !no_input

    lod rA, [CURRENT_GUESS]
    str [rA + !PLAYER_GUESSES], rB
    set rB, 2
    add rA, rA
    add rA, 5
    str [LCD_CURSOR_X], rA
    str [LCD_CURSOR_Y], rB
    str [LCD_CURSOR], rZ
    lod rA, [CURRENT_GUESS] ; just god awful logic
    inc rA
    mod rA, 5
    str [CURRENT_GUESS], rA
    
    set rB, SOUND_F5
    str [BUZZER_LEFT], rB
    set rB, 100
    cal !wait_rB
    str [BUZZER_LEFT], rZ

    cmp rA, rZ  ; Last string check_our code
    jne !lets_wait
    cal !render_current_guess
    cal !check_player_code

!lets_wait
    cal !resourceful_wait
   
;-------------------------------------------
!no_input
    jmp !buffered_timer  ; This is our scuffed timer code.....

!game_over

    set rA, 500
    str [BUZZER_LEFT], rA
    set rA, 500
    bts rA, 15
    str [BUZZER_RIGHT], rA
    set rB, 100
    cal !wait_rB

    set rC, 500
!explosion_loop
    lod rA, [RAND]
    str [LCD_CURSOR_XY], rA
    mod rA, 10
    cmp rA, 5
        
    jle !draw_0
    set rB, '1'
    jmp !draw_explosion
!draw_0
    set rB, '0'

!draw_explosion
    str [LCD_CURSOR], rB

    set rB, 20
    cal !wait_rB

    lup rC, !explosion_loop

    str [BUZZER_LEFT], rZ
    str [BUZZER_RIGHT], rZ

    set rA, START_SCREEN_TEXT_LOCATION_1
    str [LCD_CLEAR_SCREEN], rZ
    str [LCD_CURSOR_XY], rA

    set rB, !Game_Over_str
    cal !print_string_rB

!game_over_loop
    jmp !game_over_loop

;------------- UTILITY SECTION ------------------
!render_current_guess
    psh rB
    psh rC
    set rC, rZ

    set rB, GAME_SCREEN_TEXT_LOCATION_1
    str [LCD_CURSOR_XY], rB

!guess_rendering_loop
    lod rB, [rC + !PLAYER_GUESSES]
    lod rB, [rB + !CHARACTER_CODES_MAP]
    str [LCD_CURSOR], rB
    set rB, 1
    str [LCD_MOVE_RIGHT], rB
    inc rC
    cmp rC, 4
    jg !end_guess_rendering
    jmp !guess_rendering_loop

!end_guess_rendering
    pop rB
    pop rC
    ret

!wait_till_port_A_zero
    psh rB
    set rB, 10
    cal !wait_rB
    pop rB
    ret

!print_string_rB
    psh rB
    psh rA

!print_string_rB_loop
    lod rA, [rB]
    cmp rA, rZ
    je !print_string_rB_terminated

    str [LCD_CURSOR], rA
    inc rB
    jmp !print_string_rB_loop

!print_string_rB_terminated
    pop rA
    pop rB
    ret

!start_screen_reset
    psh rA
    set rA, START_SCREEN_TEXT_LOCATION_0
    str [LCD_CLEAR_SCREEN], rZ
    str [LCD_CURSOR_XY], rA
    pop rA
    ret

!game_screen_reset
    psh rA
    str [LCD_CLEAR_SCREEN], rZ
    str [LCD_CURSOR_XY], rZ
    set rA, CLOCK_SPRITE
    str [LCD_CURSOR], rA
    set rA, ARROW_HEAD_SPRITE
    str [LCD_CURSOR], rA
    set rA, GAME_SCREEN_TEXT_LOCATION_0
    str [LCD_CURSOR_XY], rA
    set rA, BOMB_SPRITE
    str [LCD_CURSOR], rA
    pop rA
    ret

!render_game_arrow
    psh rA
    psh rB
    str [LCD_CURSOR_Y], rZ
    lod rA, [ARROW_LOCATION]
    str [LCD_CURSOR_X], rA
    cmp rA, 1
    jne !not_base_arrow
    set rB, ARROW_END_SPRITE
    str [LCD_CURSOR], rB
    jmp !render_arrow_end_return
!not_base_arrow
    set rB, ARROW_TRAIL_SPRITE
    str [LCD_CURSOR], rB
!render_arrow_end_return
    inc rA
    cmp rA, 19
    jge !game_over
    str [ARROW_LOCATION], rA
    set rB, ARROW_HEAD_SPRITE
    str [LCD_CURSOR], rB

!blank_front
    inc rA
    cmp rA, 18
    jg !done_blanking
    str [LCD_CURSOR], rZ
    jmp !blank_front
!done_blanking
    pop rA
    pop rB
    ret

; OHHHHH BABYYYYY
!resourceful_wait
    psh rA
    psh rB
    set rA, rZ
!resourceful_wait_loop
    lod rB, [PORT_A]
    cmp rB, rZ
    je !end_resorceful_wait
    inc rA
    cmp rA, 0xFFE 
    jge !check_values_resorceful_wait
    jmp !resourceful_wait_loop
!check_values_resorceful_wait
    lod rA, [ RESORCEFUL_WAIT_TIMER ]
    cmp rA, 0x8  
    jge !end_resorceful_wait
    inc rA
    str [RESORCEFUL_WAIT_TIMER], rA
    set rA, rZ
    jmp !resourceful_wait_loop
!end_resorceful_wait
    str [RESORCEFUL_WAIT_TIMER], rZ
    pop rA
    pop rB
    ret

!wait_rB
    psh rB
!wait_rB_loop
    cal !kill_loop
    lup rB, !wait_rB_loop
    pop rB
    ret

!kill_loop
;----- Edit this if your thing is tooo slowwww
    psh rA
    set rA, 0
!delay_loop
    dly KILL_LOOP_DELAY_COUNT
    inc rA
    cmp rA, 10
    jl !delay_loop
    pop rA
    ret

!wait_for_button_push
    psh rA
!wait_for_button_push_loop
    lod rA, [PORT_A]
    cmp rA, rZ
    je !wait_for_button_push_loop
!wait_for_button_push_zero
    lod rA, [PORT_A]
    cmp rA, rZ
    jne !wait_for_button_push_zero
    pop rA
    ret
;----------------- RETURNS FIRST ONE IN PORT_A ---------------------------------
!get_single_button_push_rB
    psh rA
    psh rC
    set rC, rZ
    set rB, rZ
    lod rA, [PORT_A]

    cmp rA, rZ
    je !get_single_button_push_return
    
!get_single_button_push_loop
    set rB, rA
    and rB, 1
    jne  !get_single_button_push_calculate   ; was bit in value move on calculate value
    shf rA, 1
    inc rC
    jmp !get_single_button_push_loop
            
!get_single_button_push_calculate
    set rB, 1
    neg rC
    shf rB, rC
;--------------- NORMALIZE TO KEY MAP -------------------------------------
!get_single_button_push_return
    set rC, -1
!key_map_loop
    inc rC
    lod rA, [rC + !CHARACTER_CODES_INDEX]
    cmp rA, rB
    jne !key_map_loop
    set rB, rC
    pop rA
    pop rC
    ret
;---------------- Scaled Timersssss ----------------------
!buffered_timer
    inc rD
    psh rA
    lod rA, [BUFFER_TIMER_THRESHOLD]
    cmp rD, rA 
    pop rA
    jne !code_breaker_game_loop
!check_timer_value
    lod rD, [rE + !TIMER_BUFFER]
    cmp rD, -1
    je !reset_timers_render_arrow
    inc rD
    cmp rD, 0xFFFF
    jge !shift_timer_buffer
    str [rE + !TIMER_BUFFER ], rD
    set rD, rZ
    jmp !code_breaker_game_loop

!shift_timer_buffer
    str [rE + !TIMER_BUFFER], rZ
    inc rE
    set rD, rZ
    jmp !code_breaker_game_loop

!reset_timers_render_arrow
    set rE, 0
    set rD, rZ
    cal !render_game_arrow
    jmp !code_breaker_game_loop
;------------------------------------------

!check_player_code
    psh rA
    psh rB
    psh rC
    psh rD
    set rD, rZ
    set rA, 3
    str [LCD_CURSOR_Y], rA
    set rA, rZ

!check_player_code_loop
    lod rB, [rA + !SECRET_CODE ]
    lod rC, [rA + !PLAYER_GUESSES ]

    cmp rB, rC

    jne !set_sprite_wrong
    set rB, CHECK_SPRITE
    inc rD
    jmp !draw_result_sprite

!set_sprite_wrong
    set rB, WRONG_SPRITE
!draw_result_sprite
    set rC, rA
    add rC, rC
    add rC, 5
    str [LCD_CURSOR_X], rC
    str [LCD_CURSOR], rB

    inc rA
    cmp rA, 5
    je !end_player_check
    jmp !check_player_code_loop

!end_player_check
    cmp rD, 5
    jne !its_wrong

    cal !generate_new_secret_code
    jmp !return_from_check

!its_wrong
    set rB, 400
    bts rB, 15
    str [BUZZER_LEFT], rB
    set rB, 1000
    cal !wait_rB
    str [BUZZER_LEFT], rZ

    set rB, SOUND_G5
    str [BUZZER_LEFT], rB
    set rB, 2000
    cal !wait_rB
    str [BUZZER_LEFT], rZ

!return_from_check
    pop rD
    pop rA
    pop rB
    pop rC
    ret

!generate_new_secret_code
    psh rA
    psh rB
    psh rC

; This is how we increase the speed of the timer
    lod rB, [BUFFER_TIMER_THRESHOLD]
    cmp rB, 148 ; This value was calulated using python reaches max speed around score of 10
    jle !skip_arrow_subtraction
    sub rB, 100
    str [BUFFER_TIMER_THRESHOLD], rB
    
!skip_arrow_subtraction
    set rB, SOUND_F5
    str [BUZZER_LEFT], rB
    set rB, 1000
    cal !wait_rB
    str [BUZZER_LEFT], rZ

    set rB, SOUND_G5
    str [BUZZER_LEFT], rB
    set rB, 2000
    cal !wait_rB
    str [BUZZER_LEFT], rZ

    set rB, SOUND_A5
    str [BUZZER_LEFT], rB
    set rB, 1000
    cal !wait_rB
    str [BUZZER_LEFT], rZ

    set rB, SOUND_B5
    str [BUZZER_LEFT], rB
    set rB, 2000
    cal !wait_rB
    str [BUZZER_LEFT], rZ

    set rA, 3
    str [LCD_CURSOR_Y], rA
    str [LCD_CURSOR_X], rA
    set rA, 16

!clear_3rd_row
    str [LCD_CURSOR], rZ
    lup rA, !clear_3rd_row
    

    set rA, 1
    str [ARROW_LOCATION], rA

    set rA, rZ
!generate_new_secret_code_loop

    lod rB, [RAND]
    lod rC, [SECRET_CODE_RANGE]
    mod rB, rC 
    cmp rB, rZ
    jg !K
    set rB, 1
!K 
    cmp rB, rC
    jl !M
    set rB, rC
    dec rB
!M
    str [rA + !PLAYER_GUESSES], rZ ; clear old guess
    str [rA + !SECRET_CODE], rB    ; put new secret code

    inc rA
    cmp rA, 5
    jne !generate_new_secret_code_loop
    
    lod rA, [PLAYER_SCORE]
    inc rA
    str [PLAYER_SCORE], rA
    str [PORT_B], rA

    pop rC
    pop rB
    pop rA
    ret

;------------- DATA SECTION ---------------------
!SECRET_CODE
.raw 2 4 2 4 2

!PLAYER_GUESSES
.raw 0 0 0 0 0

!CHARACTER_CODES_INDEX
.raw 0 LEFT_KEY UP_KEY RIGHT_KEY DOWN_KEY F_KEY D_KEY S_KEY A_KEY R_KEY E_KEY W_KEY Q_KEY 

!CHARACTER_CODES_MAP
.raw '?' 16793 8605 409 413 'F' 'D' 'S' 'A' 'R' 'E' 'W' 'Q'

!TIMER_BUFFER
.raw 0 0 0 -1

!CODE_BREAKER_str
.raw 'C''O''D''E'' ''B''R''E''A''K''E''R' 0

!Press_ANY_str
.raw 'P''r''e''s''s'' ''A''N''Y' 0

!Game_Over_str
.raw 'G''a''m''e'' ''O''v''e''r'' '':''(' 0