#include "controls.h"
#include <math.h>
#include <iostream>

controls::controls()
{
    turner = new motor(530,"/dev/motor");
    calib = 250.0; // according to wikipedia
    
}

int controls::calculate(double iso, double aperture, double shutter, double exposure)
{
    double luxRequired;
    int diff;
    int ret, lux;
    bool okay = true;
    char bf[6];
    this->sensor = fopen("/proc/fortune","r");
    fread(bf,6,1,this->sensor);
    sscanf(bf,"%d",&lux);
    fclose(sensor);
    luxRequired = ((calib)*pow(aperture,2.0)/(pow(shutter,-1)*iso)) + pow(2.0,exposure);
    diff = (int) (luxRequired - lux);
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
            diff = (int) (luxRequired - lux);
            std::cout<<"lux: "<< lux<<" diff:" << diff<< std::endl;
            printf("steps %d\n",turner->steps);
            
        }while(diff > 100 && (turner->steps != turner->maxStep));
        if(diff > 100)
            okay = false;
    }
    // moving backwards
    else
    {
        do
        {
           ret = turner->moveMotor('f',10);
           usleep(3*1E5L);
           if(ret != -5)
                turner->steps -= 5;
           printf("ret: %d\n",ret);
            // stub in function to get Lux from sensor
            // diff = luxRequired - func()
            this->sensor = fopen("/proc/fortune","r");
            fread(bf,6,1,this->sensor);
            sscanf(bf,"%d",&lux);
            fclose(sensor);
            diff = (int) (luxRequired - lux);
            std::cout<<"lux: "<< lux<<" diff:" << diff<< std::endl;
            printf("steps %d\n",turner->steps);
        }while(diff < (-100) && (turner->steps != 0));
        if(diff < (-100))
            okay = false;
    }
    printf("ret: %d\n",ret);
    printf("total step: %d", turner->steps);

    // save block .////////////////////
    char buffer[10];

    FILE *sav = fopen("/etc/lux/stepsData","w");
    // write current position to file
    sprintf(buffer,"%d",this->turner->steps);
    printf("buffer %s\n",buffer);
    fwrite(buffer,10,1,sav);
    fclose(sav);
///////////////////////////////////////////

    emit errorOccurs(turner->steps, turner->maxStep,okay);
}

controls::~controls()
{
    fclose(this->sensor);
    delete(this->turner);
}
