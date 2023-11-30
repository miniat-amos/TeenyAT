/*
    Some things to remember:
        When reading in a pixel's value from memory return the update_screen 0b16 value

        ./system ../ASMCODE/pallete.asm
*/

#include <iostream>
#include <cstdlib>
#include "tigr.h"
#include "screen.h"
#include "teenyat.h"
#include "util.h"

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
#define MOUSEB 0xFFFB // return the mouse input 0->7
#define TERM 0xFFFF
#define KEY 0xFFFE

void bus_read(teenyat *t, tny_uword addr, tny_word *data, uint16_t *delay);
void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay);
bool regridLengthd = false;

// Screen to write too
int main(int argc, char *argv[])
{
    // Initialize the teenyAT
    if (argc < 2)
    {
        std::cout << "Please provide an asm file" << std::endl;
        return 1;
    }

    initScreen(2903);

    std::string fileName = argv[1];
    teenyat t;
    bool success = false;
    FILE *bin_file = fopen(fileName.c_str(), "rb");
    if (bin_file != NULL)
    {
        success = true;
        tny_init_from_file(&t, bin_file, bus_read, bus_write);
    }else{
        std::cout << "Failed to init bin file (invalid path?)" << std::endl;
        return 0;
    }

    while (!tigrClosed(window) && !tigrKeyDown(window, TK_ESCAPE))
    {

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
    // Handle pixel screen reads
    if (addr >= UPDATESCREEN && addr <= (UPDATESCREEN + 0xFFF))
    {
        int index = map(addr, UPDATESCREEN, (UPDATESCREEN + 0xFFF), 0, (gridLength * gridLength) - 1);
        data->u = update_screen[index];
        *delay = 1;
        return;
    }

    if (addr >= LIVESCREEN && addr <= (LIVESCREEN + 0xFFF))
    {
        int index = map(addr, LIVESCREEN, (LIVESCREEN + 0xFFF), 0, (gridLength * gridLength) - 1);
        data->u = live_screen[index];
        *delay = 2;
        return;
    }

    switch (addr)
    {
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
        data->u = (uint16_t)(mouseX / res);
        break;
    case MOUSEY:
        data->u = (uint16_t)(mouseY / res);
        break;
    case MOUSEB:
        data->u = mouseButton;
        break;
    default:
        // apply lag spike for a read from an unused address
        data->u = 0;
        *delay = 20;
        break;
    }
    return;
}

void bus_write(teenyat *t, tny_uword addr, tny_word data, uint16_t *delay)
{
    *delay = 0;
    // Handle pixel screen writes
    if (addr >= UPDATESCREEN && addr <= (UPDATESCREEN + 0xFFF))
    {
        int index = map(addr, UPDATESCREEN, (UPDATESCREEN + 0xFFF), 0, (gridLength * gridLength) - 1);
        update_screen[index] = data.u;
        *delay = 0;
        return;
    }

    if (addr >= LIVESCREEN && addr <= (LIVESCREEN + 0xFFF))
    {
        int index = map(addr, LIVESCREEN, (LIVESCREEN + 0xFFF), 0, (gridLength * gridLength) - 1);
        live_screen[index] = data.u;
        *delay = 2;
        render();
        return;
    }

    switch (addr)
    {
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
        // apply a lag spike for a write to an unused address
        *delay = 20;
        break;
    }
    return;
}
