;
; M""""""""M            oo          dP       dP             M""""""""M            oo          dP       dP          
; Mmmm  mmmM                        88       88             Mmmm  mmmM                        88       88          
; MMMM  MMMM dP  dP  dP dP 88d888b. 88  .dP  88 .d8888b.    MMMM  MMMM dP  dP  dP dP 88d888b. 88  .dP  88 .d8888b. 
; MMMM  MMMM 88  88  88 88 88'  `88 88888"   88 88ooood8    MMMM  MMMM 88  88  88 88 88'  `88 88888"   88 88ooood8 
; MMMM  MMMM 88.88b.88' 88 88    88 88  `8b. 88 88.  ...    MMMM  MMMM 88.88b.88' 88 88    88 88  `8b. 88 88.  ... 
; MMMM  MMMM 8888P Y8P  dP dP    dP dP   `YP dP `88888P'    MMMM  MMMM 8888P Y8P  dP dP    dP dP   `YP dP `88888P' 
; MMMMMMMMMM                                                MMMMMMMMMM                                             
;                                                                                                                 
; M""MMMMMMMM oo   dP     dP   dP             MP""""""`MM   dP                                                     
; M  MMMMMMMM      88     88   88             M  mmmmm..M   88                                                     
; M  MMMMMMMM dP d8888P d8888P 88 .d8888b.    M.      `YM d8888P .d8888b. 88d888b.                                 
; M  MMMMMMMM 88   88     88   88 88ooood8    MMMMMMM.  M   88   88'  `88 88'  `88                                 
; M  MMMMMMMM 88   88     88   88 88.  ...    M. .MMM'  M   88   88.  .88 88                                       
; M         M dP   dP     dP   dP `88888P'    Mb.     .dM   dP   `88888P8 dP                                       
; MMMMMMMMMMM                                 MMMMMMMMMMM                                                          
;
; Adapted by:  John Pertell
; Arrangement: Twinkle Twinkle Little Star


; TeenyAT Constants
.const PORT_A_DIR       0x8000
.const PORT_B_DIR       0x8001
.const PORT_A           0x8002
.const PORT_B           0x8003
.const RAND             0x8010
.const RAND_BITS        0x8011

; Edison Constants
.const LCD_CURSOR           0xA000 
.const LCD_CLEAR_SCREEN     0xA001
.const LCD_MOVE_LEFT        0xA002  
.const LCD_MOVE_RIGHT       0xA003
.const LCD_MOVE_UP          0xA004
.const LCD_MOVE_DOWN        0xA005
.const LCD_MOVE_LEFT_WRAP   0xA006  
.const LCD_MOVE_RIGHT_WRAP  0xA007
.const LCD_MOVE_UP_WRAP     0xA008
.const LCD_MOVE_DOWN_WRAP   0xA009
.const LCD_CURSOR_X         0xA00A
.const LCD_CURSOR_Y         0xA00B
.const LCD_CURSOR_XY        0xA00C
.const BUZZER_LEFT          0xA010
.const BUZZER_RIGHT         0xA011
.const FADER_LEFT           0xA020
.const FADER_RIGHT          0xA021


; Buzzer Note Constants
.const SOUND_C4         0x0105  ; C4 (261 Hz)
.const SOUND_CSHARP4    0x0115 ; C#4 (277 Hz)
.const SOUND_D4         0x0125  ; D4 (293 Hz)
.const SOUND_DSHARP4    0x0137 ; D#4 (311 Hz)
.const SOUND_E4         0x0149  ; E4 (329 Hz)
.const SOUND_F4         0x015D  ; F4 (349 Hz)
.const SOUND_FSHARP4    0x0171 ; F#4 (369 Hz)
.const SOUND_G4         0x0188  ; G4 (392 Hz)
.const SOUND_GSHARP4    0x019F ; G#4 (415 Hz)
.const SOUND_A4         0x01B8  ; A4 (440 Hz)
.const SOUND_ASHARP4    0x01D2 ; A#4 (466 Hz)
.const SOUND_B4         0x01ED  ; B4 (494 Hz)
.const SOUND_C5         0x020B  ; C5 (523 Hz)
.const SOUND_CSHARP5    0x022A ; C#5 (554 Hz)
.const SOUND_D5         0x024B  ; D5 (587 Hz)
.const SOUND_DSHARP5    0x026E ; D#5 (622 Hz)
.const SOUND_E5         0x0293  ; E5 (659 Hz)
.const SOUND_F5         0x02BA  ; F5 (698 Hz)
.const SOUND_FSHARP5    0x02E3 ; F#5 (739 Hz)
.const SOUND_G5         0x030F  ; G5 (783 Hz)
.const SOUND_GSHARP5    0x033E ; G#5 (830 Hz)
.const SOUND_A5         0x0370  ; A5 (880 Hz)
.const SOUND_ASHARP5    0x03A4 ; A#5 (932 Hz)
.const SOUND_B5         0x03DB  ; B5 (987 Hz)
.const SOUND_C6         0x0416  ; C6 (1046 Hz)

