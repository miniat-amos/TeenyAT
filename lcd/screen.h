#ifndef __SCREEN_H__
#define __SCREEN_H__


#include <cstdint>

#define KEYBOARD_INPUT_BUFFER_SIZE 4

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
extern int last_bus;
extern int current_frame;

extern int x1;
extern int y1;
extern int x2;
extern int y2;
extern int keyboard_input_buffer[KEYBOARD_INPUT_BUFFER_SIZE];
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
    
// Set the x1,x2,y1,y2 values
void setVal(int val,int * ret);

void verticalLine(int x1, int y1,int y2);
void horizontalLine(int y1, int x1,int x2);
void line();
void rect();

// Handles all keyboard presses and adds them to buffer
void buffer_push(int buffer[], int len,int num);
int  buffer_pop(int buffer[], int len);
void buffer_print(int buffer[], int len);
int process_keyboard_input(Tigr*);

// Just draws pixels
void render();
// update game and draw all pixels 
void update();

// initializes the window and sets all pixels to specified value
void initScreen(double v);

#endif  /*   SCREEN_H    */