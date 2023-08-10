#ifndef POLEPLACEMENT_H
#define POLEPLACEMENT_H

#include "Controller.h"

class PolePlacement : Controller{
    float desired_state[4];
    float K[4];

void set_desired_state(float state[4]);

void run();
};

#endif