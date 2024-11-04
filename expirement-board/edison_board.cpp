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
Tigr* push_buttons_img;
Tigr* dip_button_img;
tny_word inp_keyboard;
tny_word push_button_state;
int mouse_x;
int mouse_y;
int mouse_button;
int old_mouse_button;

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

    push_buttons_img = tigrLoadImage("exp_board_images/buttons.png");
    if (!push_buttons_img ) {
        printf("Failed to load image <buttons.png> .\n");
        return EXIT_FAILURE;
    }

    dip_button_img = tigrLoadImage("exp_board_images/dips.png");
    if (!dip_button_img ) {
        printf("Failed to load image <dips.png> .\n");
        return EXIT_FAILURE;
    }
    
    window = tigrWindow(background_img->w, background_img->h, "Edison Board", TIGR_FIXED);
    
    reset_board();

    return EXIT_SUCCESS;
}

/* Resets and initializes variables */
void reset_board(){
    /* only draw board background on reset */
    tigrBlit(window, background_img, 0, 0, 0, 0, background_img->w, background_img->h);
    lcd_cursor.u = 0;
    lcd_cursor_x_y.u = 0;
    inp_keyboard.u = 0;
    push_button_state.u = 0;
    lcd_clear_screen();
    led_array_draw(NULL);
    render_push_buttons(NULL);
    render_dip_switches();
}

/* Free all memory */
void kill_board(){
    tigrFree(window);
    tigrFree(background_img);
    tigrFree(lcd_font_img);
    tigrFree(leds_img);
    tigrFree(push_buttons_img);
    tigrFree(dip_button_img);
}

void render_dip_switches(){
    int dest_x = LOC_DIPS_PORTA_TL[0];
    int dest_y = LOC_DIPS_PORTA_TL[1];
    int src_x = 0;
    int src_y = 0;
    int dip_width = (dip_button_img->w / 2);
    int dip_height = dip_button_img->h;
    int index = 7;

    for(int i = 0; i < PORTA_DIPS; i++){
        src_x = dip_width * (inp_keyboard.u & (1<<index)) >> index;
        if(i == 3) dip_width-=1;  // The sprite is just a tiny too big and will cover stuff :(
        tigrBlit(window, dip_button_img, dest_x, dest_y, src_x, src_y, dip_width, dip_height);
        dest_x += dip_width;   
        index--;
    }
}

void render_push_buttons(tny_word *t){

    int dest_x = LOC_BUTTONS_PORTA_TL[0];
    int dest_y = LOC_BUTTONS_PORTA_TL[1];
    int src_x = 0;
    int src_y = 0;
    int button_width = push_buttons_img->w / 2;
    int button_height = push_buttons_img->h;

    tny_word port_a;
    port_a.u = 0;
    if(t) port_a.u = t->u;
    int index = 15;
    for(int i = 0; i < BUTTONS_ROWS; i++){
        for(int j = 0; j < BUTTONS_COLS; j++){
                src_x = button_width * (port_a.u & (1<<index)) >> index;
                tigrBlit(window, push_buttons_img, dest_x, dest_y, src_x, src_y, button_width, button_height);
                dest_x += button_width;
                index--;
        } 
        dest_x = LOC_BUTTONS_PORTA_TL[0];
        dest_y += button_height;
    }
}

void process_keyboard(teenyat* t){
    
    /* Handels button presses related to port_A */
    tny_word old_keyboard = inp_keyboard;

    inp_keyboard.bits.bit11 = tigrKeyHeld(window, 'A');
    inp_keyboard.bits.bit10 = tigrKeyHeld(window, 'S');
    inp_keyboard.bits.bit9 = tigrKeyHeld(window,  'D');
    inp_keyboard.bits.bit8 = tigrKeyHeld(window,  'F');

    inp_keyboard.bits.bit15 = tigrKeyHeld(window, 'Q');
    inp_keyboard.bits.bit14 = tigrKeyHeld(window, 'W');
    inp_keyboard.bits.bit13 = tigrKeyHeld(window, 'E');
    inp_keyboard.bits.bit12 = tigrKeyHeld(window, 'R');

    inp_keyboard.bits.bit0 = tigrKeyHeld(window, TK_LEFT);
    inp_keyboard.bits.bit1 = tigrKeyHeld(window, TK_UP);
    inp_keyboard.bits.bit2 = tigrKeyHeld(window, TK_RIGHT);
    inp_keyboard.bits.bit3 = tigrKeyHeld(window, TK_DOWN);
    
    /* Mask in push_button_state */
    inp_keyboard.u |= push_button_state.u;
    tny_set_ports(t,&inp_keyboard,NULL);

    /* Render displays on port change */
    if(inp_keyboard.u ^ old_keyboard.u){
        render_dip_switches();
        render_push_buttons(&inp_keyboard);
        led_array_draw(t);   
    }

    old_keyboard.u = inp_keyboard.u;
}

void process_mouse(){
    tigrMouse(window, &mouse_x, &mouse_y, &mouse_button);
    if(mouse_button != 0 && old_mouse_button == 0){
        mouse_pressed();
    }else if(mouse_button != 0 && old_mouse_button != 0){
        mouse_down();
    }else if(mouse_button == 0 && old_mouse_button != 0){
        mouse_released();
    }
    old_mouse_button = mouse_button;
}

void mouse_pressed(){

    /* Check collisions with port A dips */
    int dest_x = LOC_DIPS_PORTA_TL[0];
    int dest_y = LOC_DIPS_PORTA_TL[1];
    int width = (dip_button_img->w / 2);
    int height = dip_button_img->h;
    int index = 7;

    for(int i = 0; i < PORTA_DIPS; i++){
        if(point_rect(mouse_x,mouse_y,dest_x,dest_y,width,height)){
            /* mask in our dip switch values here */
            inp_keyboard.u ^= (1<<index);
            render_dip_switches();
            return;
        }
        dest_x += width;   
        index--;
    }
}

void mouse_down(){
    /* Check collisions with Port A push buttons */
    int dest_x = LOC_BUTTONS_PORTA_TL[0];
    int dest_y = LOC_BUTTONS_PORTA_TL[1];
    int width = push_buttons_img->w / 2;
    int height = push_buttons_img->h;
    int index = 15;
    push_button_state.u = 0;
    for(int i = 0; i < BUTTONS_ROWS; i++){
        for(int j = 0; j < BUTTONS_COLS; j++){
                if(point_rect(mouse_x,mouse_y,dest_x,dest_y,width,height)){
                    push_button_state.u |= (1<<index);
                    return;
                }
                dest_x += width;
                index--;
        } 
        dest_x = LOC_BUTTONS_PORTA_TL[0];
        dest_y += height;
    }
}

void mouse_released(){
    push_button_state.u = 0;
}