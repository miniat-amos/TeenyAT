#include <iostream>
#include "tigr.h"
#include "utils.h"
#include "teenyat.h"
#include "lcd_screen.h"
#include "edison_board.h"
#include "edison_sprite_locations.h"

tny_word lcd_cursor;
tny_word lcd_cursor_x_y;
tny_word lcd_data[LCD_ROWS * LCD_COLUMNS];

void lcd_clear_screen(){
    for(int i = 0; i < LCD_ROWS*LCD_COLUMNS; i++){
        lcd_data[i].u = 0;
    }
    lcd_render_full_screen();
}

/* Shifts the data in the lcd_data array by one */
void lcd_shift_screen(){
    for(int i = 0; i < (LCD_COLUMNS * LCD_ROWS)-1; i++){
            lcd_data[i] = lcd_data[i+1];
    }
}

/* Shifts the data in the lcd_data array by line */
void lcd_shift_lines(){
    int i;
    for(i = 0; i < (LCD_COLUMNS * LCD_ROWS)-LCD_COLUMNS; i++){
            lcd_data[i] = lcd_data[i + LCD_COLUMNS];
    }
    for(int k = i; k < i + LCD_COLUMNS; k++){
        lcd_data[k].u = 0;
    }
    lcd_render_full_screen();
}

void lcd_increment_variable(int *var,int amt,int wrap,int ctrl){
    int increment = amt >= 0 ? 1: -1;
    for(int i = 0; i < abs(amt); i++){
        *var += increment;
        if((*var<0 || (*var>=LCD_COLUMNS || (*var>=LCD_ROWS && ctrl))) && !wrap){
            *var -= increment;
            return;
        }
    }
    if(ctrl) *var = abs_mod(*var,LCD_ROWS);
    else *var = abs_mod(*var,LCD_COLUMNS);
}

tny_word lcd_move_cursor_x_y(int amt, int wrap,int ctrl,int read){
    
    /* This is here due to the edge condition of a draw
    *  at the end of lcd space as the cursor is technically
    *  offscreen.......
    */
    int temp_cursor = lcd_cursor.u;
    if(temp_cursor >= (LCD_COLUMNS*LCD_ROWS)) temp_cursor--;
    int x = temp_cursor % LCD_COLUMNS;
    int y = temp_cursor / LCD_COLUMNS;
    
    /* Bad solution to this problem I will figure out the math later */
    if(!ctrl){
        lcd_increment_variable(&x,amt,wrap,ctrl);
    }else{
       lcd_increment_variable(&y,amt,wrap,ctrl);
    }
   
    tny_word temp = lcd_cursor;
    lcd_cursor.u = y * LCD_COLUMNS + x;
    lcd_cursor_x_y.bytes.byte0 = lcd_cursor.u / LCD_COLUMNS; // byte zero is y
    lcd_cursor_x_y.bytes.byte1 = lcd_cursor.u % LCD_COLUMNS; // byte one is x
    tny_word return_value = lcd_data[lcd_cursor.u];
    
    if(read) lcd_cursor = temp;  
    
    return return_value;
}

void lcd_set_cursor_x_y(tny_word value, int ctrl,int combined){
        
        int x = lcd_cursor.u % LCD_COLUMNS;
        int y = lcd_cursor.u / LCD_COLUMNS;

        if(!ctrl) x = (value.u % LCD_COLUMNS);

        if(ctrl) y = (value.u % LCD_ROWS);

        if(combined){
            x = value.bytes.byte1 % LCD_COLUMNS;
            y = value.bytes.byte0 % LCD_ROWS;
        }
        
        lcd_cursor.u = y * LCD_COLUMNS + x;
        lcd_cursor_x_y.bytes.byte1 = x;
        lcd_cursor_x_y.bytes.byte0 = y;
}

/* Creates a new line shifting screen if neccessary */
void lcd_new_line(){

    int y = lcd_cursor_x_y.bytes.byte0; 
    int x = lcd_cursor_x_y.bytes.byte1; 
    y++;

    if(y >= LCD_ROWS){
        y = 3;
        lcd_shift_lines();
    }  
    lcd_cursor.u = y * LCD_COLUMNS + x;
    lcd_cursor_x_y.bytes.byte0 = y;
}

