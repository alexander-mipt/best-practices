#include <assert.h>
#include "controller.h"
#include "defines.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>


long long threshold = LLONG_MAX - 1000;

Controller createController(int maxNumberOfSpirals) {
    assert(maxNumberOfSpirals > 0);

    Controller result;
    result.screen = createScreen();
    result.maxNumberOfSpirals = maxNumberOfSpirals;
    result.spirals = (Spiral *) calloc(maxNumberOfSpirals, sizeof(Spiral));
    assert(result.spirals);
    result.isOccupied = (char *) calloc(maxNumberOfSpirals, sizeof(char));
    assert(result.isOccupied);
    result.timeIndex = 0;

    return result;
}

void deleteController(Controller *controller) {
    deleteScreen(&controller->screen);
    if (controller->spirals) {
        free(controller->spirals);
    }
    if (controller->isOccupied) {
        free(controller->isOccupied);
    }
    controller->timeIndex = 0;
}

void incrementPicture(Controller *controller) {
    static const int traceLength = 7;

    paintItBlack(controller->screen);
    for (int i = 0; i < controller->maxNumberOfSpirals; ++i) {
        if (controller->isOccupied[i]) {
            for (int j = 0; j < traceLength; ++j) {
                Point coordinates = incrementSpiral(controller->spirals[i], controller->timeIndex - j);
                if (coordinates.abscissa < 0) {
                    controller->isOccupied[i] = 0;
                } else {
                    short setAllLarge = 0;
                    if ((controller->spirals[i].lifeEndMoment - controller->timeIndex) * 2 > SPIRAL_LIFE_SPAN) {
                        setAllLarge = 1;
                    }

                    Point coord = {coordinates.abscissa, coordinates.ordinate};
                    if (j > 3 && setAllLarge == 1) {
                        setLargePoint(controller->screen, controller->spirals[i].color.red,
                                      controller->spirals[i].color.green, controller->spirals[i].color.blue,
                                      coord);
                    } else if (j > 1) {
                        setMediumPoint(controller->screen, controller->spirals[i].color.red,
                                       controller->spirals[i].color.green, controller->spirals[i].color.blue,
                                       coord);
                    } else {
                        setPoint(controller->screen, controller->spirals[i].color.red,
                                 controller->spirals[i].color.green, controller->spirals[i].color.blue,
                                 coord);
                    }
                }
            }
        }
    }
}

RGBPoint generateColor() {
    RGBPoint color;
    color.red = (uint8_t)((float)rand() / (float)RAND_MAX * 92) + 163;
    color.green = (uint8_t)((float)rand() / (float)RAND_MAX * 128) + 127;
    color.blue = (uint8_t)((float)rand() / (float)RAND_MAX * 128) + 127;
    color.alpha = 0;

    return color;
}

float generateParameterA() {
    return (float)rand() / (float)RAND_MAX * PARAMETER_A_FIRST + PARAMETER_A_SECOND;
}

float generateParameterB() {
    return (float)rand() / (float)RAND_MAX * PARAMETER_B_FIRST + PARAMETER_B_SECOND;
}

int generateCoordinate(int length) {
    int result = rand() % length;
    int offset = length / 7;
    if (result < offset) {
        result += offset;
    } else if (result > offset * 6) {
        result -= offset;
    }
    return result;
}

void addNewSpiral(Controller *controller) {
    // TODO: implement freeIndex parameter in controller
    for (int i = 0; i < controller->maxNumberOfSpirals; ++i) {
        if (!controller->isOccupied[i]) {
            Point centre = {generateCoordinate(SCREEN_WIDTH), generateCoordinate(SCREEN_HEIGHT)};
            printf("Adding %d, %d\n", centre.abscissa, centre.ordinate);

            controller->isOccupied[i] = 1;
            controller->spirals[i] = createSpiral(generateParameterA(), generateParameterB(),
                                                  controller->timeIndex, generateColor(),
                                                  centre);
            break;
        }
    }
}

short needToAddSpiral() {
    short result = 0;
    if ((float)rand() / (float)RAND_MAX > NEED_TO_ADD_SPIRAL) {
        result = 1;
    }
    return result;
}

int showLoadScreen(Controller *controller) {
    while (controller->timeIndex < threshold) {
        if (needToAddSpiral()) {
            addNewSpiral(controller);
        }
        int retCode = flushImage(controller->screen);
        if (retCode == -1) {
            fprintf(stderr, "Please run the app with sudo\n");
            exit(1);
        }
        incrementPicture(controller);
        ++controller->timeIndex;
    }
    controller->timeIndex = 0;
    return 0;
}
