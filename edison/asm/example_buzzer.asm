.const BUZZER_LEFT 0xA010
.const BUZZER_RIGHT 0xA011
.const FADER_LEFT 0xA020
.const FADER_RIGHT 0xA021

; bit15 defines wave to be played 0 -> sine wave
;                                 1 -> triangle wave
; bit14-bit0 are the frequency to be played
; stores to the buzzer turn them on and make them play the 
; sound/frequency until a zero is put into that buzzer 
; thats pretty much it....

!main
    str [BUZZER_LEFT], rA
    str [BUZZER_RIGHT], rB

    lod rA, [FADER_LEFT]
    lod rB, [FADER_RIGHT]

    mpy rA, 100
    mpy rB, 100
    bts rA, 15
    btc rB, 15

    jmp !main