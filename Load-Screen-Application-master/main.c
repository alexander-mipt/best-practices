#include "controller.h"
#include "defines.h"
#include <stdlib.h>


int main(int argc, char **argv) {
    int numberOfSpirals = DEFAULT_NUMBER_OF_SPIRALS;
    if (argc == 2) {
        numberOfSpirals = atoi(argv[1]);
    }

    Controller controller = createController(numberOfSpirals);
    showLoadScreen(&controller);
    deleteController(&controller);
    return 0;
}
