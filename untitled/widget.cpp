#include "widget.h"
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
using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    //added by Ted
	thread_alive = false;
	// stop adding
	
    ui->setupUi(this);
    iso = new Adjusters("iso");         
    shutter = new Adjusters("shutter");   
    aperture = new Adjusters("aperture"); 
    exposure = new Adjusters("exposure"); 
    overAll = new QVBoxLayout(this);
    boxBox = new QHBoxLayout();
    boxBox->addLayout(iso);
    boxBox->addLayout(shutter);
    boxBox->addLayout(aperture);
    boxBox->addLayout(exposure);
    overAll->addLayout(boxBox);
    overAll->setSpacing(10);
    this->setStyleSheet("background-color: white;");

	// connect signal and slot
	connect(this->iso->top,SIGNAL(released()),this,SLOT(setTimer()));
	connect(this->iso->bottom,SIGNAL(released()),this,SLOT(setTimer()));
	connect(this->aperture->top,SIGNAL(released()),this,SLOT(setTimer()));
	connect(this->aperture->bottom,SIGNAL(released()),this,SLOT(setTimer()));
	connect(this->shutter->top,SIGNAL(released()),this,SLOT(setTimer()));
	connect(this->shutter->bottom,SIGNAL(released()),this,SLOT(setTimer()));
	connect(this->exposure->top,SIGNAL(released()),this,SLOT(setTimer()));
	connect(this->exposure->bottom,SIGNAL(released()),this,SLOT(setTimer()));
	// stop adding
}

Widget::~Widget()
{
    delete ui;
}

// added by Ted

void Widget::setParameter()
{
	thread_alive = true;
	sleep(3);
    // read lux reading from /proc/fortune
    FILE * pFile = fopen("/proc/fortune", "r");
    char content[10] = {0};
    int lux;
    fread(content,10,1,pFile);
    sscanf(content,"%d",&lux);
    fclose(pFile);
    // motor control
    /*motor_control->calculate(iso->current, 
                            aperture->current,
                            shutter->current,
                            exposure->current,
                            lux);
	*/
	cout << "lux: "<< lux<<endl;
	thread_alive = false;
}

void * Widget::staticEntryPoint(void *c){
	((Widget *)c)->setParameter();
	return NULL;
}

void Widget::setTimer(){
	if (thread_alive){
		pthread_cancel(thread);
		thread_alive = false;
	}
	int rc;
	rc = pthread_create(&thread,NULL, Widget::staticEntryPoint,this);
	
	
}
