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
    cout << "creating widget..."<< endl;
	thread_alive = false;
    motor_control = new controls();
	// stop adding
	
    ui->setupUi(this);
    iso = new Adjusters("iso");         
    shutter = new Adjusters("shutter");   
    aperture = new Adjusters("aperture"); 
    exposure = new Adjusters("exposure"); 
    calibrate = new QPushButton("Calibrate");
    alert = new QMessageBox();
    alert->setText("Lux Value too high!");

    overAll = new QVBoxLayout(this);
    boxBox = new QHBoxLayout();
    boxBox->addLayout(iso);
    boxBox->addLayout(shutter);
    boxBox->addLayout(aperture);
    boxBox->addLayout(exposure);
    overAll->addLayout(boxBox);
    overAll->addWidget(calibrate);
    overAll->setSpacing(10);
    this->setStyleSheet("background-color: white;");

	// connect signal and slot
	connect(this->calibrate,SIGNAL(released()),this,SLOT(setTimer()));
    connect(this->motor_control,SIGNAL(errorOccurs(int)),this,SLOT(showDialog(int)));

    //showFullScreen();

	// stop adding
    cout << "widget creation complete..." << endl;
}

Widget::~Widget()
{
    delete ui;
}

// added by Ted

void Widget::setParameter()
{
	int lux;
    thread_alive = true;
	sleep(3);
    // read lux reading from /proc/fortune
    lux = motor_control->calculate( iso->current, 
                                    aperture->current,
                                    shutter->current,
                                    exposure->current);


 

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

void Widget::showDialog(int luxRequired){
    if (luxRequired > 100)
        this->alert->show();
}
