#pragma once

#include "point.h"
#include <stdint.h>


extern const char *bufferFileName;


typedef struct {
    RGBPoint *points;
    int size;
} Screen;


extern Screen createScreen();

extern void deleteScreen(Screen *screen);

extern int setPoint(Screen screen, uint8_t red, uint8_t green, uint8_t blue, Point coordinates);

extern int setMediumPoint(Screen screen, uint8_t red, uint8_t green, uint8_t blue, Point centre);

extern int setLargePoint(Screen screen, uint8_t red, uint8_t green, uint8_t blue, Point centre);

extern void paintItBlack(Screen screen);

extern int flushImage(Screen screen);
