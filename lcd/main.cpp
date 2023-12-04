#include <iostream>
#include <cstdlib>
#include "tigr.h"
#include "screen.h"
#include "teenyat.h"
#include "util.h"

/*
 *  These are the start of the address ranges or the addresses themselves: 
 *  See .... document for more detail 
 * 
 *  LIVESCREENxxxx: 
 *  - 0x8000 -- 0x8FFF each address is 1 pixel in a 64x64 lcd grid
 *  - read/write
 *  - any writes to this address automatically renders to the lcd
 *  
 *  UPDATESCREEN:
 *  - 0x9000 -- 0x9FFF each address is 1 pixel in a 64x64 lcd grid
 *  - read/write
 *  - any reads/writes to the update screen work with an off screen buffer
 *  
 *  X1:
 *  - 0xD000
 *  - read/write
 * 
 *  Y1:
 *  - 0xD001
 *  - read/write
 *  
 *  X2:
 *  - 0xD002
 *  - read/write
 * 
 *  Y2:
 *  - 0xD003
 *  - read/write
 *  
 *  STROKE:
 *  - 0xD010
 *  - read/write
 *  - on write sets the stroke color used by other memory addresses
 *  - on read sends back the current stroke color
 * 
 *  FILL:
 *  - 0xD011
 *  - read/write
 *  - on write sets the current fill color used by other memory addresses
 *  - on read sends back the current fill color
 *  
 *  DRAWFILL:
 *  - 0xD012
 *  - read/write
 *  - if non zero will draw fill 
 * 
 *  DRAWSTROKE:
 *  - 0xD013
 *  - read/write
 *  - if non zero will draw srtoke 
 * 
 *  RAND:
 *  - 0xD700
 *  - read only
 *  - returns an ensured 16 bit random number
 * 
 * 
 */

#define LIVESCREEN_START 0x8000 
#define LIVESCREEN_END 0x8FFF 
#define UPDATESCREEN_START 0x9000
#define UPDATESCREEN_END 0x9FFF
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
#define POINT 0xE012 
#define MOUSEX 0xFFFC
#define MOUSEY 0xFFFD
#define MOUSEB 0xFFFB 
#define TERM 0xFFFF
#define KEY 0xFFFE

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay);
void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay);

int main(int argc, char *argv[])
{   
    if(argc < 2) {
        std::cout << "Please provide an asm file" << std::endl;
        return 1;
    }

    /* database fabulous lavender for our color model */
    initScreen(0xA81); 

    std::string fileName = argv[1];
    teenyat t;
    bool success = false;
    FILE *bin_file = fopen(fileName.c_str(), "rb");
    if(bin_file != NULL) {
        success = true;
        tny_init_from_file(&t, bin_file, bus_read, bus_write);
    }else {
        std::cout << "Failed to init bin file (invalid path?)" << std::endl;
        return 0;
    }

    while(!tigrClosed(window) && !tigrKeyDown(window, TK_ESCAPE)) {

        tigrMouse(window, &mouseX, &mouseY, &mouseButton);
        tny_clock(&t);
    }

    tigrFree(window);
    fclose(bin_file);
    return EXIT_SUCCESS;
}

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay)
{
    *delay = 0;

    /* Handles pixel screen reads */
    if(addr >= UPDATESCREEN_START && addr <= UPDATESCREEN_END) {
        int index = map(addr, UPDATESCREEN_START, UPDATESCREEN_END, 0, (gridLength * gridLength) - 1);
        data->u = update_screen[index];
        *delay = 1;
        return;
    }

    if(addr >= LIVESCREEN_START && addr <= LIVESCREEN_END) {
        int index = map(addr, LIVESCREEN_START, LIVESCREEN_END, 0, (gridLength * gridLength) - 1);
        data->u = live_screen[index];
        *delay = 2;
        return;
    }

    switch(addr) {
    case X1:
        data->u = x1;
        break;
    case Y1:
        data->u = y1;
        break;
    case X2:
        data->u = x2;
        break;
    case Y2:
        data->u = y2;
        break;
    case STROKE:
        data->u = currStrokeColor;
        break;
    case FILL:
        data->u = currFillColor;
        break;
    case DRAWFILL:
        data->u = drawFill;
        break;
    case DRAWSTROKE:
        data->u = drawStroke;
        break;
    case RAND:
        data->u = rand16();
        break;
    case MOUSEX:
        data->u = (uint16_t)(mouseX / pixelSize);
        break;
    case MOUSEY:
        data->u = (uint16_t)(mouseY / pixelSize);
        break;
    case MOUSEB:
        data->u = mouseButton;
        break;
    default:
        // apply lag spike for a read from an unused addpixelSizes
        data->u = 0;
        *delay = 20;
        break;
    }
    return;
}

void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay)
{
    *delay = 0;
    
    if(addr >= UPDATESCREEN_START && addr <= UPDATESCREEN_END) {
        int index = map(addr, UPDATESCREEN_START, UPDATESCREEN_END, 0, (gridLength * gridLength) - 1);
        update_screen[index] = data.u;
        *delay = 0;
        return;
    }

    if(addr >= LIVESCREEN_START && addr <= LIVESCREEN_END) {
        int index = map(addr, LIVESCREEN_START, LIVESCREEN_END, 0, (gridLength * gridLength) - 1);
        live_screen[index] = data.u;
        *delay = 2;
        render();
        return;
    }

    switch(addr) {
    case X1:
        setVal(data.u, &x1);
        break;
    case Y1:
        setVal(data.u, &y1);
        break;
    case X2:
        setVal(data.u, &x2);
        break;
    case Y2:
        setVal(data.u, &y2);
        break;
    case STROKE:
        setStroke(data.u);
        break;
    case FILL:
        setFill(data.u);
        break;
    case DRAWFILL:
        setVal(data.u, &drawFill);
        break;
    case DRAWSTROKE:
        setVal(data.u, &drawStroke);
        break;
    case UPDATE:
        update();
        break;
    case RECT:
        rect();
        break;
    case LINE:
        line();
        break;
    case POINT:
        point();
        break;
    default:
        // apply a lag spike for a write to an unused addpixelSizes
        *delay = 20;
        break;
    }
    return;
}
