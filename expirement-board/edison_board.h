#ifndef __EDISON_BOARD_H__
#define __EDISON_BOARD_H__

#include "teenyat.h"

#define BUTTONS_ROWS 2
#define BUTTONS_COLS 4

extern Tigr* window;
extern Tigr* background_img;
extern Tigr* lcd_font_img;
extern Tigr* leds_img;

int initialize_board();
void reset_board();
void kill_board();
void process_keyboard(teenyat*);
void render_push_buttons(teenyat*);

#endif  /*   __EDISON_BOARD_H__    */