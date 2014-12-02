#ifndef MOTOR_H
#define MOTOR_H

#include <stdio.h>

class motor
{
public:
    motor(int max, const char* device);
    bool moveMotor(char dir,int step);
    int getSteps();
    ~motor();
private:
    int steps;
    int maxStep; // light switch does not do a full rotation
    FILE* dev;
    FILE* save;
};

#endif // MOTOR_H
