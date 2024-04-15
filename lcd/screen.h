#ifndef __SCREEN_H__
#define __SCREEN_H__


#include <cstdint>

extern Tigr* window;
extern int drawStroke;
extern int drawFill;
extern int windowWidth;
extern int windowHeight;
extern uint16_t currStrokeColor;
extern uint16_t currFillColor;
const int gridLength = 64;
extern double hue;
extern double pixelSize;
extern int mouseX;
extern int mouseY;
extern int mouseButton;
extern int current_frame;

extern int lcd_x1;
extern int lcd_y1;
extern int lcd_x2;
extern int lcd_y2;
extern uint16_t live_screen[];
extern uint16_t update_screen[];

void point();
uint16_t pointColor();
uint16_t rand16();
void noFill();
void noStroke();

// setFill and setStroke are used by main.cpp 
void setFill(uint16_t col);
void setStroke(uint16_t col);

// fill and stroke are only used by the renderer
void fill(uint16_t col);
void stroke(uint16_t col);
    
// Set the lcd_x1,lcd_x2,lcd_y1,lcd_y2 values
void setVal(int val,int * ret);

void verticalLine(int lcd_x1, int lcd_y1,int lcd_y2);
void horizontalLine(int lcd_y1, int lcd_x1,int lcd_x2);
void line();
void rect();

// Just draws pixels
void render();
// update game and draw all pixels 
void update();

// initializes the window and sets all pixels to specified value
void initScreen(double v);

#endif  /*   SCREEN_H    */