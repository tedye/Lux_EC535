#include "light.h"
#include <stdlib.h>
#include <cstring>

light::light()
{
    this->proc = fopen("/proc/fortune","r");
}

light::~light()
{
    fclose(this->proc);
}

int light::getLux()
{
    char buffer[10];
    fread(buffer,10,1,this->proc);
    return atoi(buffer);
}
