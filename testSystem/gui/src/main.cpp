/*
    Some things to remember:    
        When reading in a pixel's value from memory return the update_screen 0b16 value

        ./system ../ASMCODE/pallete.asm
*/

#include <iostream>
#include <cstdlib>
#include "../tigr.h"
#include "Screen.h"
#include "../../../teenyat.h"

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
#define POINT 0xE012 // use x1 & y1 and stroke color
#define MOUSEX 0xFFFC
#define MOUSEY 0xFFFD
#define TERM 0xFFFF
#define KEY 0xFFFE

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay);
void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay);
bool resized = false;

// Screen to write too
int main( int argc, char* argv[]){
    //Initialize the teenyAT
    if(argc < 2){
        std::cout << "Please provide an asm file" << std::endl;
        return 1;
    }

    Screen s(40000); 

    std::string fileName = argv[1];
    teenyat t;
    bool success = false;
	FILE *bin_file = fopen(fileName.c_str(), "rb");
	if (bin_file != NULL) {
		success = true;
		tny_init_from_file(&t, bin_file, bus_read, bus_write);
	}
	t.ex_data = &s;

    while (!tigrClosed(s.window) && !tigrKeyDown(s.window, TK_ESCAPE)){
        
        tigrMouse(s.window,&s.mouseX, &s.mouseY, nullptr);
        //std::cout << s.mouseX << " " << s.mouseY << std::endl;
        tny_clock(&t);
    }

    tigrFree(s.window);
	fclose(bin_file);
    return EXIT_SUCCESS;
}

// Used to map address space to grid space
int map(int num, int in_min, int in_max, int out_min, int out_max){
    return (int)(num - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay) {
	
    Screen *s = (Screen *)t->ex_data;
	*delay = 0;  
    if (s == NULL)
		return;
    std::cout << " read";
	std::cout << "(" << std::hex << addr << ")";

     // Handle pixel screen reads
    if(addr >= UPDATESCREEN && addr <= (UPDATESCREEN + 0xFFF)){
        int index = map(addr,UPDATESCREEN,(UPDATESCREEN + 0xFFF),0,(s->size*s->size)-1);
        data->u = s->update_screen[index];
        *delay = 1;
        std::cout << "update_screen" << std::endl;
        return;
    }

    if(addr >= LIVESCREEN && addr <= (LIVESCREEN + 0xFFF)){
        int index = map(addr,LIVESCREEN,(LIVESCREEN + 0xFFF),0,(s->size*s->size)-1);
        data->u = s->live_screen[index];
        *delay = 2;
        std::cout << "live_screen" << std::endl;
        return;
    }

    switch (addr) {
        case X1:
            data->u = s->x1;
            std::cout << "  x1";
            break;
        case Y1:
            data->u = s->y1;
            std::cout << "  y1";
            break;
        case X2:
            data->u = s->x2;
            std::cout << "  x2";
            break;
        case Y2:
            data->u = s->y2;
            std::cout << "  y2";
            break;
        case STROKE:
             data->u = s->currStrokeColor;
             std::cout << " stroke";
             break;
        case FILL:
             data->u = s->currFillColor;
             std::cout << " fill";
             break;
        case DRAWFILL:
             data->u = s->drawFill;
             std::cout << " drawFill";
             break;
        case DRAWSTROKE:
             data->u = s->drawStroke;
             std::cout << " drawStroke";
             break;
        case RAND:
             data->u = s->rand16();
             std::cout << " rand";
             break;
        case MOUSEX:
             data->u = (uint16_t)(s->mouseX/s->res);
             std::cout << " mouseX";
             break;
        case MOUSEY:
             data->u = (uint16_t)(s->mouseY/s->res);
             std::cout << " mouseY";
             break;
        default:
			//apply lag spike for a read from an unused address
			data->u = 0;
			*delay = 20;
			std::cout << " Invalid address";
			break;
             
    }
    std::cout << "" <<std::endl; 
	return;
}

void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay) {
    
    Screen *s = (Screen *)t->ex_data;
    *delay = 0;
    if (s == NULL)
		return;
    std::cout << " writ";
	std::cout << "(" << std::hex << addr << ")";

    // Handle pixel screen writes
    if(addr >= UPDATESCREEN && addr <= (UPDATESCREEN + 0xFFF)){
        int index = map(addr,UPDATESCREEN,(UPDATESCREEN + 0xFFF),0,(s->size*s->size)-1);
        s->update_screen[index] = data.u;
        *delay = 0;
        std::cout << "update_screen" << std::endl;
        return;
    }

    if(addr >= LIVESCREEN && addr <= (LIVESCREEN + 0xFFF)){
        int index = map(addr,LIVESCREEN,(LIVESCREEN + 0xFFF),0,(s->size*s->size)-1);
        s->live_screen[index] = data.u;
        *delay = 2;
        s->render();
        std::cout << "live_screen" << std::endl;
        return;
    }
    
    switch (addr) {
        case X1:
            s->setVal(data.u,&s->x1);
            std::cout << "  x1";
            break;
        case Y1:
            s->setVal(data.u,&s->y1);
            std::cout << "  y1";
            break;
        case X2:
            s->setVal(data.u,&s->x2);
            std::cout << "  x2";
            break;
        case Y2:
            s->setVal(data.u,&s->y2);
            std::cout << "  y2";
            break;
        case STROKE:
             s->setStroke(data.u);
             std::cout << " stroke";
             break;
        case FILL:
             s->setFill(data.u);
             std::cout << " fill";
             break;
        case DRAWFILL:
             s->setVal(data.u,&s->drawFill);
             std::cout << " drawFill";
             break;
        case DRAWSTROKE:
             s->setVal(data.u,&s->drawStroke);
             std::cout << " drawStroke";
             break;
        case UPDATE:
            s->update();
            std::cout << " update";
            break;
        case RECT:
            s->rect();
            std::cout << " rect";
            break;
        case LINE:
            s->line();
            std::cout << " line";
            break;
        case POINT:
             s->point();
             std::cout << " point";
             break;
        default:
			//apply a lag spike for a write to an unused address
			*delay = 20;
			std::cout << " Invalid address";
			break;
             
    }
    std::cout << std::endl;
	return;
}

