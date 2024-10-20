#include <iostream>
#include "teenyat.h"
#include "tigr.h"
#include "utils.h"
#include "edison_board.h"
#include "lcd_screen.h"
#include "leds.h"
#include "edison_sprite_locations.h"

Tigr* window;
Tigr* background_img;
Tigr* lcd_font_img;
Tigr* leds_img;

/* Loads images along with window width and height */
int initialize_board(){

    background_img = tigrLoadImage("exp_board_images/edison.png");
    if (!background_img) {
        printf("Failed to load image <edison_board.png> .\n");
        return EXIT_FAILURE;
    }

    lcd_font_img = tigrLoadImage("exp_board_images/teenyat_font.png");
    if (!lcd_font_img ) {
        printf("Failed to load image <teenyat_font.png> .\n");
        return EXIT_FAILURE;
    }

    leds_img = tigrLoadImage("exp_board_images/leds.png");
    if (!leds_img ) {
        printf("Failed to load image <leds.png> .\n");
        return EXIT_FAILURE;
    }

    
    window = tigrWindow(background_img->w, background_img->h, "Edison Board", TIGR_FIXED);
    
    reset_board();

    return EXIT_SUCCESS;
}

/* Resets and initializes variables */
void reset_board(){
    lcd_cursor.u = 0;
    lcd_cursor_x_y.u = 0;
    lcd_clear_screen();
}

/* Render background & components to window */
void render_board(){
    tigrBlit(window, background_img, 0, 0, 0, 0, background_img->w, background_img->h);
    lcd_render_screen();
    tigrUpdate(window);
    led_array_draw(NULL);
}

/* Free all memory */
void kill_board(){
    tigrFree(window);
    tigrFree(background_img);
    tigrFree(lcd_font_img);
    tigrFree(leds_img);
}
