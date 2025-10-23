;  ________  _  _                    ________  __  __                     
; /        |/ |/ |                  /        |/  |/  |                    
; $$$$$$$$/_$/ $/__   ______        $$$$$$$$/ $$ |$$/   _______   ______  
; $$ |__  /  |  /  | /      \       $$ |__    $$ |/  | /       | /      \ 
; $$    | $$ |  $$ |/$$$$$$  |      $$    |   $$ |$$ |/$$$$$$$/ /$$$$$$  |
; $$$$$/  $$ |  $$ |$$ |  $$/       $$$$$/    $$ |$$ |$$      \ $$    $$ |
; $$ |    $$ \__$$ |$$ |            $$ |_____ $$ |$$ | $$$$$$  |$$$$$$$$/ 
; $$ |    $$    $$/ $$ |            $$       |$$ |$$ |/     $$/ $$       |
; $$/      $$$$$$/  $$/             $$$$$$$$/ $$/ $$/ $$$$$$$/   $$$$$$$/ 
;
; Adapted by:  John Pertell
; Arrangement: Für Elise
; Composer:    Ludwig Von Beethoven

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

; Timing Constants
.const DELAY_SHORT  750
.const DELAY_LONG   1500
.const WAIT_SHORT   1500
.const WAIT_LONG    3500

    jmp !main

!fur_elise_str 
.raw "Fur Elise"

!main
    set rB, !fur_elise_str
    cal !print_string_rB

    cal !play_fur_elise
    str [BUZZER_RIGHT], rZ
    str [BUZZER_LEFT], rZ

!end
    jmp !end

; ------------------------------------------
; Für Elise with Two-Handed Playback              
; ------------------------------------------
!play_fur_elise
    cal !phrase_1
    cal !phrase_2
    cal !phrase_3
    cal !phrase_4
    cal !phrase_5
    cal !phrase_6

    cal !phrase_7
    cal !phrase_8
    cal !phrase_9
    cal !phrase_10
    cal !phrase_11
    cal !phrase_12
    cal !phrase_13
    cal !phrase_14
    cal !phrase_15
    cal !phrase_16
    ret

; ----------------------------------------------------------
; Phrase 1: Opening motif
; Right Hand (Melody): E5, D#5, E5, D#5, E5, B4, D5, C5, A4
; Left Hand (Accompaniment): Sustain A4 with each note
; ----------------------------------------------------------
!phrase_1
    set rA, SOUND_E5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_DSHARP5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_DSHARP5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_B4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_D5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_C5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_A4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_long
    ret

; ----------------------------------------------------------
; Phrase 2: Arpeggiated fragment
; Right Hand: C4, E4, A4, B4
; Left Hand: Matches the arpeggio: C4, E4, A4, B4
; ----------------------------------------------------------
!phrase_2
    set rA, SOUND_C4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_C4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_E4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_A4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_B4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_B4
    str [BUZZER_LEFT], rC
    cal !delay_long
    ret

; ----------------------------------------------------------
; Phrase 3: Continuation
; Right Hand: E4, A4, B4, C5
; Left Hand: Sustain A4
; ----------------------------------------------------------
!phrase_3
    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_A4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_B4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_C5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_long
    ret

; ----------------------------------------------------------
; Phrase 4: Repeat of Phrase 1
; ----------------------------------------------------------
!phrase_4
    cal !phrase_1
    ret

; ----------------------------------------------------------
; Phrase 5: Repeat of Phrase 2
; ----------------------------------------------------------
!phrase_5
    cal !phrase_2
    ret

; ----------------------------------------------------------
; Phrase 6: Descending line
; Right Hand: E4, C5, B4, A4
; Left Hand: Sustain A4
; ----------------------------------------------------------
!phrase_6
    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_C5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_B4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_A4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short
    ret

; ----------------------------------------------------------
; Phrase 7: Ascending scale fragment
; Right Hand: B4, C5, D5, E5
; Left Hand: Sustain A4
; ----------------------------------------------------------
!phrase_7
    set rA, SOUND_B4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_C5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_D5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_long
    ret

; ----------------------------------------------------------
; Phrase 8: Descending scale fragment
; Right Hand: G4, F4, E4, D4
; Left Hand: Sustain A4
; ----------------------------------------------------------
!phrase_8
    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_F4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_D4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_long
    ret

; ----------------------------------------------------------
; Phrase 9: Descending repetition
; Right Hand: E4, E4, D4, C4
; Left Hand: Sustain A4
; ----------------------------------------------------------
!phrase_9
    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_D4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_C4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_long
    ret

; ----------------------------------------------------------
; Phrase 10: Repeating descending with pause
; Right Hand: E4, D4, C4, E4 then two long delays
; Left Hand: Sustain A4
; ----------------------------------------------------------
!phrase_10
    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_D4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_C4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_long
    cal !delay_long
    ret

; ----------------------------------------------------------
; Phrase 11: Reprise of opening motif
; Right Hand: E5, D#5, E5, D#5, E5, E5, D5, C5, A4
; Left Hand: Sustain A4
; ----------------------------------------------------------
!phrase_11
    set rA, SOUND_E5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_DSHARP5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_DSHARP5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_E5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_D5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_C5
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_short

    set rA, SOUND_A4
    str [BUZZER_RIGHT], rA
    set rC, SOUND_A4
    str [BUZZER_LEFT], rC
    cal !delay_long
    ret

; ----------------------------------------------------------
; Phrase 12: Repeat of Phrase 2
; ----------------------------------------------------------
!phrase_12
    cal !phrase_2
    ret

; ----------------------------------------------------------
; Phrase 13: Repeat of Phrase 3
; ----------------------------------------------------------
!phrase_13
    cal !phrase_3
    ret

; ----------------------------------------------------------
; Phrase 14: Repeat of Phrase 1
; ----------------------------------------------------------
!phrase_14
    cal !phrase_1
    ret

; ----------------------------------------------------------
; Phrase 15: Repeat of Phrase 2
; ----------------------------------------------------------
!phrase_15
    cal !phrase_2
    ret

; ----------------------------------------------------------
; Phrase 16: Descending line with pause
; Right Hand: E4, C5, B4, A4 then a long delay
; Left Hand: Sustain A4
; ----------------------------------------------------------
!phrase_16
    cal !phrase_6
    cal !delay_long
    ret

; ----------------------------------------------------------
; Delay Routines
; ----------------------------------------------------------
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

!wait_short
    set rD, WAIT_SHORT
    set rE, 0                       
!inner_wait_short
    inc rE
    dly rE
    cmp rE, rD 
    jl !inner_wait_short
    set rE, 0
    ret

; ----------------------------------------------------------
; Print String Routine
; ----------------------------------------------------------
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