; Timing Constants
.const DELAY_EIGTH          375     ; Duration of an eigth note
.const DELAY_QUARTER        750     ; Duration of a quarter note
.const DELAY_HALF           1500    ; Duration of a half note
.const DELAY_THREE_QUARTER  2250    ; Duration of a 3/4 note
.const DELAY_FULL           3000    ; Duration of a full note

    jmp !main

!twinkle_str 
.raw "Twinkle Twinkle"

!main
    set rB, !twinkle_str
    cal !print_string_rB
    cal !play_twinkle
    str [BUZZER_RIGHT], rZ
    !end
    jmp !end

;------------------------------------------
; Play "Twinkle Twinkle Little Star"
; Structure: 6 Phrases
!play_twinkle
    cal !phrase1    ; "Twinkle, twinkle, little star"
    cal !phrase2    ; "How I wonder what you are"
    cal !phrase3    ; "Up above the world so high"
    cal !phrase4    ; "Like a diamond in the sky"
    cal !phrase5    ; "Twinkle, twinkle, little star"
    cal !phrase6    ; "How I wonder what you are"
    ret

;------------------------------------------
; Phrase 1: "Twinkle, twinkle, little star" 
; Notes: C, C, G, G, A, A, G 
!phrase1
    set rA, SOUND_C4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_C4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_A4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_A4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_half
    ret

;------------------------------------------
; Phrase 2: "How I wonder what you are" 
; Notes: F, F, E, E, D, D, C 
!phrase2
    set rA, SOUND_F4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_F4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_D4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_D4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_C4
    str [BUZZER_RIGHT], rA
    cal !delay_half
    ret

;------------------------------------------
; Phrase 3: "Up above the world so high" 
; Notes: G, G, F, F, E, E, D 
!phrase3
    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_F4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_F4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_D4
    str [BUZZER_RIGHT], rA
    cal !delay_half
    ret

;------------------------------------------
; Phrase 4: "Like a diamond in the sky" 
; Notes: G, G, F, F, E, E, D 
!phrase4
    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_F4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_F4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_D4
    str [BUZZER_RIGHT], rA
    cal !delay_half
    ret

;------------------------------------------
; Phrase 5: Repeat Phrase 1
!phrase5
    set rA, SOUND_C4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_C4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_A4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_A4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_G4
    str [BUZZER_RIGHT], rA
    cal !delay_half
    ret

;------------------------------------------
; Phrase 6: Repeat Phrase 2
!phrase6
    set rA, SOUND_F4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_F4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_E4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_D4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_D4
    str [BUZZER_RIGHT], rA
    cal !delay_quarter

    set rA, SOUND_C4
    str [BUZZER_RIGHT], rA
    cal !delay_half
    ret

;-----------------------------------------
; Delay Routine: Quarter Note
!delay_quarter
    set rD, DELAY_QUARTER
    set rE, 0
    !inner_delay_quarter
        inc rE
        dly rE
        cmp rE, rD
        jl !inner_delay_quarter
    set rE, 0
    ret

;-----------------------------------------
; Delay Routine: Half Note
!delay_half
    set rD, DELAY_HALF
    set rE, 0
    !inner_delay_half
        inc rE
        dly rE
        cmp rE, rD
        jl !inner_delay_half
    set rE, 0
    ret

;------------------------------------------
; Print String Routine
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
