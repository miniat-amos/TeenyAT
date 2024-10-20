#include <iostream>
#include "tigr.h"
#include "leds.h"
#include "edison_board.h"
#include "edison_sprite_locations.h"

void led_array_draw(teenyat *t){

        tny_word port_a;
        tny_word port_b;

        port_a.u = 0;
        port_b.u = 0;

        if(t){
            tny_get_ports(t,&port_a,&port_b);
        }

        int led_sprite_width = leds_img->w / LED_NUMBER;
        int led_sprite_height = leds_img->h;
        int dest_x = LOC_LEDS_PORTA_TL[0];
        int dest_y = LOC_LEDS_PORTA_TL[1];
        int src_x = 0;
        int src_y = 0;

        /* Render all of port_a's leds */
        int index = 15;
        for(int y = 0; y < LED_ROWS; y++){
            for(int x = 0; x < LED_COLUMNS; x++){
                    /* The led is either on or off */
                    src_x = led_sprite_width * (port_a.u & ( 1 << index )) >> index;
                    tigrBlit(window, leds_img, dest_x, dest_y, src_x , src_y, led_sprite_width, led_sprite_height);
                    dest_x += led_sprite_width;
                    index--;

            }
            dest_x = LOC_LEDS_PORTA_TL[0];
            dest_y += led_sprite_height;
        }

        /* Render all of port_b's leds */
        dest_x = LOC_LEDS_PORTB_TL[0];
        dest_y = LOC_LEDS_PORTB_TL[1];
        src_x = 0;
        src_y = 0;
        index = 15;
        for(int y = 0; y < LED_ROWS; y++){
            for(int x = 0; x < LED_COLUMNS; x++){
                    /* The led is either on or off */
                    src_x = led_sprite_width * (port_b.u & ( 1 << index )) >> index;
                    tigrBlit(window, leds_img, dest_x, dest_y, src_x , src_y, led_sprite_width, led_sprite_height);
                    dest_x += led_sprite_width;
                    index--;

            }
            dest_x = LOC_LEDS_PORTB_TL[0];
            dest_y += led_sprite_height;
        }
}
