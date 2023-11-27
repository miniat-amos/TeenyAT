#ifndef SCREEN_H
#define SCREEN_H

#include <iostream>
#include <ctime>
#include <sstream>
#include <cstdint>
#include <vector>
#include "tigr.h"
#include "color.h"

class Screen{
    public: 
    static const int size = 64;

    Tigr* window;
    int drawStroke = 1;
    int drawFill  = 1;
    int Window_WIDTH = 640;
    int Window_HEIGHT = 640;
    uint16_t currStrokeColor = 0;
    uint16_t currFillColor = 0;

    // 354 - lightLavender 
    double hue = 3636;

    // 1150 - orig
    double sat = 27;
    double val = 100;

    // Resoulution of each pixel
    double res = (double) Window_WIDTH / size;
    int mouseX = 0;
    int mouseY = 0;
    int mouseButton = 0;

    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    TPixel currFill = TPixel{0,0,0,255};
    TPixel currStroke = TPixel{0,0,0,255};

    //Start 0x8000 - 0x8FFF
    uint16_t live_screen[size*size] = {0};

    //Start 0x9000 - 0x9FFF
    uint16_t update_screen[size*size] = {0};

    Screen(double v){
        hue = v;
        srand(time(nullptr));
        window = tigrWindow(Window_WIDTH, Window_HEIGHT, "Test Program", 0);
        // Create renderer and initialize update_screen to lavender
        for(int i = 0; i < size*size; i++){
            update_screen[i] = hue;
        }
        update();
        render();
    }

    void point(){
        int index = y1 * size + x1;
        update_screen[index] = currStrokeColor;
    }

    uint16_t rand16(){
        return (uint16_t) (rand() << 1) ^ rand();
    }

    void noFill(){
        drawFill = 0;
    }

    void noStroke(){
        drawStroke = 0;
    }

    void setFill(uint16_t col){
        drawFill = 1;
        currFillColor = col;
    }

    void setStroke(uint16_t col){
        drawStroke = 1;
        currStrokeColor = col;
    }

    // Colors in HSV (im not getting the right colors!!!)
    // Calling both fill and stroke sets their cooresponding booleans true
    void fill(uint16_t col){
        // default saturation & value are 50%
        unsigned char r,g,b;
        unsigned char a = 255;
        //std::cout << col << " " << 50 << " " << 55 << std::endl;
        HSVtoRGB(col, &r, &g, &b);
        currFill = TPixel{r,g,b,a};
        //std::cout << (int)r << " " << (int)g << " " << (int)b << std::endl;
    }

    void stroke(uint16_t col){
        // default saturation & value are 50%
        unsigned char r,g,b;
        unsigned char a = 255;
        HSVtoRGB(col, &r, &g, &b);
        currStroke = TPixel{r,g,b,a};
        //std::cout << (int)r << " " << (int)g << " " << (int)b << std::endl;
    }

    // Set the x1,x2,y1,y2 values
    void setVal(int val,int * ret){
        *ret = (val%(size)); 
    }

    // Fast bressenham and vertical line drawer 
    // (The second point needs to be greater than the first)
    void verticalLine(int x1, int y1,int y2){
        int dist = y2 - y1;
        for(int i = 0; i < dist; i++){
            update_screen[(y1+i) * size + x1] = currFillColor;
        }
    }

    void bresenham(int x1, int y1, int x2, int y2)
    {   
        if(x1 == x2){
            verticalLine(x1, y1, y2);
            return;
        }

        // Positive Slopes
        int m_new = 2 * (y2 - y1);
        int slope_error_new = m_new - (x2 - x1);
        for (int x = x1, y = y1; x <= x2; x++) {
            
            update_screen[y * size + x] = currFillColor;

            // Add slope to increment angle formed
            slope_error_new += m_new;
    
            // Slope error reached limit, time to
            // increment y and update slope error.
            if (slope_error_new >= 0) {
                y++;
                slope_error_new -= 2 * (x2 - x1);
            }
        }
    }

    void line()
    {   
       if(drawStroke){

        // prevent bad values
        int temp1 = x1;
        int temp2 = x2;
        int temp3 = y1;
        int temp4 = y2;
        if(x1 > x2){
                x1 = x2;
                x2 = temp1;
                y1 = y2;
                y2 = temp3;
        }
        if(x1 == x2 && y1 != y2){
            verticalLine(x1,y1,y2);
            return;
        }
        int m = (y2 - y1) / (x2 - x1);
        int x = x1;
        int y = y1;
        update_screen[y * size + x] = currStrokeColor;
        update_screen[y2 * size + x2] = currStrokeColor;
        while(x <= x2){
            y = (int)(m * x) + y1;
            //  std::cout << x << ' ' << y << std::endl;
            update_screen[y * size + x] = currStrokeColor;
            x++;
        }

        x1 = temp1;
        x2 = temp2;
        y1 = temp3;
        y2 = temp4;

       }
    }


    // Draw rectangle centered at corner (x1,y1) {technically takes in a 16bit width value}
    void rect(){
            // Draw Stroke Lines
            if(drawStroke){
                uint16_t temp = currFillColor;
                currFillColor = currStrokeColor;
                bresenham(x1, y1, x2, y1);
                bresenham(x2, y1, x2, y2);
                bresenham(x1, y2, x2, y2);
                bresenham(x1, y1, x1, y2);
                currFillColor = temp;
            }
            int h = abs(y2-y1);
            // Draw Fill lines
            if(drawFill){
                for(int i = 0; i < h-1; i++){
                    bresenham((x1+1), (y1+1)+i, (x2-1), (y1+1)+i);
                }
            }

    }

    // update game and draw all pixels 
    void update(){
        // Render Pixels and turn live_screen into update_screen
        for(int y = 0; y < size; y++){
            for(int x = 0; x < size; x++){
                int index = y * size + x;
                live_screen[index] = update_screen[index];
            }
        }
        render();
    }

    void render(){
        // Render Pixels and turn live_screen into update_screen
        for(int y = 0; y < size; y++){
            for(int x = 0; x < size; x++){
                int index = y * size + x;
                fill(live_screen[index]);
                tigrFill(window,x*res,y*res,res,res,currFill);
            }
        }
        tigrUpdate(window);
    }
};

#endif