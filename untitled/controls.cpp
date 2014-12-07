#include "controls.h"
#include <math.h>
#include <iostream>

controls::controls()
{
    turner = new motor(510,"/dev/motor");
    calib = 250; // according to wikipedia
    
}

int controls::calculate(double iso, double aperture, double shutter, double exposure)
{
    int luxRequired;
    int diff;
    int ret, lux;
    char bf[6];
    this->sensor = fopen("/proc/fortune","r");
    fread(bf,6,1,this->sensor);
    sscanf(bf,"%d",&lux);
    fclose(sensor);
    luxRequired = ((calib)*pow(aperture,2.0)/(pow(shutter,-1)*iso)) + pow(2.0,exposure);
    diff = luxRequired - lux;
    printf("LuxRequired: %f, Diff: %d\n",luxRequired,diff);
    //moving forwards
    if(diff > 0)
    {
        do
        {
            ret = turner->moveMotor('b',10);
            usleep(3*1E5L);
            if(ret != -5)
                turner->steps += 10;
            /*else
                return -99;*/
            printf("ret: %d\n",ret);
            // stub in function to get Lux from sensor
            // diff = luxRequired - func()
            this->sensor = fopen("/proc/fortune","r");
            fread(bf,6,1,this->sensor);
            sscanf(bf,"%d",&lux);
            fclose(sensor);
            diff = luxRequired - lux;
            std::cout<<"lux: "<< lux<<" diff:" << diff<< std::endl;
            printf("steps %d\n",turner->steps);
            
        }while(diff > 100 && (turner->steps != turner->maxStep));
    }
    // moving backwards
    else
    {
        do
        {
           ret = turner->moveMotor('f',10);
           usleep(3*1E5L);
           if(ret != -5)
                turner->steps -= 10;
           printf("ret: %d\n",ret);
            // stub in function to get Lux from sensor
            // diff = luxRequired - func()
            this->sensor = fopen("/proc/fortune","r");
            fread(bf,6,1,this->sensor);
            sscanf(bf,"%d",&lux);
            fclose(sensor);
            diff = luxRequired - lux;
            std::cout<<"lux: "<< lux<<" diff:" << diff<< std::endl;
            printf("steps %d\n",turner->steps);
        }while(diff < (-100) && (turner->steps != 0));
    }
    printf("ret: %d\n",ret);
    printf("total step: %d", turner->steps);

    emit errorOccurs(luxRequired);
}

controls::~controls()
{
    fclose(this->sensor);
    delete(this->turner);
}
