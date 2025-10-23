;  _   _                           ____  _      _   _         _             
; | | | | __ _ _ __  _ __  _   _  | __ )(_)_ __| |_| |__   __| | __ _ _   _ 
; | |_| |/ _` | '_ \| '_ \| | | | |  _ \| | '__| __| '_ \ / _` |/ _` | | | |
; |  _  | (_| | |_) | |_) | |_| | | |_) | | |  | |_| | | | (_| | (_| | |_| |
; |_| |_|\__,_| .__/| .__/ \__, | |____/|_|_|   \__|_| |_|\__,_|\__,_|\__, |
;             |_|   |_|    |___/                                      |___/ 
;
; Composer: John Pertell

; TeenyAT Constants
.const PORT_A_DIR 0x8000
.const PORT_B_DIR 0x8001
.const PORT_A 0x8002
.const PORT_B 0x8003
.const RAND 0x8010
.const RAND_BITS 0x8011

; Edison Constants
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

; Buzzer Note Constants
.const SOUND_C4     0x0105  ; C4 (261 Hz)
.const SOUND_CSHARP4 0x0115 ; C#4 (277 Hz)
.const SOUND_D4     0x0125  ; D4 (293 Hz)
.const SOUND_DSHARP4 0x0137 ; D#4 (311 Hz)
.const SOUND_E4     0x0149  ; E4 (329 Hz)
.const SOUND_F4     0x015D  ; F4 (349 Hz)
.const SOUND_FSHARP4 0x0171 ; F#4 (369 Hz)
.const SOUND_G4     0x0188  ; G4 (392 Hz)
.const SOUND_GSHARP4 0x019F ; G#4 (415 Hz)
.const SOUND_A4     0x01B8  ; A4 (440 Hz)
.const SOUND_ASHARP4 0x01D2 ; A#4 (466 Hz)
.const SOUND_B4     0x01ED  ; B4 (494 Hz)
.const SOUND_C5     0x020B  ; C5 (523 Hz)
.const SOUND_CSHARP5 0x022A ; C#5 (554 Hz)
.const SOUND_D5     0x024B  ; D5 (587 Hz)
.const SOUND_DSHARP5 0x026E ; D#5 (622 Hz)
.const SOUND_E5     0x0293  ; E5 (659 Hz)
.const SOUND_F5     0x02BA  ; F5 (698 Hz)
.const SOUND_FSHARP5 0x02E3 ; F#5 (739 Hz)
.const SOUND_G5     0x030F  ; G5 (783 Hz)
.const SOUND_GSHARP5 0x033E ; G#5 (830 Hz)
.const SOUND_A5     0x0370  ; A5 (880 Hz)
.const SOUND_ASHARP5 0x03A4 ; A#5 (932 Hz)
.const SOUND_B5     0x03DB  ; B5 (987 Hz)
.const SOUND_C6     0x0416  ; C6 (1046 Hz)

.const DELAY_TEENY  750
.const DELAY_SHORT  1000
.const DELAY_LONG   1500
.const WAIT_LONG    3500

    jmp !init

!happy_birthday_str
.raw "Happy Birthday"

!init
    set rB, !happy_birthday_str
    cal !print_string_rB

!main
    set rA, SOUND_G4  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_G4  
    str [BUZZER_RIGHT], rA
    cal !delay_teeny

    set rA, SOUND_A4  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_G4  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_C5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_B4  
    str [BUZZER_RIGHT], rA
    cal !delay_long

    set rA, SOUND_G4  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_G4  
    str [BUZZER_RIGHT], rA
    cal !delay_teeny

    set rA, SOUND_A4  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_G4  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_D5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_C5  
    str [BUZZER_RIGHT], rA
    cal !delay_long

    set rA, SOUND_G4  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_G4  
    str [BUZZER_RIGHT], rA
    cal !delay_teeny

    set rA, SOUND_G5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_E5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_C5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_B4  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_A4  
    str [BUZZER_RIGHT], rA
    cal !delay_long

    set rA, SOUND_F5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_F5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_E5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_C5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_D5  
    str [BUZZER_RIGHT], rA
    cal !delay_short

    set rA, SOUND_C5  
    str [BUZZER_RIGHT], rA
    cal !delay_long

    str [BUZZER_RIGHT], rZ
    cal !wait_long

    jmp !main

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

!delay_teeny
    set rD, DELAY_TEENY
    set rE, 0                       
!inner_delay_teeny
    inc rE
    dly rE
    cmp rE, rD 
    jl !inner_delay_teeny
    set rE, 0
    ret

!delay_short
    set rD, DELAY_SHORT
    set rE, 0                       
!inner_delay_short
    inc rE
    dly rE
    cmp rE, rD 
    jl !inner_delay_short
    set rE, 0
    ret

!delay_long
    set rD, DELAY_LONG
    set rE, 0                       
!inner_delay_long
    inc rE
    dly rE
    cmp rE, rD 
    jl !inner_delay_long
    set rE, 0
    ret

!wait_long
    set rD, WAIT_LONG
    set rE, 0                       
!inner_wait_long
    inc rE
    dly rE
    cmp rE, rD 
    jl !inner_wait_long
    set rE, 0
    ret
