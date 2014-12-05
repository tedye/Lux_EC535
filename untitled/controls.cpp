#include "controls.h"
#include <math.h>

controls::controls()
{
    turner = new motor(500,"/dev/motor");
    calib = 250; // according to wikipedia
}

int controls::calculate(double iso, double aperture, double shutter, double exposure, int Lux)
{
    int luxRequired;
    int diff;
    int ret;
    luxRequired = ((calib)*pow(aperture,2)/(shutter*iso)) + pow(2,exposure);
    diff = luxRequired - Lux;

    if(diff < 0)
    {
        do
        {
            ret = turner->moveMotor('b',5);
            // stub in function to get Lux from sensor
            // diff = luxRequired - func()
        }while(diff < 100 && !ret);
    }
    else
    {
        do
        {
           ret = turner->moveMotor('f',5);
            // stub in function to get Lux from sensor
            // diff = luxRequired - func()
        }while(diff > 100 & !ret);
    }
}
