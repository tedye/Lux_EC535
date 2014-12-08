#include "meterWidget.h"
#include "ui_widget.h"
#include <QIcon>
#include <QPixmap>
#include "adjusters.h"
// added by Ted
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <math.h>
using namespace std;

 meterWidget::meterWidget(QWidget *parent) :
    QWidget(parent)
{
    //added by Ted
    cout << "creating widget..."<< endl;
	// stop adding
    iso = new Adjusters("iso");            
    aperture = new Adjusters("aperture"); 
    calculate = new QPushButton("Calculate");
    swap = new QPushButton(QIcon(QPixmap(":/Images/lamp.png")),"",0);
    shutter = new QLabel("press calculate\n");
    //alert->setText("Lux Value too high!");

    overAll = new QVBoxLayout(this);
    boxBox = new QHBoxLayout();
    boxBox->addLayout(iso);
    boxBox->addLayout(aperture);
    boxBox->addWidget(shutter);
    overAll->addWidget(swap);
    overAll->addLayout(boxBox);
    overAll->addWidget(calculate);
    overAll->setSpacing(10);
    swap->setIconSize(QSize(50,50));
    this->setStyleSheet("background-color: white;");

	// connect signal and slot
    connect(this->calculate,SIGNAL(released()),this,SLOT(calculation()));
    connect(this->swap,SIGNAL(released()),this,SLOT(swapMode()));

    showFullScreen();

	// stop adding
    cout << "widget creation complete..." << endl;
}

meterWidget::~meterWidget()
{

}

void meterWidget::swapMode(){

    close();

}

void meterWidget::calculation(){
    int lux, shutterSpeed, ISO, aperture;
    char buffer[30];
    char bf[6];
    this->sensor = fopen("/proc/fortune","r");
    fread(bf,6,1,this->sensor);
    sscanf(bf,"%d",&lux);
    fclose(this->sensor);
    ISO = (int) this->iso->current;
    aperture = (int) this->aperture->current;
    double divisor =(((double)lux)*((double)ISO));
    double numerator =(250.0*pow((double)aperture,2.0));
    printf("divsor %d, numerator %d\n",divisor,numerator);
    shutterSpeed = (int) pow(numerator/divisor,-1.0);

    sprintf(buffer,"Shutter: 1/%d",shutterSpeed);

    this->shutter->setText(buffer);
}