#pragma once

#include "point.h"


typedef struct {
    float a;
    float b;
    long long lifeEndMoment;
    RGBPoint color;
    Point coordinates;
} Spiral;


extern Spiral createSpiral(float a, float b, long long currentMoment, RGBPoint color, Point centre);

extern Point incrementSpiral(Spiral spiral, long long currentMoment);
