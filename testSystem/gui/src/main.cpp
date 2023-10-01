/*
    Some things to remember:    
        When reading in a pixels value from memory return the update_screen 16b value


*/

#include <iostream>
#include <SDL2/SDL.h>
#include "color.h"


// Some globals (so scary)

// How many pixels to draw size*size
const int size = 64;
bool drawStroke = 1;
bool drawFill  = 1;
int WIDTH = 400;
int HEIGHT = 400;
uint16_t currStrokeColor = 0;
uint16_t currFillColor = 0;

// Resoulution of each pixel
double res = (double) WIDTH / size;

void pixel(SDL_Renderer* renderer,int x, int y,int w, int h){
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    // Render rect
    SDL_RenderFillRect( renderer, &r );
}

void noFill(){
    drawFill = 0;
}

void noStroke(){
    drawStroke = 0;
}

// Colors in HSV (im not getting the right colors!!!)
// Calling both fill and stroke sets their cooresponding booleans true
void fill(SDL_Renderer* renderer, uint16_t col){
    // default saturation & value are 50%
    drawFill = 1;
    currFillColor = col;
    unsigned char r,g,b;
    unsigned char a = 255;
    col = col % 360;
    convertHSVtoRGB(col, 54, 100, &r, &g, &b);
    //std::cout << (int)r << " " << (int)g << " " << (int)b << std::endl;
    SDL_SetRenderDrawColor( renderer, r, g, b, a);
}

void stroke(SDL_Renderer* renderer, uint16_t col){
    // default saturation & value are 50%
    drawStroke = 1;
    currStrokeColor = col;
    unsigned char r,g,b;
    unsigned char a = 255;
    col = col % 360;
    convertHSVtoRGB(col, 54, 100, &r, &g, &b);
    //std::cout << (int)r << " " << (int)g << " " << (int)b << std::endl;
    SDL_SetRenderDrawColor( renderer, r, g, b, a);
}

// Set the x1,x2,y1,y2 values
void setVal(int val,int * ret){
    *ret = (val%64); 
}

// Fast bressenham and vertical line drawer 
// (The second point needs to be greater than the first)
void verticalLine(uint16_t src[],int x1, int y1,int y2){
    int dist = y2 - y1;
    for(int i = 0; i < dist; i++){
        src[(y1+i) * size + x1] = currFillColor;
    }
}

void bresenham(uint16_t src[],int x1, int y1, int x2, int y2)
{   
    if(x1 == x2){
        verticalLine(src, x1, y1, y2);
        return;
    }
    int m_new = 2 * (y2 - y1);
    int slope_error_new = m_new - (x2 - x1);
    for (int x = x1, y = y1; x <= x2; x++) {
        
        src[y * size + x] = currFillColor;

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

// Draw rectangle centered at corner (x1,y1) {technically takes in a 16bit width value}
void rect(uint16_t src[],int x1, int y1,int w){
        int _x2 = x1 + w;
        if((_x2 >= size)) _x2 = (size-1);

        int _y2 = y1 + w;
        if((_y2 >= size)) _y2 = (size-1);
        
        // Draw Stroke Lines
        if(drawStroke){
            uint16_t temp = currFillColor;
            currFillColor = currStrokeColor;
            bresenham(src, x1, y1, _x2, y1);
            bresenham(src, _x2, y1, _x2, _y2);
            bresenham(src, x1, _y2,_x2, _y2);
            bresenham(src, x1, y1, x1, _y2);
            currFillColor = temp;
        }
        // Draw Fill lines
        if(drawFill){
            for(int i = 0; i < w-1; i++){
                bresenham(src, (x1+1), (y1+1)+i, (_x2-1), (y1+1)+i);
            }
        }

}


// update game and draw all pixels 
void update(SDL_Renderer* renderer, int size,uint16_t dest[],uint16_t src[]){
    for(int y = 0; y < size; y++){
        for(int x = 0; x < size; x++){
            int index = y * size + x;
            dest[index] = src[index];
            fill(renderer,dest[index]);
            pixel(renderer,x*res,y*res,res+1,res+1);
        }
    }
    SDL_RenderPresent(renderer);
}

int main( int argc, char* argv[]){
    
    SDL_Init( SDL_INIT_EVERYTHING   );

    SDL_Window *window;
    window = SDL_CreateWindow("Test System GUI", 
                                SDL_WINDOWPOS_UNDEFINED, 
                                SDL_WINDOWPOS_UNDEFINED, 
                                WIDTH, 
                                HEIGHT, 
                                SDL_WINDOW_RESIZABLE);
    if(NULL == window){
        std::cout << "Could not create window" << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_GetWindowSize(window, &WIDTH, &HEIGHT);

    SDL_Renderer *renderer;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    //Start 0x8000 - 0x8FFF
    uint16_t live_screen[size*size] = {0};

    //Start 0x9000 - 0x9FFF
    uint16_t update_screen[size*size] = {0};

    SDL_Event windowEvent;
    int mouseX = 0;
    int mouseY = 0;
    
    int x1 = 25;
    int y1 = 20;
    int x2 = 0;
    int y2 = 0;

    
    //Init the Scene
    stroke(renderer,90);
    fill(renderer,190);
    rect(update_screen,x1,y1,10);
    update(renderer,size,live_screen,update_screen);
    
    while(SDL_PollEvent(&windowEvent) == 0 || windowEvent.type != SDL_QUIT){
        SDL_GetMouseState(&mouseX, &mouseY);
        // This is where i'll handle the teenyAT instructions
    }


    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

