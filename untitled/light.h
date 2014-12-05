#ifndef LIGHT_H
#define LIGHT_H

#include <stdio.h>

class light
{
public:
    light();
    ~light();
    int getLux();
private:
    FILE *proc;
};

#endif // LIGHT_H
