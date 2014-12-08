#ifndef CONTROLS_H
#define CONTROLS_H

#include "motor.h"
#include <stdio.h>
#include <QObject>

class controls:public QObject
{	Q_OBJECT
public:
    controls();
    ~controls();
    int calculate(double iso, double aperture, double shutter, double exposure);

signals:
  	void errorOccurs(int steps, int stepsMax, bool okay);

private:
    motor *turner;
    double calib;
    FILE* sensor;
};

#endif // CONTROLS_H
