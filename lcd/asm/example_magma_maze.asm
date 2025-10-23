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

.const DATABASE_FABULOUS 2689  ; the default screen color
.const MAZE_COLOR 0

.const BLOCK_SIZE 8
.const BLOCK_CNT 8

.const MAG_CNT 20

.const MAGMA 3585
.const BLACK 0

!main

; clear the screen
    STR [X1], rZ
    STR [Y1], rZ
    SET rA, DATABASE_FABULOUS
    STR [FILL], rA
    SET rA, 63
    STR [X2], rZ
    STR [Y2], rZ
    STR [STROKE], rZ
    STR [RECT], rZ

    lod rA, [LIVESCREEN]

    STR [STROKE], rZ

;;;
;;; Draw the angled maze background
;;;

    SET rD, BLOCK_CNT  ; rD if the row
!next_block_row
    SET rE, BLOCK_CNT  ; rE is the col
!next_block_col

; calculate the extreme Xs of this block
    SET rA, BLOCK_SIZE
    SET rB, rE - 1
    MPY rA, rB
    STR [X1], rA
    ADD rA, BLOCK_SIZE
    DEC rA
    STR [X2], rA

; calculate the top block Y
    SET rA, BLOCK_SIZE
    SET rB, rD - 1
    MPY rA, rB

; randomly choose forward vs back slash direction
    lod rB, [RAND]
    AND rB, 0x0001
    cmp rB, rZ
    JNE !forward_slash

;back_slash
    STR [Y1], rA
    ADD rA, BLOCK_SIZE
    DEC rA
    STR [Y2], rA
    JMP !draw_slash

!forward_slash
    STR [Y2], rA
    ADD rA, BLOCK_SIZE
    DEC rA
    STR [Y1], rA

!draw_slash
    STR [LINE], rZ

    LUP rE, !next_block_col
    LUP rD, !next_block_row

    ;;;
    ;;; Erase the top row so there's room for magma!
    ;;;
    SET rB, DATABASE_FABULOUS
    SET rC, 64
!erase_next_in_top_row
    SET rA, rC - 1
    ADD rA, UPDATESCREEN
    STR [rA], rB
    LUP rC, !erase_next_in_top_row    

;;;
;;; Remove some junctions where a maze pixel is recursively connected 
;;; via N,E,W, or S to another maze pixel. 
;;;

    ;;;
    ;;; The two loops below start at 63 in each dimension and only look through 
    ;;; 1 in that dimension.  No need to start from row or column zero as we
    ;;; look up and left in our scan, so we'll already have looked in the zeros.
    ;;;

    SET rD, 63  ; rD if the row
!scan_next_row_for_maze_pixel
    SET rE, 63  ; rE is the col
!scan_next_col_for_maze_pixel

; calculate rA as address of a bottom right pixel in a 4-box area
    SET rA, 64
    MPY rA, rD
    ADD rA, rE
    ADD rA, UPDATESCREEN

; checking the bottom right start pixel    
    lod rB, [rA]
    cmp rB, MAZE_COLOR
    JNE  !continue_maze_pixel_scan

; check bottom left pixel
    lod rB, [rA - 1]
    cmp rB, MAZE_COLOR
    JNE  !continue_maze_pixel_scan


!wipe_4_block

; only wipe with some probability so it looks more natural, 1:4 chance
    lod rB, [RAND]
    AND rB, 0x1
    cmp rB, rZ
    JNE !continue_maze_pixel_scan
    
    SET rC, DATABASE_FABULOUS

    STR [rA + 0], rC
    STR [rA - 1], rC
    STR [rA - 64], rC
    STR [rA - 65], rC

!continue_maze_pixel_scan
    LUP rE, !scan_next_col_for_maze_pixel
    LUP rD, !scan_next_row_for_maze_pixel

!inf

;;;
;;; Make the magma glisten!!!
;;;
    SET rA, 0x1000
!check_next_pixel_for_glistening
    SET rB, rA - 1
    ADD rB, UPDATESCREEN   

    lod rC, [rB]
;if_magma_to_glisten
    cmp rC, DATABASE_FABULOUS
    je  !done_magma_to_glisten
    cmp rC, BLACK
    je  !done_magma_to_glisten

; shift magma color a little
    lod rD, [RAND_BITS]
    MOD rD, 48
    ADD rC, rD
    STR [rB], rC

!done_magma_to_glisten
    LUP rA, !check_next_pixel_for_glistening

;;;
;;; Draw magma randomly in the top row to ooze in
;;;
    SET rE, MAGMA
    SET rC, 64
!create_next_magma_in_top_row
    SET rA, rC - 1
    ADD rA, UPDATESCREEN

    lod rB, [RAND_BITS]
    AND rB, 0x07FF
;if_create_rand_magma
    cmp rB, rZ
    JNE !done_create_rand_magma
    STR [rA], rE
!done_create_rand_magma
    LUP rC, !create_next_magma_in_top_row

;;;
;;; Show the maze
;;;
    STR [UPDATE], rZ

;;;
;;; Make the magma ooze downward
;;;
    SET rA, 0x1000
    SUB rA, 64             ; offset of next to last row right pixel
!check_next_pixel_for_magma
    SET rB, rA - 1
    ADD rB, UPDATESCREEN   

;if_pixel_is_magma
    lod rC, [rB]
    cmp rC, DATABASE_FABULOUS
    je !done_pixel_is_magma
    cmp rC, BLACK
    je !done_pixel_is_magma

; the pixel is magma

    lod rD, [RAND]
    AND rD, 0x1
    lod rE, [RAND]
    AND rE, 0x1
    ADD rD, rE
    DEC rD

    ;;;
    ;;; rD is now a random value from {-1,0,1} with 
    ;;; probabilities of {1,2,1}, respectively
    ;;;

    cmp rD, -1
    je  !ooze_left
    cmp rD, 1
    je  !ooze_right

!ooze_down
    ADD rB, 64
    JMP !try_oozed_magma

!ooze_left
    DEC rB
    JMP !try_oozed_magma

!ooze_right
    INC rB

!try_oozed_magma
    lod rC, [rB]
    cmp rC, BLACK
    je !done_pixel_is_magma

    lod rC, [RAND]
    AND rC, 0x1
    cmp rC, rZ
    JNE !done_pixel_is_magma
    SET rE, MAGMA
    STR [rB], rE

!done_pixel_is_magma
    LUP rA, !check_next_pixel_for_magma    


    JMP !inf
