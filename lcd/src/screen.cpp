#ifndef SCREEN_H
#define SCREEN_H

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
int Window_WIDTH = 640;
int Window_HEIGHT = 640;
uint16_t currStrokeColor = 0;
uint16_t currFillColor = 0;
const int gridLength = 64;
// 354 - lightLavender 
double hue = 3636;
// Resoulution of each pixel
double res = (double) Window_WIDTH / gridLength;

int x1 = 0;
int y1 = 0;
int x2 = 0;
int y2 = 0;

int mouseX = 0;
int mouseY = 0;
int mouseButton = 0;

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

//Start 0x8000 - 0x8FFF
uint16_t live_screen[gridLength * gridLength] = {
  0
};

//Start 0x9000 - 0x9FFF
uint16_t update_screen[gridLength * gridLength] = {
  0
};

void point() {
  int index = y1 * gridLength + x1;
  update_screen[index] = currStrokeColor;
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

// setFill and setStroke are used by main.cpp 
void setFill(uint16_t col) {
  drawFill = 1;
  currFillColor = col;
}

void setStroke(uint16_t col) {
  drawStroke = 1;
  currStrokeColor = col;
}

// fill and stroke are only used by the renderer
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

// Set the x1,x2,y1,y2 values
void setVal(int val, int * ret) {
  * ret = (val % (gridLength));
}

// Fast bressenham and vertical line drawer 
// (The second point needs to be greater than the first)
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
  if (drawStroke) {

    int dx = x2 - x1;
    int dy = y2 - y1;
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
}

// Draw rectangle centered at corner (x1,y1) {technically takes in a 16bit width value}
void rect() {
  // Draw Stroke Lines
  if (drawStroke) {
    horizontalLine(y1, x1, x2);
    horizontalLine(y2, x1, x2);
    verticalLine(x1, y1, y2);
    verticalLine(x2, y1, y2);
  }
  int h = abs(y2 - y1);
  // Draw Fill lines
  if (drawFill) {
    uint16_t temp = currStrokeColor;
    currStrokeColor = currFillColor;
    for (int i = 0; i < h - 1; i++) {
      horizontalLine((y1 + 1) + i, (x1 + 1), (x2 - 1));
    }
    currStrokeColor = temp;
  }

}

// renders the pixels to tigr
void render() {
  // Render Pixels and turn live_screen into update_screen
  for (int y = 0; y < gridLength; y++) {
    for (int x = 0; x < gridLength; x++) {
      int index = y * gridLength + x;
      fill(live_screen[index]);
      tigrFill(window, x * res, y * res, res, res, currFill);
    }
  }
  tigrUpdate(window);
}

// update game and draw all pixels 
void update() {
  // Render Pixels and turn live_screen into update_screen
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
  window = tigrWindow(Window_WIDTH, Window_HEIGHT, "Test Program", 0);
  // Create renderer and initialize update_screen to lavender
  for (int i = 0; i < gridLength * gridLength; i++) {
    live_screen[i] = hue;
    update_screen[i] = hue;
  }
  render();
}

#endif