#ifndef MOTOR_H
#define MOTOR_H

#include <stdio.h>

/*
	Max steps for this motor is 1100
*/

class motor
{
public:
    motor(int max, char* device);
    int moveMotor(char dir,int step);
    int getSteps();
    ~motor();
private:
    int steps; // steps away from the off position
    int maxStep; // light switch does not do a full rotation
    char *deviceName;
    FILE* dev;
    FILE* save;
};

#endif // MOTOR_H
