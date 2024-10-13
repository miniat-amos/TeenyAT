#include <iostream>
#include "teenyat.h"
#include "tigr.h"
#include "utils.h"
#include "edison_board.h"
#include "edison_sprite_locations.h"

Tigr* window;
Tigr* background_img;
Tigr* lcd_font_img;

tny_word lcd_cursor;
tny_word lcd_cursor_x_y;
tny_word lcd_data[LCD_ROWS * LCD_COLUMNS];

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

void lcd_clear_screen(){
    for(int i = 0; i < LCD_ROWS*LCD_COLUMNS; i++){
        lcd_data[i].u = 0;
    }
}

/* Shifts the data in the lcd_data array by one */
void lcd_shift_screen(){
    for(int i = 0; i < (LCD_COLUMNS * LCD_ROWS)-1; i++){
            lcd_data[i] = lcd_data[i+1];
    }
}

void lcd_increment_variable(int *var,int amt,int wrap){
    int increment = amt >= 0 ? 1: -1;
    for(int i = 0; i < abs(amt); i++){
        *var += increment;
        if((*var<0 || *var >= LCD_COLUMNS) && !wrap){
            *var -= increment;
            break;
        }
        *var = abs_mod(*var,LCD_COLUMNS);
    }
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
        lcd_increment_variable(&x,amt,wrap);
    }else{
       lcd_increment_variable(&y,amt,wrap);
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


/* Writes given char to lcd and updates cursor position*/
void lcd_draw_character(tny_word value){
    if(lcd_cursor.u >= (LCD_COLUMNS * LCD_ROWS)){
        lcd_cursor.u = (LCD_COLUMNS * LCD_ROWS) - 1;
        lcd_shift_screen();
    }
    lcd_data[lcd_cursor.u] = value;
    lcd_cursor.u = (lcd_cursor.u + 1);

    lcd_cursor_x_y.bytes.byte0 = lcd_cursor.u / LCD_COLUMNS; // byte zero is y
    lcd_cursor_x_y.bytes.byte1 = lcd_cursor.u % LCD_COLUMNS; // byte one is x
}

/* Renders lcd peripheral with corresponding flip and rotation */
void render_lcd_screen(){

    int lcd_sprite_width = lcd_font_img->w / LCD_SPRITE_SHEET_SIZE;
    int lcd_sprite_height = lcd_font_img->h / LCD_SPRITE_SHEET_SIZE;

    for(int y = 0; y < LCD_ROWS; y++){
        for(int x = 0; x < LCD_COLUMNS; x++){
        
            /* Get control bits */
            int flip_vertical = lcd_data[y * LCD_COLUMNS + x].bits.bit13;
            int flip_horizontal = lcd_data[y * LCD_COLUMNS + x].bits.bit14;
            int rotate_180 = lcd_data[y * LCD_COLUMNS + x].bits.bit15;

            int font_index = lcd_data[y * LCD_COLUMNS + x].bytes.byte0; // get index into tny_sprite sheet
            int font_x = (font_index % LCD_SPRITE_SHEET_SIZE) * lcd_sprite_width;
            int font_y = (font_index / LCD_SPRITE_SHEET_SIZE) * lcd_sprite_height;
            
            int dest_x = LOC_LCD_TL[0] + (lcd_sprite_width  * x);
            int dest_y = LOC_LCD_TL[1] + (lcd_sprite_height * y);

            /* Check if sprite should be flipped or rotated */
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
                tigrBlit(window, sprite_sheet, dest_x, dest_y, 0 , 0, lcd_sprite_width, lcd_sprite_height);
            }else{

                tigrBlit(window, lcd_font_img, dest_x, dest_y, font_x , font_y, lcd_sprite_width, lcd_sprite_height);
            }
            
        }
    }
}

/* Render background & components to window */
void render_board(){
    tigrBlit(window, background_img, 0, 0, 0, 0, background_img->w, background_img->h);
    render_lcd_screen();
    tigrUpdate(window);
}

/* Free all memory */
void kill_board(){
    tigrFree(window);
    tigrFree(background_img);
    tigrFree(lcd_font_img);
}
