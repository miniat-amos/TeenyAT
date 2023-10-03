/*
    Some things to remember:    
        When reading in a pixels value from memory return the update_screen 16b value


*/

#include <iostream>
#include <SDL2/SDL.h>
#include "Screen.h"

#define LIVESCREEN 0x8000
#define UPDATESCREEN 0x9000
#define X1 0xD000
#define Y1 0xD001
#define X2 0xD002
#define Y2 0xD003
#define STROKE 0xD010
#define FILL 0xD011
#define DRAWFILL 0xD012
#define DRAWSTROKE 0xD013
#define RAND 0xD700
#define UPDATE 0xE000
#define RECT 0xE010
#define LINE 0xE011
#define MOUSEX 0xFFFC
#define MOUSEY 0xFFFD
#define TERM 0xFFFF
#define KEY 0xFFFE

int main( int argc, char* argv[]){
   
    Screen s;

    //Initial the Scene
    s.setVal(25,&s.x1);
    s.setVal(25,&s.y1);

    s.stroke(0);
    s.fill(190);
    s.rect(10);


    s.setVal(0,&s.x1);
    s.setVal(0,&s.y1);
    s.setVal(63,&s.x2);
    s.setVal(63,&s.y2);

    s.stroke(100);
    s.line();

    s.setVal(63,&s.x1);
    s.setVal(0,&s.y1);
    s.setVal(0,&s.x2);
    s.setVal(63,&s.y2);

    s.stroke(100);
    s.line();

    s.update();
    
    while(SDL_PollEvent(&s.windowEvent) == 0 || s.windowEvent.type != SDL_QUIT){
        SDL_GetMouseState(&s.mouseX, &s.mouseY);
    }

    SDL_DestroyWindow(s.window);
    SDL_Quit();

    return EXIT_SUCCESS;
}