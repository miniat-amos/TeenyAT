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

int x1 = 0;
int y1 = 0;
int x2 = 0;
int y2 = 0;

int mouseX = 0;
int mouseY = 0;
int mouseButton = 0;

int last_bus = 0;
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
  int index = y1 * gridLength + x1;
  update_screen[index] = currStrokeColor;
}

uint16_t pointColor(){
  int index = y1 * gridLength + x1;
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
 
    int dx = x2 - x1;
    int dy = y2 - y1;

    /*
     * Use corresponding cardinal line method if slope is 0 or infinite
     */
    if (dx == 0) {
      verticalLine(x1, y1, y2);
      return;
    } else if (dy == 0) {
      horizontalLine(y1, x1, x2);
      return;
    }

    // Determine the direction of movement
    int sx = (dx > 0) ? 1 : -1;
    int sy = (dy > 0) ? 1 : -1;

    int abs_dx = abs(dx);
    int abs_dy = abs(dy);

    int err = abs_dx - abs_dy;
    int x = x1, y = y1;

    while (x != x2 || y != y2) {
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
    update_screen[y2 * gridLength + x2] = currStrokeColor;
}

/*
* Draw rectangle with corners at (x1,y2) & (x2,y2)
*/
void rect() {

  /* Draw Stroke Lines */
  if (drawStroke) {
    horizontalLine(y1, x1, x2);
    horizontalLine(y2, x1, x2);
    verticalLine(x1, y1, y2);
    verticalLine(x2, y1, y2);
  }
  int h = abs(y2 - y1);
  /* Draw Fill lines */
  if (drawFill) {
    uint16_t temp = currStrokeColor;
    currStrokeColor = currFillColor;
    for (int i = 0; i < h - 1; i++) {
      horizontalLine((y1 + 1) + i, (x1 + 1), (x2 - 1));
    }
    currStrokeColor = temp;
  }

}

/* processes key-board inputs and appends them to buffer */
int process_keyboard_input(Tigr* win){
        char key = 'A';
        int flag = 0;
        for(key; key <= 'Z'; key++){
            int hit = tigrKeyHeld(win, key);
            if(hit){
                flag = 1;
                buffer_push(keyboard_input_buffer,KEYBOARD_INPUT_BUFFER_SIZE,key);
            }
        }
        return flag;
}

void buffer_push(int buffer[],int len,int num){
		int index;
		for(index = 0; index < len; index++){
			if(buffer[index] == 0){
				break;
			}
		}
		if(index >= len) index--;
		buffer[index] = num;
}

int buffer_pop(int buffer[],int len){
	int num = buffer[0];
	for(int i = 0; i < (len-1); i++){
		buffer[i] = buffer[i + 1];
	}
	buffer[len-1] = 0;
	return num;
}

void buffer_print(int buffer[], int len){
	for(int i = 0; i < len; i++ ){
		printf("%d ", buffer[i]);
	}
	printf("\n");		
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
  window = tigrWindow(windowWidth, windowHeight, "Test Program", 0);
  /* Create renderer and initialize update_screen to hue */
  for (int i = 0; i < gridLength * gridLength; i++) {
    live_screen[i] = hue;
    update_screen[i] = hue;
  }
  render();
}