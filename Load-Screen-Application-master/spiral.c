#include "defines.h"
#include <math.h>
#include "spiral.h"


Spiral createSpiral(float a, float b, long long currentMoment, RGBPoint color, Point centre) {
    Spiral result = {a, b, currentMoment + SPIRAL_LIFE_SPAN, color, centre};
    return result;
}

Point incrementSpiral(Spiral spiral, long long currentMoment) {
    Point result = {-1, -1};

    if (spiral.lifeEndMoment > currentMoment) {
        /**
         * Rules for point incrementation
         * b << 1.0
         * x = a * t * cos(b * t)
         * y = a * t * sin(b * t)
         */
        float delta = (float)(spiral.lifeEndMoment - currentMoment);
        result.abscissa = spiral.coordinates.abscissa + (int)(spiral.a * delta * cosf(spiral.b * delta));
        result.ordinate = spiral.coordinates.ordinate + (int)(spiral.a * delta * sinf(spiral.b * delta));
//        printf("Step to point (%d, %d) for spiral %lld\n", result.abscissa,
//               result.ordinate, spiral.lifeEndMoment);
    }
    return result;
}
