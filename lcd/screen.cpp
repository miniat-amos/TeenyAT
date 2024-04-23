#include <iostream>
#include <ctime>
#include <sstream>
#include <cstdint>
#include <vector>

#include "tigr.h"
#include "color.h"
#include "screen.h"


Tigr* window;
int drawStroke = 1;
int drawFill = 1;
int windowWidth = 640;
int windowHeight = 640;
uint16_t currStrokeColor = 0;
uint16_t currFillColor = 0;
double hue = 0;
double pixelSize = (double) windowWidth / gridLength;

int lcd_x1 = 0;
int lcd_y1 = 0;
int lcd_x2 = 0;
int lcd_y2 = 0;

int mouseX = 0;
int mouseY = 0;
int mouseButton = 0;
int current_frame = 0;

TPixel currFill = TPixel {
  0,
  0,
  0,
  255
};
TPixel currStroke = TPixel {
  0,
  0,
  0,
  255
};

uint16_t live_screen[gridLength * gridLength] = {
  0
};

uint16_t update_screen[gridLength * gridLength] = {
  0
};

int keyboard_input_buffer[KEYBOARD_INPUT_BUFFER_SIZE] = {
  0
};

void point() {
  int index = lcd_y1 * gridLength + lcd_x1;
  update_screen[index] = currStrokeColor;
}

uint16_t pointColor(){
  int index = lcd_y1 * gridLength + lcd_x1;
  return update_screen[index];
}

uint16_t rand16() {

  return (uint16_t)(rand() << 1) ^ rand();
}

void noFill() {

  drawFill = 0;
}

void noStroke() {

  drawStroke = 0;
}

/* setFill and setStroke are used by main.cpp */
void setFill(uint16_t col) {

  drawFill = 1;
  currFillColor = col;
}

void setStroke(uint16_t col) {

  drawStroke = 1;
  currStrokeColor = col;
}

/* fill and stroke are only used by the renderer */
void fill(uint16_t col) {

  unsigned char r, g, b;
  unsigned char a = 255;
  HSVtoRGB(col, & r, & g, & b);
  currFill = TPixel {
    r,
    g,
    b,
    a
  };
}

void stroke(uint16_t col) {

  unsigned char r, g, b;
  unsigned char a = 255;
  HSVtoRGB(col, & r, & g, & b);
  currStroke = TPixel {
    r,
    g,
    b,
    a
  };
}

/* Sets the x1,x2,y1,y2 values */
void setVal(int val, int * ret) {

  *ret = (val % (gridLength));
}

/* Fast horizontal and vertical line drawer */  
void verticalLine(int x1, int y1, int y2) {

  int dist = abs(y2 - y1) + 1;
  int y = std::min(y1, y2);
  for (int i = 0; i < dist; i++) {
    update_screen[(y + i) * gridLength + x1] = currStrokeColor;
  }
}

void horizontalLine(int y1, int x1, int x2) {

  int dist = abs(x2 - x1) + 1;
  int x = std::min(x1, x2);
  for (int i = 0; i < dist; i++) {
    update_screen[y1 * gridLength + ((x + i))] = currStrokeColor;
  }
}

void line() {
 
    int dx = lcd_x2 - lcd_x1;
    int dy = lcd_y2 - lcd_y1;

    /*
     * Use corresponding cardinal line method if slope is 0 or infinite
     */
    if (dx == 0) {
      verticalLine(lcd_x1, lcd_y1, lcd_y2);
      return;
    } else if (dy == 0) {
      horizontalLine(lcd_y1, lcd_x1, lcd_x2);
      return;
    }

    // Determine the direction of movement
    int sx = (dx > 0) ? 1 : -1;
    int sy = (dy > 0) ? 1 : -1;

    int abs_dx = abs(dx);
    int abs_dy = abs(dy);

    int err = abs_dx - abs_dy;
    int x = lcd_x1, y = lcd_y1;

    while (x != lcd_x2 || y != lcd_y2) {
      update_screen[y * gridLength + x] = currStrokeColor;
      int double_err = 2 * err;

      // Step along the x-axis
      if (double_err > -abs_dy) {
        x += sx;
        err -= abs_dy;
      }

      // Step along the y-axis
      if (double_err < abs_dx) {
        err += abs_dx;
        y += sy;
      }
    }
    update_screen[lcd_y2 * gridLength + lcd_x2] = currStrokeColor;
}

/*
* Draw rectangle with corners at (x1,y2) & (x2,y2)
*/
void rect() {

  /* Draw Fill lines */
  if (drawFill) {
    int min_x = std::min(lcd_x1,lcd_x2);
    int max_x = std::max(lcd_x1,lcd_x2);

    int min_y = std::min(lcd_y1,lcd_y2);
    int max_y = std::max(lcd_y1,lcd_y2);

    uint16_t temp = currStrokeColor;
    currStrokeColor = currFillColor;
    for (int i = 0; i < std::abs(max_y-min_y)+1; i++) {
      horizontalLine((min_y) + i, (min_x), (max_x));
    }
    currStrokeColor = temp;
  }

   /* Draw Stroke Lines */
  if (drawStroke) {
    horizontalLine(lcd_y1, lcd_x1, lcd_x2);
    horizontalLine(lcd_y2, lcd_x1, lcd_x2);
    verticalLine(lcd_x1, lcd_y1, lcd_y2);
    verticalLine(lcd_x2, lcd_y1, lcd_y2);
  }
  

}

/* append values to a buffer 
*  
*  if a push is attempted but the buffer if full 
*  it will pop off the most recent value to allow 
*  for new inputs to be later pushed  
*/
void buffer_push(int buffer[],int len,int num){

		int index;
		for(index = (len-1); index > -1; index--){

			  if(buffer[index] == 0) break;
		}
		if(index >= 0){
      buffer[index] = num;
      return;
    } 
    buffer_pop(buffer,len);
}

int buffer_pop(int buffer[],int len){
	int num = buffer[0];
	for(int i = 0; i < (len-1); i++){
		buffer[i] = buffer[i + 1];
	}
	buffer[len-1] = 0;
	return num;
}

/* processes key-board inputs and appends them to buffer */
void process_keyboard_input(Tigr* win){

  char keys[] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
  for(char *c = keys; *c != '\0'; c++){
      char key = *c;
      int hit = tigrKeyHeld(win, key);
      if(hit){
          buffer_push(keyboard_input_buffer,KEYBOARD_INPUT_BUFFER_SIZE,key);
      }
  }
}

/* Render Pixels using live screen */
void render() {

  for (int y = 0; y < gridLength; y++) {
    for (int x = 0; x < gridLength; x++) {
      int index = y * gridLength + x;
      fill(live_screen[index]);
      tigrFill(window, x * pixelSize, y * pixelSize, pixelSize, pixelSize, currFill);
    }
  }
  tigrUpdate(window);
}

/* Render Pixels and turn live_screen into update_screen */
void update() {

  for (int y = 0; y < gridLength; y++) {
    for (int x = 0; x < gridLength; x++) {
      int index = y * gridLength + x;
      live_screen[index] = update_screen[index];
    }
  }
  render();
}

void initScreen(double v) {

  hue = v;
  srand(time(nullptr));
  window = tigrWindow(windowWidth, windowHeight, "TeenyAT LCD", TIGR_FIXED);
  /* Create renderer and initialize update_screen to hue */
  for (int i = 0; i < gridLength * gridLength; i++) {
    live_screen[i] = hue;
    update_screen[i] = hue;
  }
  render();
}