#ifndef CONTROLS_H
#define CONTROLS_H

#include "motor.h"

class controls
{
public:
    controls();
    ~controls();
    int calculate(double iso, double aperture, double shutter, double exposure, int Lux);

private:
    motor *turner;
    int calib;
};

#endif // CONTROLS_H
