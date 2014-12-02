#include "motor.h"
#include <stdlib.h>
#include <cstring>
motor::motor(int max, const char* device)
{
    char buffR[4];
    char buffW[5];
    this->dev = fopen(device,"rw");
    this->save = fopen("/etc/lux/stepsData","r");
    if(!this->save)
    {
        this->steps = 0;
    }
    else
    {
        fread(buffR,4,1,this->save);
        this->steps = atoi(buffR);
        buffW[0] = 'w';
        strcpy(buffW+1,buffR);
        fwrite(buffW,5,1,dev);
    }
    fclose(this->save);
    this->maxStep = max;
}

int motor::getSteps()
{
    return steps;
}

motor::~motor()
{
    char buffer[10];

    fopen("/etc/lux/stepsData","w");
    // write current position to file
    fread(buffer,10,1,this->dev);
    fwrite(buffer,10,1,save);

    fclose(this->dev);
    fclose(this->save);
}

bool motor::moveMotor(char dir, int step)
{
    // commanding the motor
    // EX: 'f10' - foward 10 steps
    // EX: 'b50' - backwards 50 steps
    char command[4];
    command[0] = dir;
    itoa(step,command+1,10);
    if(fwrite(command,4,1,dev) != 4)
    {
        return false;
    }
    return true;
}
