#include <iostream>
#include "tigr.h"
#include "segment.h"
#include "teenyat.h"
#include "edison_board.h"
#include "edison_sprite_locations.h"

void calculate_values(int* v,tny_word inp, int render_mode, int off_set){
    // Render Mode RAW
    if(!render_mode){
        v[off_set+2] = inp.bytes.byte1;
        v[off_set+3] = inp.bytes.byte0;
    }
    // Render Mode DEC
    else if(render_mode == 1){
         for(int i = 0; i < 4; i++){
            int r = inp.u % 10;
            v[(off_set+3)-i] = INDEX_7SEG_HEX[r][0] * SEGMET_SPRITE_NUMBER + INDEX_7SEG_HEX[r][1];
            inp.u /= 10;
        }
    }
    // Render Mode HEX
    else{
        for(int i = 0; i < 4; i++){
            int r = inp.u % 16;
            v[(off_set+3)-i] = INDEX_7SEG_HEX[r][0] * SEGMET_SPRITE_NUMBER + INDEX_7SEG_HEX[r][1];
            inp.u /= 16;
        }
    }
}

void segment_render_display(teenyat* t){
    tny_word value_a;
    tny_word value_b;

    tny_get_ports(t,&value_a,&value_b);

    /* Values stores the index into the sprite sheet to render 
    *  please not however that the order is reversed for easy rendering
    *  so B's higher order bits are first then A is drawn 
    */
    int values[8] = {0};
    calculate_values(values,value_b, segment_dips.bytes.byte1, 0);
    calculate_values(values,value_a, segment_dips.bytes.byte0, 4);
    
    int increment_x = LOC_7SEG_PORTB_TL[1][0] - LOC_7SEG_PORTB_TL[0][0];
    int dest_x = LOC_7SEG_PORTB_TL[0][0];
    int dest_y = LOC_7SEG_PORTB_TL[0][1];
    int src_x = 0;
    int src_y = 0;
    int width = seg_seven_img->w / SEGMET_SPRITE_NUMBER;
    int height = seg_seven_img->h / SEGMET_SPRITE_NUMBER;

    int index = 0;
    for(int y = 0; y < SEGMENT_ROWS; y++){
        for(int x = 0; x < SEGMENT_COLUMNS; x++){
            src_x = values[index] % SEGMET_SPRITE_NUMBER;
            src_y = values[index] / SEGMET_SPRITE_NUMBER;
            tigrBlitAlpha(window, seg_seven_img, dest_x, dest_y, src_x*width , src_y*height, width, height,BLIT_ALPHA);
            dest_x += increment_x;
            index++;
        }
        dest_x = LOC_7SEG_PORTA_TL[0][0];
        dest_y = LOC_7SEG_PORTA_TL[0][1];
        increment_x = LOC_7SEG_PORTA_TL[1][0] - LOC_7SEG_PORTA_TL[0][0];
    }
    

}