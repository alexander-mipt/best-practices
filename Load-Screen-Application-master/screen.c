#include <assert.h>
#include "defines.h"
#include <fcntl.h>
#include "screen.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


const char *bufferFileName = "/dev/fb0";


Screen createScreen() {
    Screen result;
    result.points = (RGBPoint *) calloc(SCREEN_HEIGHT * SCREEN_WIDTH, sizeof(RGBPoint));
    assert(result.points);
    result.size = SCREEN_HEIGHT * SCREEN_WIDTH;

    return result;
}

void deleteScreen(Screen *screen) {
    if (screen->points) {
        free(screen->points);
    }
    screen->points = NULL;
}

int setPoint(Screen screen, uint8_t red, uint8_t green, uint8_t blue, Point coordinates) {
    /**
     * @brief Sets point given by coordinates on the given screen
     * @param screen                Screen to write on
     * @params red, green, blue     Target point's colors
     * @param coordinates           Point's coordinates
     * @returns -1 if any coordinate is negative, 0 on success, 1 in overflow case
     */
    if (coordinates.abscissa < 0 || coordinates.ordinate < 0) {
        fprintf(stderr, "Error when trying to set point (%d, %d)\n",
                coordinates.abscissa, coordinates.ordinate);
        return -1;
    }
    int index = coordinates.abscissa + coordinates.ordinate * SCREEN_WIDTH;
    if (index >= screen.size) {
        fprintf(stderr, "Screen size overflow when trying to set point (%d, %d)\n",
                coordinates.abscissa, coordinates.ordinate);
        return 1;
    }

    screen.points[index].red = red;
    screen.points[index].green = green;
    screen.points[index].blue = blue;
    return 0;
}

int setMediumPoint(Screen screen, uint8_t red, uint8_t green, uint8_t blue, Point centre) {
    if (centre.abscissa < 0 || centre.ordinate < 0) {
        fprintf(stderr, "Error when trying to set point (%d, %d)\n",
                centre.abscissa, centre.ordinate);
        return -1;
    }
    int index = centre.abscissa + centre.ordinate * SCREEN_WIDTH;
    if (index >= screen.size) {
        fprintf(stderr, "Screen size overflow when trying to set point (%d, %d)\n",
                centre.abscissa, centre.ordinate);
        return 1;
    }

    static const int mediumPointHalfNumberOfPixels = 7;
    static const int mediumPointAbscissaOffsets[7] = {0, -1, 0, 1, -2, -1, 0};
    static const int mediumPointOrdinateOffsets[7] = {-2 * SCREEN_WIDTH, -1 * SCREEN_WIDTH, -1 * SCREEN_WIDTH, -1 * SCREEN_WIDTH,
                                                     0, 0, 0};

    int pixel_index;
    for (int i = 0; i < mediumPointHalfNumberOfPixels; ++i) {
        pixel_index = index + mediumPointAbscissaOffsets[i] + mediumPointOrdinateOffsets[i];
        if (pixel_index > 0 && pixel_index < screen.size) {
            screen.points[pixel_index].red = red;
            screen.points[pixel_index].green = green;
            screen.points[pixel_index].blue = blue;
        }

        pixel_index = index - mediumPointAbscissaOffsets[i] - mediumPointOrdinateOffsets[i];
        if (pixel_index > 0 && pixel_index < screen.size) {
            screen.points[pixel_index].red = red;
            screen.points[pixel_index].green = green;
            screen.points[pixel_index].blue = blue;
        }
    }

    return 0;
}

int setLargePoint(Screen screen, uint8_t red, uint8_t green, uint8_t blue, Point centre) {
    if (centre.abscissa < 0 || centre.ordinate < 0) {
        fprintf(stderr, "Error when trying to set point (%d, %d)\n",
                centre.abscissa, centre.ordinate);
        return -1;
    }
    int index = centre.abscissa + centre.ordinate * SCREEN_WIDTH;
    if (index >= screen.size) {
        fprintf(stderr, "Screen size overflow when trying to set point (%d, %d)\n",
                centre.abscissa, centre.ordinate);
        return 1;
    }

    static const int largePointHalfNumberOfPixels = 29;
    static const int largePointAbscissaOffsets[29] = {-1, 0, 1, -2, -1, 0, 1, 2, -3, -2, -1, 0, 1, 2, 3, -4, -3, -2, -1,
                                                      0, 1, 2, 3, 4, -4, -3, -2, -1, 0};
    static const int largePointOrdinateOffsets[29] = {-4 * SCREEN_WIDTH, -4 * SCREEN_WIDTH, -4 * SCREEN_WIDTH, -3 * SCREEN_WIDTH,
                                                      -3 * SCREEN_WIDTH, -3 * SCREEN_WIDTH, -3 * SCREEN_WIDTH, -3 * SCREEN_WIDTH,
                                                      -2 * SCREEN_WIDTH, -2 * SCREEN_WIDTH, -2 * SCREEN_WIDTH, -2 * SCREEN_WIDTH,
                                                      -2 * SCREEN_WIDTH, -2 * SCREEN_WIDTH, -2 * SCREEN_WIDTH, -1 * SCREEN_WIDTH,
                                                      -1 * SCREEN_WIDTH, -1 * SCREEN_WIDTH, -1 * SCREEN_WIDTH, -1 * SCREEN_WIDTH,
                                                      -1 * SCREEN_WIDTH, -1 * SCREEN_WIDTH, -1 * SCREEN_WIDTH, -1 * SCREEN_WIDTH,
                                                      0, 0, 0, 0, 0};

    int pixel_index;
    for (int i = 0; i < largePointHalfNumberOfPixels; ++i) {
        pixel_index = index + largePointAbscissaOffsets[i] + largePointOrdinateOffsets[i];
        if (pixel_index > 0 && pixel_index < screen.size) {
            screen.points[pixel_index].red = red;
            screen.points[pixel_index].green = green;
            screen.points[pixel_index].blue = blue;
        }

        pixel_index = index - largePointAbscissaOffsets[i] - largePointOrdinateOffsets[i];
        if (pixel_index > 0 && pixel_index < screen.size) {
            screen.points[pixel_index].red = red;
            screen.points[pixel_index].green = green;
            screen.points[pixel_index].blue = blue;
        }
    }

    return 0;
}

void paintItBlack(Screen screen) {
    for (int i = 0; i < screen.size; ++i) {
        screen.points[i].red = 0;
        screen.points[i].green = 0;
        screen.points[i].blue = 0;
    }
}

int flushImage(Screen screen) {
    int bufferDescriptor = open(bufferFileName, O_WRONLY);
    if (bufferDescriptor == -1) {
        fprintf(stderr, "Failed to open /dev/fb0\n");
        return -1;
    }

    if (write(bufferDescriptor, (void*)(screen.points),screen.size * sizeof(RGBPoint)) == -1) {
        fprintf(stderr, "Failed to write into /dev/fb0\n");
        return 1;
    }
    // printf("Flushed on step %lld\n", controller->timeIndex);
    if (close(bufferDescriptor)) {
        fprintf(stderr, "Failed to close /dev/fb0\n");
        return -2;
    }
    return 0;
}
