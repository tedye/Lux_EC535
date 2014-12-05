#include "controls.h"
#include <math.h>
#include <iostream>

controls::controls()
{
    turner = new motor(500,"/dev/motor");
    calib = 250; // according to wikipedia
}

int controls::calculate(double iso, double aperture, double shutter, double exposure, double Lux)
{
    double luxRequired;
    double diff;
    int ret;
    luxRequired = ((calib)*pow(aperture,2.0)/(pow(shutter,-1)*iso)) + pow(2.0,exposure);
    diff = luxRequired - Lux;
    printf("LuxRequired: %f, Diff: %f\n",luxRequired,diff);
    if(diff < 0)
    {
        do
        {
            ret = turner->moveMotor('b',50);
            printf("ret: %d\n",ret);
            // stub in function to get Lux from sensor
            // diff = luxRequired - func()
        }while(diff < 100 && !ret);
    }
    else
    {
        do
        {
           ret = turner->moveMotor('f',50);
           printf("ret: %d\n",ret);
            // stub in function to get Lux from sensor
            // diff = luxRequired - func()
        }while(diff > 100 & !ret);
    }
    printf("ret: %d\n",ret);
}
