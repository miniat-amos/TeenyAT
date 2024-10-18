#ifndef __LCD_SCREEN_H__
#define __LCD_SCREEN_H__


#include "teenyat.h"

#define LCD_ROWS 4
#define LCD_COLUMNS 20
#define LCD_SPRITE_SHEET_SIZE 16

extern tny_word lcd_cursor;
extern tny_word lcd_cursor_x_y;
extern tny_word lcd_data[LCD_ROWS * LCD_COLUMNS];

void lcd_render_screen();
void lcd_draw_character(tny_word);
void lcd_clear_screen();
void lcd_new_line();
void lcd_return_line();
void lcd_set_cursor_x_y(tny_word, int,int);
tny_word lcd_move_cursor_x_y(int,int,int,int);

#endif  /*__LCD_SCREEN_H__ */ 