#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <ctime>
#include <sstream>
#include <SDL2/SDL.h>
#include "Color.h"

class Screen{
    public: 
    static const int size = 64;
    bool drawStroke = 1;
    bool drawFill  = 1;
    int WIDTH = 640;
    int HEIGHT = 640;
    uint16_t currStrokeColor = 0;
    uint16_t currFillColor = 0;
    // 354 - lightLavender 
    int hue = 354;
    int sat = 1150;
    int val = 200;
    // Resoulution of each pixel
    double res = (double) WIDTH / size;
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Event windowEvent;

    int mouseX = 0;
    int mouseY = 0;
    
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    //Start 0x8000 - 0x8FFF
    uint16_t live_screen[size*size] = {0};

    //Start 0x9000 - 0x9FFF
    uint16_t update_screen[size*size] = {0};

    Screen(){

        srand(time(nullptr));
        SDL_Init( SDL_INIT_EVERYTHING   );
        window = SDL_CreateWindow("Test System GUI", 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SDL_WINDOWPOS_UNDEFINED, 
                                WIDTH, 
                                HEIGHT,SDL_WINDOW_ALLOW_HIGHDPI);
        if(NULL == window){
            std::cout << "Could not create window" << SDL_GetError() << std::endl;
        }

        // Create renderer and initialize update_screen to lavender
        renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
        for(int i = 0; i < size*size; i++){
            update_screen[i] = hue;
        }
        update();
    }

    void pixel(int x, int y,int w, int h){
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = w;
        r.h = h;
        // Render rect
        SDL_RenderFillRect(renderer, &r );
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

    // Colors in HSV (im not getting the right colors!!!)
    // Calling both fill and stroke sets their cooresponding booleans true
    void fill(uint16_t col){
        // default saturation & value are 50%
        drawFill = 1;
        currFillColor = col;
        unsigned char r,g,b;
        unsigned char a = 255;
        col = col % 360;
        //std::cout << col << " " << 50 << " " << 55 << std::endl;
        convertHSVtoRGB(col, sat, val, &r, &g, &b);
        //std::cout << (int)r << " " << (int)g << " " << (int)b << std::endl;
        SDL_SetRenderDrawColor( renderer, r, g, b, a);
    }

    void stroke(uint16_t col){
        // default saturation & value are 50%
        drawStroke = 1;
        currStrokeColor = col;
        unsigned char r,g,b;
        unsigned char a = 255;
        col = col % 360;
        convertHSVtoRGB(col, sat, val, &r, &g, &b);
        //std::cout << (int)r << " " << (int)g << " " << (int)b << std::endl;
        SDL_SetRenderDrawColor( renderer, r, g, b, a);
    }

    // Set the x1,x2,y1,y2 values
    void setVal(int val,int * ret){
        *ret = (val%64); 
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
        if(x1 > x2){
                x1 = x2;
                x2 = temp1;
        }
        uint16_t temp = currFillColor;
        currFillColor = currStrokeColor;
        bresenham(x1,y1,x2,y2);
        currFillColor = temp;
        x1 = temp1;
        x2 = temp2;
       }
    }


    // Draw rectangle centered at corner (x1,y1) {technically takes in a 16bit width value}
    void rect(int w){
            int _x2 = x1 + w;
            if((_x2 >= size)) _x2 = (size-1);

            int _y2 = y1 + w;
            if((_y2 >= size)) _y2 = (size-1);
            
            // Draw Stroke Lines
            if(drawStroke){
                uint16_t temp = currFillColor;
                currFillColor = currStrokeColor;
                bresenham(x1, y1, _x2, y1);
                bresenham(_x2, y1, _x2, _y2);
                bresenham(x1, _y2,_x2, _y2);
                bresenham(x1, y1, x1, _y2);
                currFillColor = temp;
            }
            // Draw Fill lines
            if(drawFill){
                for(int i = 0; i < w-1; i++){
                    bresenham((x1+1), (y1+1)+i, (_x2-1), (y1+1)+i);
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
                fill(live_screen[index]);
                pixel(x*res,y*res,res,res);
            }
        }
        SDL_RenderPresent(renderer);
    }




};

#endif