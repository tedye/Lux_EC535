#ifndef CONTROLS_H
#define CONTROLS_H

#include "motor.h"

class controls
{
public:
    controls();
    ~controls();
    int calculate(double iso, double aperture, double shutter, double exposure, double Lux);

private:
    motor *turner;
    double calib;
};

#endif // CONTROLS_H