/* Returns the cursor to the start of a line */
void lcd_return_line(){
    tny_word zero;
    zero.u = 0;
    lcd_set_cursor_x_y(zero, 0,0);
}

/* Writes given char to lcd and updates cursor position*/
void lcd_draw_character(tny_word value){

    /* On write to final screen position make a new line and return cursor */
    if(lcd_cursor.u >= (LCD_COLUMNS * LCD_ROWS)){  
        lcd_cursor.u = (LCD_COLUMNS * LCD_ROWS) - LCD_COLUMNS;
        lcd_new_line();
        lcd_return_line();
    }

    lcd_data[lcd_cursor.u] = value;
    lcd_render_cursor_panel(lcd_cursor);

    lcd_cursor.u = (lcd_cursor.u + 1);

    if(value.u == '\r'){
        if(lcd_cursor.u > 0) lcd_cursor.u = (lcd_cursor.u - 1);
        lcd_return_line();
    }else if(value.u == '\n'){
        lcd_new_line();
        if(lcd_cursor.u > 0) lcd_cursor.u = (lcd_cursor.u - 1);
    }

    lcd_cursor_x_y.bytes.byte0 = lcd_cursor.u / LCD_COLUMNS; // byte zero is y
    lcd_cursor_x_y.bytes.byte1 = lcd_cursor.u % LCD_COLUMNS; // byte one is x
}

void lcd_render_cursor_panel(tny_word val){

    int lcd_sprite_width = lcd_font_img->w / LCD_SPRITE_SHEET_SIZE;
    int lcd_sprite_height = lcd_font_img->h / LCD_SPRITE_SHEET_SIZE;

    int x = val.u % LCD_COLUMNS;
    int y = val.u / LCD_COLUMNS;

    /* Get control bits */
    int flip_vertical = lcd_data[y * LCD_COLUMNS + x].bits.bit13;
    int flip_horizontal = lcd_data[y * LCD_COLUMNS + x].bits.bit14;
    int rotate_180 = lcd_data[y * LCD_COLUMNS + x].bits.bit15;

    int font_index = lcd_data[y * LCD_COLUMNS + x].bytes.byte0; // get index into tny_sprite sheet
    int font_x = (font_index % LCD_SPRITE_SHEET_SIZE) * lcd_sprite_width;
    int font_y = (font_index / LCD_SPRITE_SHEET_SIZE) * lcd_sprite_height;
            
    int dest_x = LOC_LCD_TL[0] + (lcd_sprite_width  * x);
    int dest_y = LOC_LCD_TL[1] + (lcd_sprite_height * y);

    if(flip_horizontal || flip_vertical || rotate_180){
        Tigr* sprite_sheet = tigrBitmap(lcd_sprite_width, lcd_sprite_height);
        tigrBlit(sprite_sheet, lcd_font_img, 0, 0, font_x , font_y, lcd_sprite_width, lcd_sprite_height);
        if(flip_vertical){
            sprite_sheet = img_flip_vertical(sprite_sheet);
        }
        if(flip_horizontal){
            sprite_sheet = img_flip_horizontal(sprite_sheet);
        }
        if(rotate_180){
            sprite_sheet = img_rotate_180(sprite_sheet);
        }
        tigrBlitAlpha(window, sprite_sheet, dest_x, dest_y, 0 , 0, lcd_sprite_width, lcd_sprite_height,BLIT_ALPHA);
    }else{

        tigrBlitAlpha(window, lcd_font_img, dest_x, dest_y, font_x , font_y, lcd_sprite_width, lcd_sprite_height,BLIT_ALPHA);
    }
}

/* Renders lcd peripheral with corresponding flip and rotation */
void lcd_render_full_screen(){
    for(int i = 0; i < LCD_ROWS * LCD_COLUMNS; i++){   
            tny_word index;
            index.u = i; 
            lcd_render_cursor_panel(index);
    }
}