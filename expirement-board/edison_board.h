#ifndef __EDISON_BOARD_H__
#define __EDISON_BOARD_H__

#include "teenyat.h"

#define LCD_ROWS 4
#define LCD_COLUMNS 20
#define LCD_SPRITE_SHEET_SIZE 16

extern Tigr* window;
extern Tigr* background_img;
extern Tigr* lcd_font_img;

extern tny_word lcd_cursor;
extern tny_word lcd_cursor_x_y;
extern tny_word lcd_data[];


int initialize_board();
void reset_board();
void render_board();
void kill_board();
void lcd_draw_character(tny_word);
void lcd_clear_screen();
void lcd_set_cursor_x_y(tny_word, int,int);
tny_word lcd_move_cursor_x_y(int,int,int,int);

#endif  /*   __EDISON_BOARD_H__    */