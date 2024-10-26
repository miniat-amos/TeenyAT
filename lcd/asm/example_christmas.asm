;; This is a little christmas demo I made in a half hour. the codes not very clean ;;
;; but whatever.....MERRY CHRISTMAS................................................;;

.const RAND 0x8010

.const LIVESCREEN 0x9000
.const UPDATESCREEN 0xA000
.const X1 0xD000
.const Y1 0xD001
.const X2 0xD002
.const Y2 0xD003
.const STROKE 0xD010
.const FILL 0xD011
.const DRAWSTROKE 0xD013
.const UPDATE 0xE000
.const RECT 0xE010
.const POINT 0xE012

.const BROWN 3900
.const WHITE 2561
.const RED 3585
.const GREEN 1537
.const LIGHTGREEN 1796
.const YELLOW 2116
.const LAVENDER 2689

.const BOTTOMSCREEN 45056


CAL !draw_trunk
!main
    CAl !draw_lights
    CAl !gen_snow
    CAL !update_snow
    STR [UPDATE], rZ
    JMP !main

!gen_snow
    SET rA, 0
    LOD rA, [RAND]
    MOD rA, 65
    SET rB, WHITE
    STR [STROKE], rB
    STR [X1], rA
    STR [Y1], rZ
    STR [POINT], rZ

    RET

!update_snow
    SET rA, 0x1000
    !snow_loop
        LOD rB, [rA + UPDATESCREEN]

        CMP rB, WHITE
        JNE !snow_loop_bottom
        
        SET rC, rA + UPDATESCREEN ; the index were currently at

        ;; SETS all white points to LAVENDER
        ADD rC, 64 ;; the index below it
        CMP rC, BOTTOMSCREEN 
        JGE !snow_loop_bottom

        LOD rD, [rC + 0]
        CMP rD, LAVENDER 
        JE !snow_shimmy_random

        CMP rD, WHITE 
        JE !snow_shimmy_check
        JNE !snow_shimmy_check
        
        !snow_shimmy_return
        SET rE, WHITE
        STR [STROKE], rE
        STR [rC + 0], rE

        SET rE, LAVENDER
        STR [STROKE], rE
        STR [rA + UPDATESCREEN], rE

        !snow_shimmy_fail
        !snow_loop_bottom
        DEC rA
        CMP rA, 0
        JLE !return
        JMP !snow_loop

!snow_shimmy_random
        SET rE, 0
        LOD rE, [RAND]
        MOD rE, 2

        CMP rE, 0
        JE !snow_shimmy_right

        CMP rE, 1
        JE !snow_shimmy_left

        !snow_shimmy_right
                INC rC
                LOD rD, [rC + 0]
                CMP rD, LAVENDER 
                JE !snow_shimmy_return
                DEC rC
                JMP !snow_shimmy_return
        !snow_shimmy_left
                DEC rC
                LOD rD, [rC + 0]
                CMP rD, LAVENDER 
                JE !snow_shimmy_return
                INC rC
                JMP !snow_shimmy_return
        
!snow_shimmy_check
        INC rC
        LOD rD, [rC + 0]
        CMP rD, LAVENDER 
        JE !snow_shimmy_return
        DEC rC
    
        DEC rC
        LOD rD, [rC + 0]
        CMP rD, LAVENDER 
        JE !snow_shimmy_return
        INC rC
        JMP !snow_shimmy_fail
        
!set_yellow
    SET rD, YELLOW
    STR [STROKE], rD
    JMP !slide_end

!set_light_green
    SET rD, LIGHTGREEN
    STR [STROKE], rD
    JMP !slide_end

!set_red
    SET rD, RED
    STR [STROKE], rD
    JMP !slide_end

!draw_trunk
    SET rA, 28
    SET rB, 44
    STR [X1], rA
    STR [Y1], rB
    SET rA, 35
    SET rB, 63
    STR [X2], rA
    STR [Y2], rB

    SET rA, BROWN
    STR [DRAWSTROKE], rZ
    STR [FILL], rA
    STR [RECT], rZ
    RET

!draw_lights
    SET rC, 0
    !lights_loop
        SET rD, 0
        SET rE, 0
        SET rA, 18
        ADD rA, rC
        SET rB, 44
        SUB rB, rC
        STR [Y1], rB
        SET rB, 45
        SUB rB, rC
        !slide_loop
            
            SET rE, rA
            MOD rE, 2
            CMP rE, 0
            SET rD, GREEN
            STR [STROKE], rD
            JE !slide_end

            SET rE, 0
            LOD rE, [RAND]
            MOD rE, 4

            CMP rE, 0
            JE !set_yellow

            CMP rE, 1
            JE !set_light_green

            CMP rE, 2
            JE !set_red

        !slide_end
            STR [X1], rA
            STR [POINT], rZ
            DLY 500
            INC rA
            CMP rA, rB
            JLE !slide_loop
    
        INC rC
        CMP rC, 15
        JGE !return
        JMP !lights_loop


!return
    RET