#pragma once

#include "screen.h"
#include "spiral.h"


typedef struct {
    Screen screen;
    int maxNumberOfSpirals;
    Spiral *spirals;
    char *isOccupied;
    long long timeIndex;
} Controller;

extern long long threshold;

extern Controller createController(int maxNumberOfSpirals);

extern void deleteController(Controller *controller);

extern void incrementPicture(Controller *controller);

extern float generateParameterA();

extern float generateParameterB();

extern void addNewSpiral(Controller *controller);

extern int showLoadScreen(Controller *controller);
