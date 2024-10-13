#ifndef __EDISON_BOARD_H__
#define __EDISON_BOARD_H__

#include "teenyat.h"

extern Tigr* window;
extern Tigr* background_img;
extern Tigr* lcd_font_img;

int initialize_board();
void reset_board();
void render_board();
void kill_board();

#endif  /*   __EDISON_BOARD_H__    */